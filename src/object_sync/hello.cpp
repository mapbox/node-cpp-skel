#include "hello.hpp"

namespace object_sync {
  
  // If this function was not defined within a namespace, it would be in the global scope.
  NAN_METHOD(HelloObject::New) {

  
  }

  // If this function was not defined within a namespace, it would be in the global scope.
  NAN_METHOD(HelloObject::hello) {

    // "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(Nan::New<v8::String>("...initialized an object...world").ToLocalChecked());
  
  }

  NAN_MODULE_INIT(HelloObject::Init)
  {
  	  // Add comments...not sure what this is doing
      const auto whoami = Nan::New("HelloObject").ToLocalChecked();  

      // Add comments...not sure what this is doing
      auto fnTp = Nan::New<v8::FunctionTemplate>(New);
      fnTp->InstanceTemplate()->SetInternalFieldCount(1);
      fnTp->SetClassName(whoami);  

      // custom methods added here
      SetPrototypeMethod(fnTp, "hello", hello); 

      // Add comments...not sure what this is doing
      const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
      Nan::Set(target, whoami, fn);
  }

} // namespace object_sync