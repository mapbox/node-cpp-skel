#include "hello_world.hpp"

#include <exception>
#include <iostream>

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
            // return Nan::ThrowTypeError(ex.what());
            return Nan::ThrowTypeError("Error");
        }

        info.GetReturnValue().Set(info.This());
    }
    else
    {
        return Nan::ThrowTypeError(
            "Cannot call constructor as function, you need to use 'new' keyword");
    }
}

NAN_METHOD(HelloWorld::wave)
{
    // info comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(Nan::New<v8::String>("howdy world!").ToLocalChecked());
}

Nan::Persistent<v8::Function> &HelloWorld::constructor()
{
    static Nan::Persistent<v8::Function> init;
    return init;
}

NAN_MODULE_INIT(HelloWorld::Init)
{
    const auto whoami = Nan::New("HelloWorld").ToLocalChecked();

    auto fnTp = Nan::New<v8::FunctionTemplate>(New);
    fnTp->InstanceTemplate()->SetInternalFieldCount(1);
    fnTp->SetClassName(whoami);

    SetPrototypeMethod(fnTp, "wave", wave);

    const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();

    constructor().Reset(fn);

    Nan::Set(target, whoami, fn);
}

NODE_MODULE(HelloWorld, HelloWorld::Init);



