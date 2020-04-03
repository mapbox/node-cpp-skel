#include "object_async/hello_async.hpp"
#include "object_sync/hello.hpp"
#include "standalone/hello.hpp"
#include "standalone_async/hello_async.hpp"
#include <napi.h>
// #include "your_code.hpp"

Napi::Object init(Napi::Env env, Napi::Object exports)
{
    // expose hello method
    exports.Set(Napi::String::New(env, "hello"), Napi::Function::New(env, standalone::hello));

    // expose helloAsync method
    exports.Set(Napi::String::New(env, "helloAsync"), Napi::Function::New(env, standalone_async::helloAsync));

    // expose HelloObject class
    object_sync::HelloObject::Init(env, exports);

    // expose HelloObjectAsync class
    object_async::HelloObjectAsync::Init(env, exports);

    return exports;
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

// Initialize the module (we only do this once)
// Mark this NOLINT to avoid the clang-tidy checks
// NODE_GYP_MODULE_NAME is the name of our module as defined in 'target_name'
// variable in the 'binding.gyp', which is passed along as a compiler define
NODE_API_MODULE(NODE_GYP_MODULE_NAME, init) // NOLINT
