#pragma once
#include <napi.h>

namespace standalone {

// hello, custom sync method
Napi::Value hello(Napi::CallbackInfo const& info);

} // namespace standalone
