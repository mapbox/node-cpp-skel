#include <nan.h>
#include "hello.hpp"

// target is magic var that nodejs passes into modules scope
// When you write things to target, they become available to call from js
static void init(v8::Local<v8::Object> target) {

    // expose hello method
    Nan::SetMethod(target, "hello", hello);

    // add more methods below that youd like to use in node.js-world
}

NODE_MODULE(module, init)