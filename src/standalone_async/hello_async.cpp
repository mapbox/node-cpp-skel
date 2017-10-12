#include "hello_async.hpp"
#include "../module_utils.hpp"

#include <exception>
#include <iostream>
#include <map>
#include <stdexcept>

/**
 * This is an asynchronous standalone function that logs a string.
 * @name helloAsync
 * @param {Object} args - different ways to alter the string
 * @param {boolean} args.louder - adds exclamation points to the string
 * @param {Function} callback - from whence the hello comes, returns a string
 * @returns {string}
 * @example
 * var module = require('./path/to/lib/index.js');
 * module.helloAsync({ louder: true }, function(err, result) {
 *   if (err) throw err;
 *   console.log(result); // => "...threads are busy async bees...hello
 * world!!!!"
 * });
 */

// If this was not defined within a namespace, it would be in the global scope.
// Namespaces are used because C++ has no notion of scoped modules, so all of
// the code you write in any file could conflict with other code.
// Namespaces are generally a great idea in C++ because it helps scale and
// clearly organize your application.
namespace standalone_async {

// Expensive allocation of std::map, querying, and string comparison,
// therefore threads are busy
std::string do_expensive_work(bool louder) {

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

    std::string result = "...threads are busy async bees...hello world";

    if (louder) {
        result += "!!!!";
    }

    return result;
}

// This is the worker running asynchronously and calling a user-provided
// callback when done.
// Consider storing all C++ objects you need by value or by shared_ptr to keep
// them alive until done.
// Nan AsyncWorker docs:
// https://github.com/nodejs/nan/blob/master/doc/asyncworker.md
struct AsyncHelloWorker : Nan::AsyncWorker {
    using Base = Nan::AsyncWorker;

    AsyncHelloWorker(bool louder, Nan::Callback* cb)
        : Base(cb), result_{""}, louder_{louder} {}

    // The Execute() function is getting called when the worker starts to run.
    // - You only have access to member variables stored in this worker.
    // - You do not have access to Javascript v8 objects here.
    void Execute() override {
        // The try/catch is critical here: if code was added that could throw an
        // unhandled error INSIDE the threadpool, it would be disasterous
        try {
            result_ = do_expensive_work(louder_);
        } catch (const std::exception& e) {
            SetErrorMessage(e.what());
        }
    }

    // The HandleOKCallback() is getting called when Execute() successfully
    // completed.
    // - In case Execute() invoked SetErrorMessage("") this function is not
    // getting called.
    // - You have access to Javascript v8 objects again
    // - You have to translate from C++ member variables to Javascript v8 objects
    // - Finally, you call the user's callback with your results
    void HandleOKCallback() override {
        Nan::HandleScope scope;

        const auto argc = 2u;
        v8::Local<v8::Value> argv[argc] = {
            Nan::Null(), Nan::New<v8::String>(result_).ToLocalChecked()};

        // Static cast done here to avoid 'cppcoreguidelines-pro-bounds-array-to-pointer-decay' warning with clang-tidy
        callback->Call(argc, static_cast<v8::Local<v8::Value>*>(argv));
    }

    std::string result_;
    const bool louder_;
};

// helloAsync is a "standalone function" because it's not a class.
// If this function was not defined within a namespace ("standalone_async"
// specified above), it would be in the global scope.
NAN_METHOD(helloAsync) {

    bool louder = false;

    // Check second argument, should be a 'callback' function.
    // This allows us to set the callback so we can use it to return errors
    // instead of throwing.
    // Also, "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    if (!info[1]->IsFunction()) {
        return Nan::ThrowTypeError("second arg 'callback' must be a function");
    }
    v8::Local<v8::Function> callback = info[1].As<v8::Function>();

    // Check first argument, should be an 'options' object
    if (!info[0]->IsObject()) {
        return utils::CallbackError("first arg 'options' must be an object",
                                    callback);
    }
    v8::Local<v8::Object> options = info[0].As<v8::Object>();

    // Check options object for the "louder" property, which should be a boolean
    // value
    if (options->Has(Nan::New("louder").ToLocalChecked())) {
        v8::Local<v8::Value> louder_val =
            options->Get(Nan::New("louder").ToLocalChecked());
        if (!louder_val->IsBoolean()) {
            return utils::CallbackError("option 'louder' must be a boolean",
                                        callback);
        }
        louder = louder_val->BooleanValue();
    }

    // Creates a worker instance and queues it to run asynchronously, invoking the
    // callback when done.
    // - Nan::AsyncWorker takes a pointer to a Nan::Callback and deletes the
    // pointer automatically.
    // - Nan::AsyncQueueWorker takes a pointer to a Nan::AsyncWorker and deletes
    // the pointer automatically.
    auto* worker = new AsyncHelloWorker{louder, new Nan::Callback{callback}};
    Nan::AsyncQueueWorker(worker);
}

} // namespace standalone_async
