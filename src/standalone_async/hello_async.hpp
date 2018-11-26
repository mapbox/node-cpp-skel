#pragma once
#include <napi.h>

namespace standalone_async {

// hello, custom sync method
// method's logic lives in hello.cpp
Napi::Value helloAsync(Napi::CallbackInfo const& info);

} // namespace standalone_async
