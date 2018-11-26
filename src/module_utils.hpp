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

inline Napi::Value NewBufferFrom(Napi::Env const& env, std::unique_ptr<std::string>&& ptr)
{
    std::string& str = *ptr;
    Napi::Value val = Napi::Buffer<char>::New(env,
                                              const_cast<char*>(str.data()),
                                              str.size(),
                                              [](Napi::Env, char*, std::string* s) {
                                                  delete s;
                                              },
                                              ptr.release());
    return val;
}

} // namespace utils
