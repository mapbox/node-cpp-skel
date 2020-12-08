#include "hello_async.hpp"
#include "../cpu_intensive_task.hpp"
#include "../module_utils.hpp"

#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

/**
 * This is an asynchronous standalone function that logs a string.
 * @name helloAsync
 * @param {Object} args - different ways to alter the string
 * @param {boolean} args.louder - adds exclamation points to the string
 * @param {boolean} args.buffer - returns value as a node buffer rather than a string
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

namespace standalone_async {

// This is the worker running asynchronously and calling a user-provided
// callback when done.
// Consider storing all C++ objects you need by value or by shared_ptr to keep
// them alive until done.
// Napi::AsyncWorker docs:
// https://github.com/nodejs/node-addon-api/blob/master/doc/async_worker.md
struct AsyncHelloWorker : Napi::AsyncWorker
{
    using Base = Napi::AsyncWorker;
    // ctor
    AsyncHelloWorker(bool louder, bool buffer, Napi::Function const& cb)
        : Base(cb),
          louder_(louder),
          buffer_(buffer) {}

    // The Execute() function is getting called when the worker starts to run.
    // - You only have access to member variables stored in this worker.
    // - You do not have access to Javascript v8 objects here.
    void Execute() override
    {
        // The try/catch is critical here: if code was added that could throw an
        // unhandled error INSIDE the threadpool, it would be disasterous
        try
        {
            result_ = detail::do_expensive_work("world", louder_);
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
    void OnOK() final
    {
        if (!Callback().IsEmpty() && result_)
        {
            if (buffer_)
            {
                char* data = result_->data();
                std::size_t size = result_->size();
                auto buffer = Napi::Buffer<char>::New(
                    Env(),
                    data,
                    size,
                    [](Napi::Env /*unused*/, char* /*unused*/, gsl::owner<std::vector<char>*> v) {
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
    const bool louder_;
    const bool buffer_;
};

// helloAsync is a "standalone function" because it's not a class.
// If this function was not defined within a namespace ("standalone_async"
// specified above), it would be in the global scope.
Napi::Value helloAsync(Napi::CallbackInfo const& info)
{
    bool louder = false;
    bool buffer = false;
    Napi::Env env = info.Env();
    // Check second argument, should be a 'callback' function.
    if (!info[1].IsFunction())
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
    // Check options object for the "buffer" property, which should be a boolean value
    if (options.Has(Napi::String::New(env, "buffer")))
    {
        Napi::Value buffer_val = options.Get(Napi::String::New(env, "buffer"));
        if (!buffer_val.IsBoolean())
        {
            return utils::CallbackError(env, "option 'buffer' must be a boolean", callback);
        }
        buffer = buffer_val.As<Napi::Boolean>().Value();
    }

    // Creates a worker instance and queues it to run asynchronously, invoking the
    // callback when done.
    // - Napi::AsyncWorker takes a pointer to a Napi::FunctionReference and deletes the
    // pointer automatically.
    // - Napi::AsyncQueueWorker takes a pointer to a Napi::AsyncWorker and deletes
    // the pointer automatically.
    auto* worker = new AsyncHelloWorker{louder, buffer, callback}; // NOLINT
    worker->Queue();
    return env.Undefined(); // NOLINT
}

} // namespace standalone_async
