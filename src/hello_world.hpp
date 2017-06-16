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

        // methods required for the constructor
        static NAN_METHOD(New);
        static Nan::Persistent<v8::Function> &constructor();

        // wave, custom sync method
        static NAN_METHOD(wave);

        // Function called after aync work is done
        // Currently re-used by all async functions
        static void AfterAsync(uv_work_t* req);
        
        // shout, custom async method
        static NAN_METHOD(shout);
        static void AsyncShout(uv_work_t* req);

        // busyThreads, custom async method
        static NAN_METHOD(busyThreads);
        static void AsyncBusyThreads(uv_work_t* req);

        // sleepyThreads, custom async method
        static NAN_METHOD(sleepyThreads);
        static void AsyncSleepyThreads(uv_work_t* req);

        // sleepyThreads, custom async method
        static NAN_METHOD(contentiousThreads);
        static void AsyncContentiousThreads(uv_work_t* req);

        // constructor
        // This includes a Default Argument
        // If a paramter value is passed in, it takes priority over the default arg
        // HelloWorld --> class
        HelloWorld(std::string name="hello");

    private:
        // member variable
        // specific to each instance of the class
        std::string name_; 

};