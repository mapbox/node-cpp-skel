#pragma once
#include <nan.h>

namespace object_sync {

    /**
     * HelloWorld class
     * This is in a header file so we can access it across other .cpp files if necessary
     * Also, this class adheres to the rule of Zero because we define no custom destructor or copy constructor
     */
    class Hello: public Nan::ObjectWrap {
	    
	    // hello, custom sync method tied to module.cpp
	    // method's logic lives in hello.cpp
	    NAN_METHOD(hello);

	};

}