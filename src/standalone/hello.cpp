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

Napi::Value hello(Napi::CallbackInfo const& info)
{
    Napi::Env env = info.Env();
    return Napi::String::New(env, "hello world");
}

} // namespace standalone
