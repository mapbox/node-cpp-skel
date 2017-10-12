#include "object_async/hello_async.hpp"
#include "object_sync/hello.hpp"
#include "standalone/hello.hpp"
#include "standalone_async/hello_async.hpp"
#include <nan.h>
// #include "your_code.hpp"

// "target" is a magic var that nodejs passes into a module's scope.
// When you write things to target, they become available to call from
// Javascript world.
static void init(v8::Local<v8::Object> target) {

    // expose hello method
    Nan::SetMethod(target, "hello", standalone::hello);

    // expose helloAsync method
    Nan::SetMethod(target, "helloAsync", standalone_async::helloAsync);

    // expose HelloObject class
    object_sync::HelloObject::Init(target);

    // expose HelloObjectAsync class
    object_async::HelloObjectAsync::Init(target);

    /**
   * You may have noticed there are multiple "hello" functions as part of this
   * module.
   * They are exposed a bit differently.
   * 1) standalone::hello    // exposed above
   * 2) HelloObject.hello    // exposed in object_sync/hello.cpp as part of
   * HelloObject
   */

    // add more methods/classes below that youd like to use in Javascript world
    // then create a directory in /src with a .cpp and a .hpp file.
    // Include your .hpp file at the top of this file.
}

// Here we initialize the module (we only do this once)
// by attaching the init function to the module. This invokes
// a variety of magic from inside nodejs core that we don't need to
// worry about, but if you care the details are at https://github.com/nodejs/node/blob/34d1b1144e1af8382dad71c28c8d956ebf709801/src/node.h#L431-L518
// We mark this NOLINT to avoid the clang-tidy checks
// warning about code inside nodejs that we don't control and can't
// directly change to avoid the warning.
NODE_MODULE(module, init) // NOLINT