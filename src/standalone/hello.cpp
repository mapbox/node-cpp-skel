#include "hello.hpp"

/**
 * This is a synchronous standalone function that logs a string.
 * @name hello
 * @returns {string}
 * @example
 * var module = require('./path/to/lib/index.js');
 * var check = module.hello();
 * console.log(check); // => "hello world"
 */
namespace standalone {
// If this was not defined within a namespace, it would be in the global scope.
// Namespaces are used because C++ has no notion of scoped modules, so all of
// the code you write in any file could conflict with other code.
// Namespaces are generally a great idea in C++ because it helps scale and
// clearly organize your application.

// hello is a "standalone function" because it's not a class.
// If this function was not defined within a namespace, it would be in the
// global scope.
Napi::Value hello(Napi::CallbackInfo const& info) {

    // "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    Napi::Env env = info.Env();
    return Napi::String::New(env, "hello world");
}
} // namespace standalone
