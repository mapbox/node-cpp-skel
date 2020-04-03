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
* Napi::CallbackInfo info;
* return CallbackError("error message", info);
*
*/
inline Napi::Value CallbackError(std::string const& message, Napi::CallbackInfo const& info)
{
    Napi::Object obj = Napi::Object::New(info.Env());
    obj.Set("message", message);
    auto func = info[info.Length() - 1].As<Napi::Function>();
    // ^^^ here we assume that info has a valid callback function
    // TODO: consider changing either method signature or adding internal checks
    return func.Call({obj});
}
} // namespace utils

namespace gsl {
template <typename T>
using owner = T;
} // namespace gsl

// ^^^ type alias required for clang-tidy (cppcoreguidelines-owning-memory)
