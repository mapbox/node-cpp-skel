#include "hello.hpp"

namespace standalone {

Napi::Value hello(Napi::CallbackInfo const& info)
{
    Napi::Env env = info.Env();
    return Napi::String::New(env, "hello world");
}

} // namespace standalone
