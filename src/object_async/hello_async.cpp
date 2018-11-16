#include "hello_async.hpp"
#include "../module_utils.hpp"

#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

/**
 * Asynchronous class, called HelloObjectAsync
 * @class HelloObjectAsync
 * @example
 * var module = require('index.js');
 * var Obj = new module.HelloObjectAsync('greg');
 */

/**
 * Say hello while doing expensive work in threads
 *
 * @name helloAsync
 * @memberof HelloObjectAsync
 * @param {Object} args - different ways to alter the string
 * @param {boolean} args.louder - adds exclamation points to the string
 * @param {buffer} args.buffer - returns object as a node buffer rather then string
 * @param {Function} callback - from whence the hello comes, returns a string
 * @returns {String}
 * @example
 * var module = require('index.js');
 * var Obj = new module.HelloObjectAsync('greg');
 * Obj.helloAsync({ louder: true }, function(err, result) {
 *   if (err) throw err;
 *   console.log(result); // => '...threads are busy async bees...hello greg!!!'
 * });
 */

// If this was not defined within a namespace, it would be in the global scope.
namespace object_async {

// This is the worker running asynchronously and calling a user-provided
// callback when done.
// Consider storing all C++ objects you need by value or by shared_ptr to keep
// them alive until done.
// Nan AsyncWorker docs:
// https://github.com/nodejs/nan/blob/master/doc/asyncworker.md

// This function performs expensive allocation of std::map, querying, and string
// comparison, therefore threads are nice & busy.
// Also, notice that name is passed by reference (std::string const& name)
std::unique_ptr<std::string> do_expensive_work(bool louder, std::string const& name) {

    std::map<std::size_t, std::string> container;
    std::size_t work_to_do = 100000;

    for (std::size_t i = 0; i < work_to_do; ++i) {
        container.emplace(i, std::to_string(i));
    }

    for (std::size_t i = 0; i < work_to_do; ++i) {
        std::string const& item = container[i];

        if (item != std::to_string(i)) {

            // AsyncHelloWorker's Execute function will take care of this error
            // and return it to js-world via callback
            // Marked NOLINT to avoid clang-tidy cert-err60-cpp error which we cannot
            // avoid on some linux distros where std::runtime_error is not properly
            // marked noexcept. Details at https://www.securecoding.cert.org/confluence/display/cplusplus/ERR60-CPP.+Exception+objects+must+be+nothrow+copy+constructible
            throw std::runtime_error("Uh oh, this should never happen"); // NOLINT
        }
    }

    std::unique_ptr<std::string> result = std::make_unique<std::string>("...threads are busy async bees...hello " + name);

    if (louder) {
        *result += "!!!!";
    }

    return result;
}

struct AsyncHelloWorker : Napi::AsyncWorker // NOLINT to disable cppcoreguidelines-special-member-functions
{

    using Base = Napi::AsyncWorker;
    AsyncHelloWorker(AsyncHelloWorker const&) = delete;
    AsyncHelloWorker& operator=(AsyncHelloWorker const&) = delete;
    AsyncHelloWorker(bool louder,
                     bool buffer,
                     std::string const* name,
                     Napi::Function& cb)
        : Base(cb),
          louder_(louder),
          buffer_(buffer),
          name_(name) {}

    // The Execute() function is getting called when the worker starts to run.
    // - You only have access to member variables stored in this worker.
    // - You do not have access to Javascript v8 objects here.
    void Execute() override {
        try {
            result_ = do_expensive_work(louder_, *name_);
        } catch (std::exception const& e) {
            SetError(e.what());
        }
    }

    // The OnOK() is getting called when Execute() successfully
    // completed.
    // - In case Execute() invoked SetErrorMessage("") this function is not
    // getting called.
    // - You have access to Javascript v8 objects again
    // - You have to translate from C++ member variables to Javascript v8 objects
    // - Finally, you call the user's callback with your results
    void OnOK() override {
        Napi::HandleScope scope(Env());
        if (buffer_) {
            Callback().Call({Env().Null(), utils::NewBufferFrom(Env(), std::move(result_))});
        } else {
            Callback().Call({Env().Null(), Napi::String::New(Env(), *result_)});
        }
    }

    std::unique_ptr<std::string> result_ = std::make_unique<std::string>();
    bool const louder_;
    bool const buffer_;
    // We use a pointer here to avoid copying the string data.
    // This works because we know that the original string we are
    // pointing to will be kept in scope/alive for the time while AsyncHelloWorker
    // is using it. If we could not guarantee this then we would need to either
    // copy the string or pass a shared_ptr<std::string>.
    std::string const* name_;
};

Napi::FunctionReference HelloObjectAsync::constructor;

HelloObjectAsync::HelloObjectAsync(Napi::CallbackInfo const& info)
    : Napi::ObjectWrap<HelloObjectAsync>(info),
      name_{} {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::size_t length = info.Length();
    if (length != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }
    name_ = info[0].As<Napi::String>().Utf8Value();
    if (name_.empty()) {
        Napi::TypeError::New(env, "arg must be a non-empty string").ThrowAsJavaScriptException();
    }
}

// NAN_METHOD is applicable to methods you want to expose to JS world
Napi::Value HelloObjectAsync::helloAsync(Napi::CallbackInfo const& info) {
    // ????
    bool louder = false;
    bool buffer = false;
    // ????

    Napi::Env env = info.Env();
    if (info.Length() != 2 || !info[1].IsFunction()) {
        Napi::TypeError::New(env, "second arg 'callback' must be a function").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Function callback = info[1].As<Napi::Function>();

    // Check first argument, should be an 'options' object
    if (!info[0].IsObject()) {
        return utils::CallbackError("first arg 'options' must be an object", info);
    }
    Napi::Object options = info[0].As<Napi::Object>();

    // Check options object for the "louder" property, which should be a boolean
    // value
    if (options.Has(Napi::String::New(env, "louder"))) {
        Napi::Value louder_val = options.Get(Napi::String::New(env, "louder"));
        if (!louder_val.IsBoolean()) {
            return utils::CallbackError("option 'louder' must be a boolean", info);
        }
        louder = louder_val.As<Napi::Boolean>().Value();
    }
    // Check options object for the "buffer" property, which should be a boolean
    // value
    if (options.Has(Napi::String::New(env, "buffer"))) {
        Napi::Value buffer_val = options.Get(Napi::String::New(env, "buffer"));
        if (!buffer_val.IsBoolean()) {
            return utils::CallbackError("option 'buffer' must be a boolean", info);
        }
        buffer = buffer_val.As<Napi::Boolean>().Value();
    }

    auto* worker = new AsyncHelloWorker{louder, buffer, &name_, callback};
    worker->Queue();
    return info.Env().Undefined();
}

Napi::Object HelloObjectAsync::Init(Napi::Env env, Napi::Object exports) {

    Napi::Function func = DefineClass(env, "HelloObjectAsync", {InstanceMethod("helloAsync", &HelloObjectAsync::helloAsync)});
    // Create a peristent reference to the class constructor. This will allow
    // a function called on a class prototype and a function
    // called on instance of a class to be distinguished from each other.
    constructor = Napi::Persistent(func);
    // Call the SuppressDestruct() method on the static data prevent the calling
    // to this destructor to reset the reference when the environment is no longer
    // available.
    constructor.SuppressDestruct();
    exports.Set("HelloObjectAsync", func);
    return exports;
}

} // namespace object_async
