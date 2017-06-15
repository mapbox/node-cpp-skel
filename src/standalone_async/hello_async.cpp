#include "hello_async.hpp"

#include <exception>
#include <stdexcept>
#include <iostream>
#include <map>

namespace standalone_async {

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

  // expensive allocation of std::map, querying, and string comparison
  std::string do_expensive_work(bool louder, std::size_t work_to_do=100000) {  

      std::map<std::size_t,std::string> container;  

      for (std::size_t i=0;i<work_to_do;++i) {
          container.emplace(i,std::to_string(i));
      }  

      for (std::size_t i=0;i<work_to_do;++i) {
          std::string const& item = container[i];
          if (item != std::to_string(i)) {
              abort(); // should never get here
          }
      }  

      std::string result = "...threads are busy bees...world";
 
      if (louder)
      {
        result += "!!!!";
      }
     
      return result;  

  }  

  // This is the worker running asynchronously and calling a user-provided callback when done.
  // Consider storing all C++ objects you need by value or by shared_ptr to keep them alive until done.
  struct AsyncHelloWorker : Nan::AsyncWorker {
      using Base = Nan::AsyncWorker;  

      AsyncHelloWorker(bool louder, Nan::Callback* callback)
          : Base(callback), louder_(louder) { }  

      // The Execute() function is getting called when the worker starts to run.
      // - You only have access to member variables stored in this worker.
      // - You do not have access to Javascript v8 objects here.
      void Execute() override {
          try {
              result_ = do_expensive_work(louder_);
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
  };  

  // "hello" is a Standalone function because it's not a class.
  // If this function was not defined within a namespace, it would be in the global scope.
  NAN_METHOD(hello_async) {

    bool louder = false;

    // check second argument, should be a 'callback' function.
    // This allows us to set the callback so we can use it to return errors
    // instead of throwing as well.
    if (!info[1]->IsFunction())
    {
        Nan::ThrowTypeError("second arg 'callback' must be a function");
        return;
    }
    v8::Local<v8::Function> callback = info[1].As<v8::Function>();

    // check first argument, should be an 'options' object
    if (!info[0]->IsObject())
    {
        CallbackError("first arg 'options' must be an object", callback);
        return;
    }
    v8::Local<v8::Object> options = info[0].As<v8::Object>();

    if (options->Has(Nan::New("louder").ToLocalChecked()))
    {
        v8::Local<v8::Value> louder_val = options->Get(Nan::New("louder").ToLocalChecked());
        if (!louder_val->IsBoolean())
        {
            CallbackError("option 'louder' must be a boolean", callback);
            return;
        }
        louder = louder_val->BooleanValue();
    }

    // Create a worker instance and queues it to run asynchronously invoking the callback when done.
    // - Nan::AsyncWorker takes a pointer to a Nan::Callback and deletes the pointer automatically.
    // - Nan::AsyncQueueWorker takes a pointer to a Nan::AsyncWorker and deletes the pointer automatically.
    auto* worker = new AsyncHelloWorker{louder, new Nan::Callback{callback}};
    Nan::AsyncQueueWorker(worker);

    // // "info" comes from the NAN_METHOD macro, which returns differently
    // // according to the version of node
    // info.GetReturnValue().Set(Nan::New<v8::String>("world").ToLocalChecked());
  
  }

}


