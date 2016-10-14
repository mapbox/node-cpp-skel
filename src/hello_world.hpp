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
class HelloWorld : public Nan::ObjectWrap 
{
    public:
        // initializer
        // static NAN_MODULE_INIT(Init)
        static void Init(v8::Local<v8::Object>, v8::Local<v8::Object> module);

        // methods required for the constructor
        // static NAN_METHOD(New)
        static void New(const Nan::FunctionCallbackInfo<v8::Value>&);
        static Nan::Persistent<v8::Function> &constructor();

        // Wave, custom sync method
        // static NAN_METHOD(Wave)
        static void Wave(const Nan::FunctionCallbackInfo<v8::Value>&);
        
        // Shout, custom async method
        // static NAN_METHOD(Shout)
        static void Shout(const Nan::FunctionCallbackInfo<v8::Value>&);
        static void AsyncShout(uv_work_t* req);
        static void AfterShout(uv_work_t* req);

        // constructor
        // This includes a Default Argument
        // If a paramter value is passed in, it takes priority over the default arg
        HelloWorld(std::string name="hello");

    private:
        // member variable
        // specific to each instance of the class
        std::string name_; 

};
