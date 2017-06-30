#include <nan.h>
#include "standalone/hello.hpp"
#include "standalone_async/hello_async.hpp"
#include "object_sync/hello.hpp"
// #include "your_code.hpp"

// "target" is a magic var that nodejs passes into modules scope
// When you write things to target, they become available to call from Javascript world
static void init(v8::Local<v8::Object> target) {

    // expose hello method
    Nan::SetMethod(target, "hello", standalone::hello);
    
    // expose hello_async method
    Nan::SetMethod(target, "hello_async", standalone_async::hello_async);

    // expose HelloObject class
    object_sync::HelloObject::Init(target);
    
   /**
    * You may have noticed there are multiple "hello" functions as part of this module.
    * They are both exposed a bit differently.
    * 1) standalone::hello    // exposed above
    * 2) HelloObject.hello    // exposed in object_sync/hello.cpp as part of HelloObject
    */

    // add more methods/classes below that youd like to use in Javascript world
    // then create a directory in /src with a .cpp and a .hpp file
}

NODE_MODULE(module, init)