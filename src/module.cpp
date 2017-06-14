#include <nan.h>
#include "standalone/hello.hpp"

// "target" is a magic var that nodejs passes into modules scope
// When you write things to target, they become available to call from js
static void init(v8::Local<v8::Object> target) {

    // expose hello method
    Nan::SetMethod(target, "hello", standalone::hello);

    // add more methods below that youd like to use in node.js-world
    // then create a .cpp and .hpp file in /src for each new method
}

NODE_MODULE(module, init)