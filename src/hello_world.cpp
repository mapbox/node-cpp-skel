#include "hello_world.hpp"

#include <exception>
#include <iostream>

/**
 * Main class, called HelloWorld
 * @class HelloWorld
 * @example
 * var HelloWorld = require('index.js');
 * var HW = new HelloWorld();
 */
NAN_METHOD(HelloWorld::New)
{
    if (info.IsConstructCall())
    {
        try
        {
            auto *const self = new HelloWorld();
            self->Wrap(info.This());
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
    std::string phrase;
    bool louder;
    std::string error_name;
    std::string result;
};

NAN_METHOD(HelloWorld::shout)
{
    std::string phrase = "";
    bool louder = false;

    // check first argument, should be a 'phrase' string
    if (!info[0]->IsString()) 
    {
        Nan::ThrowTypeError("first arg 'phrase' must be a string");
        return;
    }
    phrase = *v8::String::Utf8Value((info[0])->ToString());

    // check second argument, should be an 'options' object
    if (!info[1]->IsObject()) 
    {
        Nan::ThrowTypeError("second arg 'options' must be an object");
        return;
    }

    v8::Local<v8::Object> options = info[1].As<v8::Object>();
    if (options->Has(Nan::New("louder").ToLocalChecked())) 
    {
        v8::Local<v8::Value> louder_val = options->Get(Nan::New("louder").ToLocalChecked());
        if (!louder_val->IsBoolean())
        {
            Nan::ThrowError("option 'louder' must be a boolean");
            return;
        }
        louder = louder_val->BooleanValue();
    }

    // check third argument, should be a 'callback' function
    if (!info[2]->IsFunction()) 
    {
      Nan::ThrowTypeError("third arg 'callback' must be a function");
      return;
    }
    v8::Local<v8::Value> callback = info[2];

    // set up the baton to pass into our threadpool
    AsyncBaton *baton = new AsyncBaton();
    baton->request.data = baton;
    baton->phrase = phrase;
    baton->louder = louder;
    baton->cb.Reset(callback.As<v8::Function>());

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

// this is where we actually exclaim our shout phrase
void HelloWorld::AsyncShout(uv_work_t* req)
{
    AsyncBaton *baton = static_cast<AsyncBaton *>(req->data);

    /***************** custom code here ******************/
    try
    {
        std::string return_string = baton->phrase + "!";

        if (baton->louder)
        {
            return_string += "!!!!";
        }

        baton->result = return_string;
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

NODE_MODULE(HelloWorld, HelloWorld::Init);