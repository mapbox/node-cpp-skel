#include "hello.hpp"

namespace object_sync {

  // hello is a "standalone function" because it's not a class.
  // If this function was not defined within a namespace, it would be in the global scope.
  NAN_METHOD(hello) {

    // "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(Nan::New<v8::String>("world").ToLocalChecked());
  
  }
} // namespace standalone