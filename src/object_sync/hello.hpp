#pragma once

#include <napi.h>

namespace object_sync {

/**
 * HelloObject class
 * This is in a header file so we can access it across other .cpp files if necessary
 * Also, this class adheres to the rule of Zero because we define no custom destructor or copy constructor
 */
class HelloObject : public Napi::ObjectWrap<HelloObject>
{
  public:
    // initializers
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    explicit HelloObject(Napi::CallbackInfo const& info);
    Napi::Value hello(Napi::CallbackInfo const& info);

  private:
    static Napi::FunctionReference constructor;
    std::string name_ = "";
};
} // namespace object_sync
