#pragma once
#include <nan.h>

namespace object_sync {

    /**
     * HelloObject class
     * This is in a header file so we can access it across other .cpp files if necessary
     * Also, this class adheres to the rule of Zero because we define no custom destructor or copy constructor
     */
    class HelloObject: public Nan::ObjectWrap {

        public:
            // initializer
            static NAN_MODULE_INIT(Init);

            // methods required for the constructor
            static NAN_METHOD(New);
            //static Nan::Persistent<v8::Function> &constructor();

            // hello, custom sync method tied to module.cpp
            // method's logic lives in hello.cpp
            static NAN_METHOD(hello);

	};

}