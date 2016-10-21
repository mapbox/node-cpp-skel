#include "hello_world.hpp"

#include <exception>
#include <iostream>
#include <chrono> // time lib
#include <thread> // sleep_for is a function within the thread lib
#include <map>

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

// this is the cpp object that will be passed around in 'shout' and callbacks
// referred to as a "baton"
class AsyncBaton
{
  public:
    uv_work_t request; // required
    Nan::Persistent<v8::Function> cb; // callback function type
    std::string phrase; // required
    bool louder; // optional
    std::uint32_t sleep; // optional (# of seconds)
    std::string error_name;
    std::string result;
};

NAN_METHOD(HelloWorld::shout)
{
    std::string phrase = "";
    bool louder = false;
    std::uint32_t sleep = 0;

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

    if (options->Has(Nan::New("sleep").ToLocalChecked())) 
    {
        v8::Local<v8::Value> sleep_val = options->Get(Nan::New("sleep").ToLocalChecked());
        if (!sleep_val->IsUint32())
        {
            CallbackError("option 'sleep' must be a positive integer", callback);
            return;
        }
        sleep = sleep_val->Uint32Value();
    }

    // set up the baton to pass into our threadpool
    AsyncBaton *baton = new AsyncBaton();
    baton->request.data = baton;
    baton->phrase = phrase;
    baton->louder = louder;
    baton->sleep = sleep;
    baton->cb.Reset(callback);

    /*
    `uv_queue_work` is the all-important way to pass info into the threadpool.
    It cannot take v8 objects, so we need to do some manipulation above to convert into cpp objects
    otherwise things get janky. It takes four arguments:

    1) which loop to use, node only has one so we pass in a pointer to the default
    2) the baton defined above, we use this to access information important for the method
    3) operations to be executed within the threadpool
    4) operations to be executed after #3 is complete to pass into the callback
    */
    uv_queue_work(uv_default_loop(), &baton->request, AsyncShout, (uv_after_work_cb)AfterShout);
    return;
}

NAN_METHOD(HelloWorld::busyThreads)
{
    std::string phrase = "";

    // check second argument, should be a 'callback' function.
    // This allows us to set the callback so we can use it to return errors
    // instead of throwing as well.
    if (!info[1]->IsFunction()) 
    {
        Nan::ThrowTypeError("second arg 'callback' must be a function");
        return;
    }
    v8::Local<v8::Function> callback = info[1].As<v8::Function>();

    // check first argument, should be a 'phrase' string
    if (!info[0]->IsString()) 
    {
        CallbackError("first arg 'phrase' must be a string", callback);
        return;
    }
    phrase = *v8::String::Utf8Value((info[0])->ToString());

    // set up the baton to pass into our threadpool
    AsyncBaton *baton = new AsyncBaton();
    baton->request.data = baton;
    baton->phrase = phrase;
    baton->cb.Reset(callback);

    /*
    `uv_queue_work` is the all-important way to pass info into the threadpool.
    It cannot take v8 objects, so we need to do some manipulation above to convert into cpp objects
    otherwise things get janky. It takes four arguments:

    1) which loop to use, node only has one so we pass in a pointer to the default
    2) the baton defined above, we use this to access information important for the method
    3) operations to be executed within the threadpool
    4) operations to be executed after #3 is complete to pass into the callback
    */
    uv_queue_work(uv_default_loop(), &baton->request, AsyncBusyThreads, (uv_after_work_cb)AfterBusyThreads);
    return;
}

NAN_METHOD(HelloWorld::sleepyThreads)
{
    std::string phrase = "";
    std::uint32_t sleep = 0;

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

    if (options->Has(Nan::New("sleep").ToLocalChecked())) 
    {
        v8::Local<v8::Value> sleep_val = options->Get(Nan::New("sleep").ToLocalChecked());
        if (!sleep_val->IsUint32())
        {
            CallbackError("option 'sleep' must be a positive integer", callback);
            return;
        }
        sleep = sleep_val->Uint32Value();
    }

    // set up the baton to pass into our threadpool
    AsyncBaton *baton = new AsyncBaton();
    baton->request.data = baton;
    baton->phrase = phrase;
    baton->sleep = sleep;
    baton->cb.Reset(callback);

    /*
    `uv_queue_work` is the all-important way to pass info into the threadpool.
    It cannot take v8 objects, so we need to do some manipulation above to convert into cpp objects
    otherwise things get janky. It takes four arguments:

    1) which loop to use, node only has one so we pass in a pointer to the default
    2) the baton defined above, we use this to access information important for the method
    3) operations to be executed within the threadpool
    4) operations to be executed after #3 is complete to pass into the callback
    */
    uv_queue_work(uv_default_loop(), &baton->request, AsyncSleepyThreads, (uv_after_work_cb)AfterSleepyThreads);
    return;
}

// expensive allocation of std::map, querying, and string comparison
std::string do_expensive_work(std::string const& phrase) {

    if (phrase != "rawr") {
        throw std::runtime_error("we really would prefer rawr all the time");
    }

    std::map<std::size_t,std::string> container;
    std::size_t iterations = 1000000;

    for (std::size_t i=0;i<iterations;++i) {
        container.emplace(i,std::to_string(i));
    }

    for (std::size_t i=0;i<iterations;++i) {
        std::string const& item = container[i];
        if (item != std::to_string(i)) {
            abort();
        }
    }

    std::string result = phrase + "...threads are busy bees";

    return result;
}

