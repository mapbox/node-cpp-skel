#pragma once
#include <napi.h>

namespace standalone_promise {

// hello, custom sync method
// method's logic lives in hello_promise.cpp
Napi::Value helloPromise(Napi::CallbackInfo const& info);

} // namespace standalone_promise
