#pragma once

#pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-parameter"
// #pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

/**
 * HelloWorld class
 * This is in a header file so we can access it across other .cpp files
 * if necessary
 */
class HelloWorld: public Nan::ObjectWrap 
{
    public:
        // initializer
        static NAN_MODULE_INIT(Init);

        // method new used for the constructor
        static NAN_METHOD(New);

        // custom method called wave
        static NAN_METHOD(wave);
        static Nan::Persistent<v8::Function> &constructor();
};