#pragma once
#include <memory>
#include <napi.h>
#include <string>

namespace utils {

/*
* This is an internal function used to return callback error messages instead of
* throwing errors.
* Usage:
*
* return CallbackError(env, "error message", callback);
*
*/

inline Napi::Value CallbackError(Napi::Env env, std::string const& message, Napi::Function const& func)
{
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("message", message);
    return func.Call({obj});
}

} // namespace utils

namespace gsl {
template <typename T>
using owner = T;
} // namespace gsl

// ^^^ type alias required for clang-tidy (cppcoreguidelines-owning-memory)
