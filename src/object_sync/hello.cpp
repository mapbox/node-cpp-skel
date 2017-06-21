#include "hello.hpp"

  // If this was not defined within a namespace, it would be in the global scope.
namespace object_sync {
  
  // Default constructor
  NAN_METHOD(HelloObject::New) {

    if (!info.IsConstructCall())
    {
        return Nan::ThrowTypeError(
            "Cannot call constructor as function, you need to use 'new' keyword");
    }
  }

  // If this function was not defined within a namespace, it would be in the global scope.
  NAN_METHOD(HelloObject::hello) {

    // "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(Nan::New<v8::String>("...initialized an object...world").ToLocalChecked());
  
  }

  // Defined function is doing a bunch of stuff. Check docs.
  // NAN_MODULE_INIT(HelloObject::Init)
  // {
  // 	  // NAN > internal relationship between C++ and Node
  //     // This is a design pattern initially impl for Node

      

 	//   // Nan::New..."Node allocate a new object of the HelloObject type"
  // 	  // Setting up a string? Is "whoami" type String?
  // 	  // whoami is just a name...therefore just a string
  // 	  // Everything in node is written in C++...so when we have a var...
  // 	  // it's just a string name in C++ world that will be mapped to some object/type in memory
  //     const auto whoami = Nan::New("HelloObject").ToLocalChecked();  
  //     //NAN_EXPORT(target, HelloObject::hello);

  //     // "Nan New"  is a special Javascript string
       
  //     // Create the HelloObject
  //     auto fnTp = Nan::New<v8::FunctionTemplate>(HelloObject::New); // Calling HelloObject::New constructor above
  //     fnTp->InstanceTemplate()->SetInternalFieldCount(1); // Always 1 ? It's 1 when holding only ObjectWrap itself
  //     fnTp->SetClassName(whoami);

  //     // custom methods added here
  //     // "hello" is just like a property of the fnTp object
  //     SetPrototypeMethod(fnTp, "hello", hello); 

  //     // Sets it to the target
  //     const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
  //     Nan::Set(target, whoami, fn);
  // }

} // namespace object_sync