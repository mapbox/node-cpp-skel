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
  // NAN_METHOD is applicable to methods you want to expose to JS world
  NAN_METHOD(HelloObject::hello) {

    // "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(Nan::New<v8::String>("...initialized an object...world").ToLocalChecked());
  
  }

  void HelloObject::Init(v8::Local<v8::Object> target)
  {
  	  // This is saying: 
  	  // "Node, please allocate a new Javascript string object 
  	  // inside the V8 memory space, with the value 'HelloObject' "
      const auto whoami = Nan::New("HelloObject").ToLocalChecked();  
       
      // Create the HelloObject
      auto fnTp = Nan::New<v8::FunctionTemplate>(HelloObject::New); // Passing the HelloObject::New constructor above
      fnTp->InstanceTemplate()->SetInternalFieldCount(1); // It's 1 when holding the ObjectWrap itself and nothing else
      fnTp->SetClassName(whoami); // Passing the Javascript string object above

      // Add custom methods here.
      // This line is attaching the "hello" method to a JavaScript function prototype.
      // "hello" is therefore like a property of the fnTp object 
      // ex: console.log(HelloObject.hello) --> [Function: hello]
      SetPrototypeMethod(fnTp, "hello", hello);

      // Create an unique instance of the HelloObject function template,
      // then set this unique instance to the target
      const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
      Nan::Set(target, whoami, fn);
  }

} // namespace object_sync