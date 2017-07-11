#include "hello_async.hpp"

#include <exception>
#include <stdexcept>
#include <iostream>
#include <map>

// If this was not defined within a namespace, it would be in the global scope.
namespace object_async {

  // Custom constructor, assigns custom name passed in from Javascript world.
  // This constructor uses member init list via the semicolon, aka "direct initialization" 
  // which is more efficient than using assignment operators.
  HelloObjectAsync::HelloObjectAsync(std::string name) : 
    name_(name) {}

  // Triggered from Javascript world when calling "new HelloObjectAsync()" or "new HelloObjectAsync(name)"
  NAN_METHOD(HelloObjectAsync::New) {
    if (info.IsConstructCall())
    {
        try
        {   
            if (info.Length() >= 1) {
              if (info[0]->IsString()) 
              {
                std::string name = *v8::String::Utf8Value(info[0]->ToString());
                auto *const self = new HelloObjectAsync(name);
                self->Wrap(info.This());
              }
              else
              {
                return Nan::ThrowTypeError(
                    "arg must be a string");
              }
            }
            else {
                auto *const self = new HelloObjectAsync();
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

  /*
  * This is an internal function used to return callback error messages instead of
  * throwing errors.
  * Usage:
  *
  * v8::Local<v8::Function> callback;
  * CallbackError("error message", callback);
  * return; // this is important to prevent duplicate callbacks from being fired!
  */
  inline void CallbackError(std::string message, v8::Local<v8::Function> callback) {
  	v8::Local<v8::Value> argv[1] = { Nan::Error(message.c_str()) };
  	Nan::MakeCallback(Nan::GetCurrentContext()->Global(), callback, 1, argv);
  }


  // Expensive allocation of std::map, querying, and string comparison,
  // therefore threads are busy
  std::string do_expensive_work(bool louder, std::string name) {  

      std::map<std::size_t,std::string> container;  
      std::size_t work_to_do=100000;

      for (std::size_t i=0;i<work_to_do;++i) {
          container.emplace(i,std::to_string(i));
      }  

      for (std::size_t i=0;i<work_to_do;++i) {
          std::string const& item = container[i];
          if (item != std::to_string(i)) {

              // AsyncHelloWorker's Execute function will take care of this error 
              // and return it to js-world via callback
              throw std::runtime_error("Uh oh, this should never happen");
          }
      }  

      std::string result = "...threads are busy async bees...hello " + name;
 
      if (louder)
      {
        result += "!!!!";
      }
     
      return result;  

  }  

  // This is the worker running asynchronously and calling a user-provided callback when done.
  // Consider storing all C++ objects you need by value or by shared_ptr to keep them alive until done.
  // Nan AsyncWorker docs: https://github.com/nodejs/nan/blob/master/doc/asyncworker.md
  struct AsyncHelloWorker : Nan::AsyncWorker {

      using Base = Nan::AsyncWorker;  

      AsyncHelloWorker(bool louder, std::string name, Nan::Callback* callback)
          : Base(callback), result_{""}, louder_{louder}, name_{name} { }  

      // The Execute() function is getting called when the worker starts to run.
      // - You only have access to member variables stored in this worker.
      // - You do not have access to Javascript v8 objects here.
      void Execute() override {
          try {
              result_ = do_expensive_work(louder_, name_);
          } catch (const std::exception& e) {
              SetErrorMessage(e.what());
          }
      }  

      // The HandleOKCallback() is getting called when Execute() successfully completed.
      // - In case Execute() invoked SetErrorMessage("") this function is not getting called.
      // - You have access to Javascript v8 objects again
      // - You have to translate from C++ member variables to Javascript v8 objects
      // - Finally, you call the user's callback with your results
      void HandleOKCallback() override {
          Nan::HandleScope scope;  

          const auto argc = 2u;
          v8::Local<v8::Value> argv[argc] = {Nan::Null(), Nan::New<v8::String>(result_).ToLocalChecked()};  

          callback->Call(argc, argv);
      }  

      std::string result_;
      const bool louder_;
      std::string name_;
  };  


  NAN_METHOD(HelloObjectAsync::hello_async) {
    // "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    // Mention anything about "Unwrap"?
    HelloObjectAsync* h = Nan::ObjectWrap::Unwrap<HelloObjectAsync>(info.Holder());

    std::string name = h->name_;
    bool louder = false;

    // Check second argument, should be a 'callback' function.
    // This allows us to set the callback so we can use it to return errors instead of throwing.
    // Also, "info" comes from the NAN_METHOD macro, which returns differently according to the version of node
    if (!info[1]->IsFunction())
    {
        return Nan::ThrowTypeError("second arg 'callback' must be a function");
    }
    v8::Local<v8::Function> callback = info[1].As<v8::Function>();

    // Check first argument, should be an 'options' object
    if (!info[0]->IsObject())
    {
        return CallbackError("first arg 'options' must be an object", callback);
    }
    v8::Local<v8::Object> options = info[0].As<v8::Object>();

    // Check options object for the "louder" property, which should be a boolean value
    if (options->Has(Nan::New("louder").ToLocalChecked()))
    {
        v8::Local<v8::Value> louder_val = options->Get(Nan::New("louder").ToLocalChecked());
        if (!louder_val->IsBoolean())
        {
            return CallbackError("option 'louder' must be a boolean", callback);
        }
        louder = louder_val->BooleanValue();
    }

    // Create a worker instance and queues it to run asynchronously invoking the callback when done.
    // - Nan::AsyncWorker takes a pointer to a Nan::Callback and deletes the pointer automatically.
    // - Nan::AsyncQueueWorker takes a pointer to a Nan::AsyncWorker and deletes the pointer automatically.
    auto* worker = new AsyncHelloWorker{louder, name, new Nan::Callback{callback}};
    Nan::AsyncQueueWorker(worker);
  
  }

  // Singleton...not really sure what to say about this
  Nan::Persistent<v8::Function> &HelloObjectAsync::create_once()
  {
      static Nan::Persistent<v8::Function> init;
      return init;
  }

  void HelloObjectAsync::Init(v8::Local<v8::Object> target)
  {
      // A handlescope is needed so that v8 objects created in the local memory space (this function in this case)
      // are cleaned up when the function is done running (and the handlescope is destroyed)
      // Fun trivia: forgetting a handlescope is one of the most common causes of memory leaks in node.js core
      // https://www.joyent.com/blog/walmart-node-js-memory-leak
      Nan::HandleScope scope;

      // This is saying:
      // "Node, please allocate a new Javascript string object
      // inside the V8 local memory space, with the value 'HelloObjectAsync' "
      v8::Local<v8::String> whoami = Nan::New("HelloObjectAsync").ToLocalChecked();
       
      // Create the HelloObject
      auto fnTp = Nan::New<v8::FunctionTemplate>(HelloObjectAsync::New); // Passing the HelloObject::New method above
      fnTp->InstanceTemplate()->SetInternalFieldCount(1); // It's 1 when holding the ObjectWrap itself and nothing else
      fnTp->SetClassName(whoami); // Passing the Javascript string object above

      // Add custom methods here.
      // This is how helloAsync() is exposed as part of HelloObjectAsync.
      // This line is attaching the "helloAsync" method to a JavaScript function prototype.
      // "helloAsync" is therefore like a property of the fnTp object 
      // ex: console.log(HelloObjectAsync.helloAsync) --> [Function: helloAsync]
      SetPrototypeMethod(fnTp, "helloAsync", hello_async);

      // Create an unique instance of the HelloObject function template,
      // then set this unique instance to the target
      const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
      create_once().Reset(fn);  // calls the static &HelloObjectAsync::create_once method above. This ensures the instructions in this Init function are retained in memory even after this code block ends.
      Nan::Set(target, whoami, fn);
  }

}