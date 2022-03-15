#include "hello_promise.hpp"

#include <iostream>
#include <utility>

namespace standalone_promise {

// async worker that handles the Deferred methods
struct PromiseWorker : Napi::AsyncWorker
{
    // constructor / ctor
    PromiseWorker(Napi::Env const& env, std::string phrase, int multiply)
        : Napi::AsyncWorker(env),
          phrase_(std::move(phrase)),
          multiply_(multiply),
          deferred_(Napi::Promise::Deferred::New(env)) {}

    // The Execute() function is getting called when the worker starts to run.
    // - You only have access to member variables stored in this worker.
    // - You do not have access to Javascript v8 objects here.
    void Execute() override
    {
        for (int i = 0; i < multiply_; ++i)
        {
            output_ += phrase_;
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
        deferred_.Resolve(Napi::String::New(Env(), output_));
    }

    // If anything in the PromiseWorker.Execute method throws an error
    // it will be caught here and rejected.
    void OnError(Napi::Error const& error) override
    {
        deferred_.Reject(error.Value());
    }

    Napi::Promise GetPromise() const
    {
        return deferred_.Promise();
    }

    const std::string phrase_;
    const int multiply_;
    Napi::Promise::Deferred deferred_;
    std::string output_ = "";
};

// entry point
Napi::Value helloPromise(Napi::CallbackInfo const& info)
{
    Napi::Env env = info.Env();

    // default params
    std::string phrase = "hello";
    int multiply = 1;

    // validate inputs
    // - if params is defined, validate contents
    // - - params is an object
    // - - params.multiply is int and greater than zero
    // - - params.phrase is string
    // - otherwise skip and use defaults
    if (!info[0].IsUndefined())
    {
        if (!info[0].IsObject())
        {
            throw Napi::Error::New(env, "options must be an object");
        }
        Napi::Object options = info[0].As<Napi::Object>();

        // phrase must be a string
        if (options.Has(Napi::String::New(env, "phrase")))
        {
            Napi::Value phrase_val = options.Get(Napi::String::New(env, "phrase"));
            if (!phrase_val.IsString())
            {
                throw Napi::Error::New(env, "options.phrase must be a string");
            }
            phrase = phrase_val.As<Napi::String>();
        }

        // multiply must be int > 1
        if (options.Has(Napi::String::New(env, "multiply")))
        {
            Napi::Value multiply_val = options.Get(Napi::String::New(env, "multiply"));
            if (!multiply_val.IsNumber())
            {
                throw Napi::Error::New(env, "options.multiply must be a number");
            }
            multiply = multiply_val.As<Napi::Number>().Int32Value();
            if (multiply < 1)
            {
                throw Napi::Error::New(env, "options.multiply must be 1 or greater");
            }
        }
    }

    // initialize Napi::AsyncWorker
    // This comes with a GetPromise that returns the necessary
    // Napi::Promise::Deferred class that we send the user
    auto* worker = new PromiseWorker{env, phrase, multiply};
    auto promise = worker->GetPromise();

    // begin asynchronous work by queueing it.
    // https://github.com/nodejs/node-addon-api/blob/main/doc/async_worker.md#queue
    worker->Queue();

    // return the deferred promise to the user. Let the
    // async worker resolve/reject accordingly
    return promise;
}

} // namespace standalone_promise