std::string do_sleepy_work(std::string const& phrase, uint32_t sleep) {
    std::string result;

    // This is purely for testing, to be able to simulate an unexpected throw
    // from a function you do not control and may throw an exception
    if (phrase != "rawr") {
        throw std::runtime_error("we really would prefer rawr all the time");
    }

    // suspends execution of the calling thread for (at least) # of seconds
    if (sleep) {
        // http://en.cppreference.com/w/cpp/chrono/duration
        // duration type object that sleep_for accepts
        std::chrono::seconds sec(sleep);
 
        std::this_thread::sleep_for(sec);
        result = phrase + " zzzZZZ";
    }

    return result;
}

// this is where we actually exclaim our shout phrase
void HelloWorld::AsyncShout(uv_work_t* req)
{
    AsyncBaton *baton = static_cast<AsyncBaton *>(req->data);

    /***************** custom code here ******************/
    // The try/catch is critical here: if code was added that could throw an unhandled error INSIDE the threadpool, it would be disasterous
    try
    {
        std::string result;
        std::string phrase = baton->phrase;

        // This is purely for testing, to be able to simulate an unexpected throw
        // from a function you do not control and may throw an exception
        if (phrase != "rawr") {
            throw std::runtime_error("we really would prefer rawr all the time");
        }

        result = phrase + "!";

        if (baton->louder)
        {
            result += "!!!!";
        }

        baton->result = result;

    }
    catch (std::exception const& ex)
    {
        baton->error_name = ex.what();
    }
    /***************** end custom code *******************/

}

// handle results from AsyncShout - if there are errors return those
// otherwise return the type & info to our callback
void HelloWorld::AfterShout(uv_work_t* req)
{
    Nan::HandleScope scope;

    AsyncBaton *baton = static_cast<AsyncBaton *>(req->data);

    if (!baton->error_name.empty()) 
    {
        v8::Local<v8::Value> argv[1] = { Nan::Error(baton->error_name.c_str()) };
        Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(baton->cb), 1, argv);
    }
    else
    {
        v8::Local<v8::Value> argv[2] = { Nan::Null(), Nan::New<v8::String>(baton->result.data()).ToLocalChecked() };
        Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(baton->cb), 2, argv);
    }

    baton->cb.Reset();
    delete baton;
}


// this is where we actually set the bees to work
void HelloWorld::AsyncBusyThreads(uv_work_t* req)
{
    AsyncBaton *baton = static_cast<AsyncBaton *>(req->data);

    /***************** custom code here ******************/
    // The try/catch is critical here: if code was added that could throw an unhandled error INSIDE the threadpool, it would be disasterous
    try
    {
        baton->result = do_expensive_work(baton->phrase);
    }
    catch (std::exception const& ex)
    {
        baton->error_name = ex.what();
    }
    /***************** end custom code *******************/

}

// handle results from AsyncShout - if there are errors return those
// otherwise return the type & info to our callback
void HelloWorld::AfterBusyThreads(uv_work_t* req)
{
    Nan::HandleScope scope;

    AsyncBaton *baton = static_cast<AsyncBaton *>(req->data);

    if (!baton->error_name.empty()) 
    {
        v8::Local<v8::Value> argv[1] = { Nan::Error(baton->error_name.c_str()) };
        Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(baton->cb), 1, argv);
    }
    else
    {
        v8::Local<v8::Value> argv[2] = { Nan::Null(), Nan::New<v8::String>(baton->result.data()).ToLocalChecked() };
        Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(baton->cb), 2, argv);
    }

    baton->cb.Reset();
    delete baton;
}

// this is where we actually exclaim our shout phrase
void HelloWorld::AsyncSleepyThreads(uv_work_t* req)
{
    AsyncBaton *baton = static_cast<AsyncBaton *>(req->data);

    /***************** custom code here ******************/
    // The try/catch is critical here: if code was added that could throw an unhandled error INSIDE the threadpool, it would be disasterous
    try
    {
        baton->result = do_sleepy_work(baton->phrase,baton->sleep);
    }
    catch (std::exception const& ex)
    {
        baton->error_name = ex.what();
    }
    /***************** end custom code *******************/

}

// handle results from AsyncShout - if there are errors return those
// otherwise return the type & info to our callback
void HelloWorld::AfterSleepyThreads(uv_work_t* req)
{
    Nan::HandleScope scope;

    AsyncBaton *baton = static_cast<AsyncBaton *>(req->data);

    if (!baton->error_name.empty()) 
    {
        v8::Local<v8::Value> argv[1] = { Nan::Error(baton->error_name.c_str()) };
        Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(baton->cb), 1, argv);
    }
    else
    {
        v8::Local<v8::Value> argv[2] = { Nan::Null(), Nan::New<v8::String>(baton->result.data()).ToLocalChecked() };
        Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(baton->cb), 2, argv);
    }

    baton->cb.Reset();
    delete baton;
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
    SetPrototypeMethod(fnTp, "busyThreads", busyThreads);
    SetPrototypeMethod(fnTp, "sleepyThreads", sleepyThreads);

    const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
    constructor().Reset(fn);
    Nan::Set(target, whoami, fn);
}

/*
 * This creates the module, started up with NAN_MODULE_INIT.
 * The naming/casing of the first argument is reflected in lib/hello_world.js
 */
NODE_MODULE(HelloWorld, HelloWorld::Init);