#include "hello_async.hpp"
#include "../cpu_intensive_task.hpp"
#include "../module_utils.hpp"

#include <exception>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>

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

/*
struct AsyncHelloWorker : Napi::AsyncWorker
{
    using Base = Napi::AsyncWorker;
    // ctor
    AsyncHelloWorker(bool louder,
                     bool buffer,
                     std::string name,
                     Napi::Function const& cb)
        : Base(cb),
          louder_(louder),
          buffer_(buffer),
          name_(std::move(name)) {}

    // The Execute() function is getting called when the worker starts to run.
    // - You only have access to member variables stored in this worker.
    // - You do not have access to Javascript v8 objects here.
    void Execute() override
    {
        try
        {
            result_ = detail::do_expensive_work(name_, louder_);
        }
        catch (std::exception const& e)
        {
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
    void OnOK() override
    {
        if (!Callback().IsEmpty())
        {
            if (buffer_)
            {
                char * data = result_->data();
                std::size_t size = result_->size();
                auto buffer = Napi::Buffer<char>::New(Env(),
                                                      data,
                                                      size,
                                                      [](Napi::Env, char*, gsl::owner<std::vector<char>*> v) {
                                                          delete v;
                                                      },
                                                      result_.release());
                Callback().Call({Env().Null(), buffer});
            }
            else
            {
                Callback().Call({Env().Null(), Napi::String::New(Env(), result_->data(), result_->size())});
            }
        }
    }

    std::unique_ptr<std::vector<char>> result_ = nullptr;
    bool const louder_;
    bool const buffer_;
    std::string const name_;
};
*/

// This V2 worker is overriding `GetResult` to return the arguments
// passed to the Callback invoked by the default OnOK() implementation.
// Above is alternative implementation with OnOK() method calling
// Callback with appropriate args. Both implementations use default OnError().
struct AsyncHelloWorker_v2 : Napi::AsyncWorker
{
    using Base = Napi::AsyncWorker;
    // ctor
    AsyncHelloWorker_v2(bool louder,
                        bool buffer,
                        std::string name,
                        Napi::Function const& cb)
        : Base(cb),
          louder_(louder),
          buffer_(buffer),
          name_(std::move(name)) {}

    // The Execute() function is getting called when the worker starts to run.
    // - You only have access to member variables stored in this worker.
    // - You do not have access to Javascript v8 objects here.
    void Execute() override
    {
        try
        {
            result_ = detail::do_expensive_work(name_, louder_);
        }
        catch (std::exception const& e)
        {
            SetError(e.what());
        }
    }

    std::vector<napi_value> GetResult(Napi::Env env) override
    {
        if (result_)
        {
            if (buffer_)
            {
                char* data = result_->data();
                std::size_t size = result_->size();
                auto buffer = Napi::Buffer<char>::New(
                    env,
                    data,
                    size,
                    [](Napi::Env /*unused*/, char* /*unused*/, gsl::owner<std::vector<char>*> v) {
                        delete v;
                    },
                    result_.release());
                return {env.Null(), buffer};
            }
            return {env.Null(), Napi::String::New(env, result_->data(), result_->size())};
        }
        return Base::GetResult(env); // returns an empty vector (default)
    }

    std::unique_ptr<std::vector<char>> result_ = nullptr;
    bool const louder_;
    bool const buffer_;
    std::string const name_;
};

Napi::FunctionReference HelloObjectAsync::constructor; // NOLINT

HelloObjectAsync::HelloObjectAsync(Napi::CallbackInfo const& info)
    : Napi::ObjectWrap<HelloObjectAsync>(info)
{
    Napi::Env env = info.Env();
    std::size_t length = info.Length();
    if (length != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return;
    }
    name_ = info[0].As<Napi::String>();
    // ^^ uses std::string() operator to convert to UTF-8 encoded string
    // alternatively Utf8Value() method can be used e.g
    // name_ = info[0].As<Napi::String>().Utf8Value();
    if (name_.empty())
    {
        Napi::TypeError::New(env, "arg must be a non-empty string").ThrowAsJavaScriptException();
    }
}

Napi::Value HelloObjectAsync::helloAsync(Napi::CallbackInfo const& info)
{
    bool louder = false;
    bool buffer = false;

    Napi::Env env = info.Env();
    if (!(info.Length() == 2 && info[1].IsFunction()))
    {
        Napi::TypeError::New(env, "second arg 'callback' must be a function").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Function callback = info[1].As<Napi::Function>();

    // Check first argument, should be an 'options' object
    if (!info[0].IsObject())
    {
        return utils::CallbackError(env, "first arg 'options' must be an object", callback);
    }
    Napi::Object options = info[0].As<Napi::Object>();

    // Check options object for the "louder" property, which should be a boolean
    // value
    if (options.Has(Napi::String::New(env, "louder")))
    {
        Napi::Value louder_val = options.Get(Napi::String::New(env, "louder"));
        if (!louder_val.IsBoolean())
        {
            return utils::CallbackError(env, "option 'louder' must be a boolean", callback);
        }
        louder = louder_val.As<Napi::Boolean>().Value();
    }
    // Check options object for the "buffer" property, which should be a boolean
    // value
    if (options.Has(Napi::String::New(env, "buffer")))
    {
        Napi::Value buffer_val = options.Get(Napi::String::New(env, "buffer"));
        if (!buffer_val.IsBoolean())
        {
            return utils::CallbackError(env, "option 'buffer' must be a boolean", callback);
        }
        buffer = buffer_val.As<Napi::Boolean>().Value();
    }

    auto* worker = new AsyncHelloWorker_v2{louder, buffer, name_, callback}; // NOLINT
    worker->Queue();
    return info.Env().Undefined(); // NOLINT
}

Napi::Object HelloObjectAsync::Init(Napi::Env env, Napi::Object exports)
{
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
