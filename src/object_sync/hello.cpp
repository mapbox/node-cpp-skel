#include "hello.hpp"

// If this was not defined within a namespace, it would be in the global scope.
namespace object_sync {

  // Custom constructor, assigns custom name passed in from Javascript world.
  // This constructor uses member init list via the semicolon, aka "direct initialization" 
  // which is more efficient than using assignment operators.
  HelloObject::HelloObject(std::string name) : 
    name_(name) {}

  // Triggered from Javascript world when calling "new HelloObject()" or "new HelloObject(name)"
  NAN_METHOD(HelloObject::New) {
    if (info.IsConstructCall())
    {
        try
        {   
            if (info.Length() >= 1) {
              if (info[0]->IsString()) 
              {
                std::string name = *v8::String::Utf8Value(info[0]->ToString());
                auto *const self = new HelloObject(name);
                self->Wrap(info.This());
              }
              else
              {
                return Nan::ThrowTypeError(
                    "arg must be a string");
              }
            }
            else {
                auto *const self = new HelloObject();
                self->Wrap(info.This());
            } 

        }
        catch (const std::exception &ex)
        {
            return Nan::ThrowTypeError(ex.what());
        }

        info.GetReturnValue().Set(info.This());
    }
    else
    {
        return Nan::ThrowTypeError(
            "Cannot call constructor as function, you need to use 'new' keyword");
    }
  }

  // If this function was not defined within a namespace, it would be in the global scope.
  // NAN_METHOD is applicable to methods you want to expose to JS world
  NAN_METHOD(HelloObject::hello) {

    HelloObject* h = Nan::ObjectWrap::Unwrap<HelloObject>(info.Holder());

    // "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(Nan::New<v8::String>("...initialized an object...hello " + h->name_).ToLocalChecked()); // ???
  
  }
  
  // Singleton...not really sure what to say about this
  Nan::Persistent<v8::Function> &HelloObject::constructor()
  {
      static Nan::Persistent<v8::Function> init;
      return init;
  }

  void HelloObject::Init(v8::Local<v8::Object> target)
  {
  	  // This is saying: 
  	  // "Node, please allocate a new Javascript string object 
  	  // inside the V8 memory space, with the value 'HelloObject' "
      const auto whoami = Nan::New("HelloObject").ToLocalChecked();  
       
      // Create the HelloObject
      auto fnTp = Nan::New<v8::FunctionTemplate>(HelloObject::New); // Passing the HelloObject::New method above
      fnTp->InstanceTemplate()->SetInternalFieldCount(1); // It's 1 when holding the ObjectWrap itself and nothing else
      fnTp->SetClassName(whoami); // Passing the Javascript string object above

      // Add custom methods here.
      // This is how hello() is exposed as part of HelloObject.
      // This line is attaching the "hello" method to a JavaScript function prototype.
      // "hello" is therefore like a property of the fnTp object 
      // ex: console.log(HelloObject.hello) --> [Function: hello]
      SetPrototypeMethod(fnTp, "hello", hello);

      // Create an unique instance of the HelloObject function template,
      // then set this unique instance to the target
      const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
      constructor().Reset(fn);  // calls the static &HelloObject::constructor method above. This ensures the instructions in this Init function are retained in memory even after this code block ends.
      Nan::Set(target, whoami, fn);
  }

} // namespace object_sync