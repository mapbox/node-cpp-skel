#pragma once
#include <nan.h>

namespace object_async {

    /**
     * HelloObject class
     * This is in a header file so we can access it across other .cpp files if necessary
     * Also, this class adheres to the rule of Zero because we define no custom destructor or copy constructor
     */
    class HelloObjectAsync: public Nan::ObjectWrap {

        public:
            // initializer
            static void Init(v8::Local<v8::Object> target);

            // methods required for the V8 constructor
            static NAN_METHOD(New);
            static Nan::Persistent<v8::Function> &create_once();

            // hello_async, custom async method tied to Init of this class
            // method's logic lives in ./hello.cpp
            static NAN_METHOD(hello_async);

            // C++ Constructor
            // This includes a Default Argument
            // If a parameter value is passed in, it takes priority over the default arg
            HelloObjectAsync(std::string name="world");

        private:
            // member variable
            // specific to each instance of the class
            std::string name_;

	};

}