#pragma once
#include <napi.h>

namespace object_async {

/**
 * HelloObject class
 * This is in a header file so we can access it across other .cpp files if
 * necessary
 * Also, this class adheres to the rule of Zero because we define no custom
 * destructor or copy constructor
 */
class HelloObjectAsync : public Napi::ObjectWrap<HelloObjectAsync>
{
  public:
    // initializer
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    explicit HelloObjectAsync(Napi::CallbackInfo const& info);
    Napi::Value helloAsync(Napi::CallbackInfo const& info);

  private:
    // member variable
    // specific to each instance of the class
    static Napi::FunctionReference constructor;
    std::string name_ = "";
};
} // namespace object_async
