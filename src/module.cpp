#include <nan.h>
// #include "your_code.hpp"
namespace standalone{
  NAN_METHOD(convert) {
      // "info" comes from the NAN_METHOD macro, which returns differently
      // according to the version of node
      info.GetReturnValue().Set(
          Nan::New<v8::String>("hello world").ToLocalChecked());
  }
}
// "target" is a magic var that nodejs passes into a module's scope.
// When you write things to target, they become available to call from
// Javascript world.
static void init(v8::Local<v8::Object> target) {

    // expose hello method
    Nan::SetMethod(target, "convert", standalone::convert);

}

NODE_MODULE(module, init) // NOLINT