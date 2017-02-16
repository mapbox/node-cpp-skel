#include "hello_world.hpp"

#include <exception>
#include <stdexcept>
#include <iostream>

// Custom constructor added in order to test/cover throwing an error during initialization
HelloWorld::HelloWorld(std::string name) :
  name_(name) {
    if (name_ != "hello") {
        throw std::runtime_error("name must be 'hello'");
    }
}

/**
 * Main class, called HelloWorld
 * @class HelloWorld
 * @example
 * var HelloWorld = require('index.js');
 * var HW = new HelloWorld();
 *          OR
 * var HW = new HelloWorld('yo');
 */
NAN_METHOD(HelloWorld::New)
{
    if (info.IsConstructCall())
    {
        try
        {
            if (info.Length() >= 1) {
              if (info[0]->IsString())
              {
                std::string name = *v8::String::Utf8Value(info[0]->ToString());
                auto *const self = new HelloWorld(name);
                self->Wrap(info.This());
              }
              else
              {
                return Nan::ThrowTypeError(
                    "arg must be a string");
              }
            }
            else {
                auto *const self = new HelloWorld();
                self->Wrap(info.This());
            }

        }
        catch (const std::exception &ex)
        {
            return Nan::ThrowTypeError(ex.what());
        }

        info.GetReturnValue().Set(info.This());
    }
    else
    {
        return Nan::ThrowTypeError(
            "Cannot call constructor as function, you need to use 'new' keyword");
    }
}

Nan::Persistent<v8::Function> &HelloWorld::constructor()
{
    static Nan::Persistent<v8::Function> init;
    return init;
}

/*
 * This is an internal function used to return callback error messages instead of
 * throwing errors.
 * Usage:
 *
 * v8::Local<v8::Function> callback;
 * CallbackError("error message", callback);
 * return; // this is important to prevent duplicate callbacks from being fired!
 */
inline void CallbackError(std::string message, v8::Local<v8::Function> callback) {
    v8::Local<v8::Value> argv[1] = { Nan::Error(message.c_str()) };
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callback, 1, argv);
}

/**
 * Say howdy to the world
 *
 * @name wave
 * @memberof HelloWorld
 * @returns {String} a happy-go-lucky string saying hi
 * @example
 * var wave = HW.wave();
 * console.log(wave); // => 'howdy world!'
 */
NAN_METHOD(HelloWorld::wave)
{
    // info comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(Nan::New<v8::String>("howdy world").ToLocalChecked());
}

std::string do_expensive_work(std::string const& phrase, bool louder) {
    std::string result;

    // This is purely for testing, to be able to simulate an unexpected throw
    // from a function you do not control and may throw an exception
    if (phrase != "rawr") {
        throw std::runtime_error("we really would prefer rawr all the time");
    }

    result = phrase + "!";

    if (louder)
    {
        result += "!!!!";
    }
    return result;
}

// This is the worker running asynchronously and calling a user-provided callback when done.
// Consider storing all C++ objects you need by value or by shared_ptr to keep them alive until done.
struct AsyncShoutWorker : Nan::AsyncWorker {
    using Base = Nan::AsyncWorker;

    AsyncShoutWorker(std::string phrase, bool louder, Nan::Callback* callback)
        : Base(callback), phrase_(phrase), louder_(louder) { }

    // The Execute() function is getting called when the worker starts to run.
    // - You only have access to member variables stored in this worker.
    // - You do not have access to Javascript v8 objects here.
    void Execute() override {
        try {
            phrase_ = do_expensive_work(phrase_, louder_);
        } catch (const std::exception& e) {
            SetErrorMessage(e.what());
        }
    }

    // The HandleOKCallback() is getting called when Execute() successfully completed.
    // - In case Execute() invoked SetErrorMessage("") this function is not getting called.
    // - You have access to Javascript v8 objects again
    // - You have to translate from C++ member variables to Javascript v8 objects
    // - Finally, you call the user's callback with your results
    void HandleOKCallback() override {
        Nan::HandleScope scope;

        const auto argc = 2u;
        v8::Local<v8::Value> argv[argc] = {Nan::Null(), Nan::New<v8::String>(phrase_).ToLocalChecked()};

        callback->Call(argc, argv);
    }

    std::string phrase_;
    const bool louder_;
};

/**
 * Shout a phrase really loudly by adding an exclamation to the end, asynchronously
 *
 * @name shout
 * @memberof HelloWorld
 * @param {String} phrase to shout
 * @param {Object} different ways to shout
 * @param {Function} callback - from whence the shout comes, returns a string
 * @example
 * var HW = new HelloWorld();
 * HW.shout('rawr', {}, function(err, shout) {
 *   if (err) throw err;
 *   console.log(shout); // => 'rawr!'
 * });
 *
 */
NAN_METHOD(HelloWorld::shout)
{
    std::string phrase = "";
    bool louder = false;

    // check third argument, should be a 'callback' function.
    // This allows us to set the callback so we can use it to return errors
    // instead of throwing as well.
    if (!info[2]->IsFunction())
    {
        Nan::ThrowTypeError("third arg 'callback' must be a function");
        return;
    }
    v8::Local<v8::Function> callback = info[2].As<v8::Function>();

    // check first argument, should be a 'phrase' string
    if (!info[0]->IsString())
    {
        CallbackError("first arg 'phrase' must be a string", callback);
        return;
    }
    phrase = *v8::String::Utf8Value((info[0])->ToString());

    // check second argument, should be an 'options' object
    if (!info[1]->IsObject())
    {
        CallbackError("second arg 'options' must be an object", callback);
        return;
    }
    v8::Local<v8::Object> options = info[1].As<v8::Object>();

    if (options->Has(Nan::New("louder").ToLocalChecked()))
    {
        v8::Local<v8::Value> louder_val = options->Get(Nan::New("louder").ToLocalChecked());
        if (!louder_val->IsBoolean())
        {
            CallbackError("option 'louder' must be a boolean", callback);
            return;
        }
        louder = louder_val->BooleanValue();
    }

    // Create a worker instance and queues it to run asynchronously invoking the callback when done.
    // - Nan::AsyncWorker takes a pointer to a Nan::Callback and deletes the pointer automatically.
    // - Nan::AsyncQueueWorker takes a pointer to a Nan::AsyncWorker and deletes the pointer automatically.
    auto* worker = new AsyncShoutWorker{phrase, louder, new Nan::Callback{callback}};
    Nan::AsyncQueueWorker(worker);
}

NAN_MODULE_INIT(HelloWorld::Init)
{
    const auto whoami = Nan::New("HelloWorld").ToLocalChecked();

    auto fnTp = Nan::New<v8::FunctionTemplate>(New);
    fnTp->InstanceTemplate()->SetInternalFieldCount(1);
    fnTp->SetClassName(whoami);

    // custom methods added here
    SetPrototypeMethod(fnTp, "wave", wave);
    SetPrototypeMethod(fnTp, "shout", shout);

    const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
    constructor().Reset(fn);
    Nan::Set(target, whoami, fn);
}

/*
 * This creates the module, started up with NAN_MODULE_INIT.
 * The naming/casing of the first argument is reflected in lib/hello_world.js
 */
NODE_MODULE(HelloWorld, HelloWorld::Init);
