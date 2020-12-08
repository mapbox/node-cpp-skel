#include "hello.hpp"
#include <memory>

/**
 * Synchronous class, called HelloObject
 * @class HelloObject
 * @example
 * var module = require('index.js');
 * var Obj = new module.HelloObject('greg');
 */

/**
 * Say hello
 *
 * @name hello
 * @memberof HelloObject
 * @returns {String}
 * @example
 * var x = Obj.hello();
 * console.log(x); // => '...initialized an object...hello greg'
 */

// If this was not defined within a namespace, it would be in the global scope.
// Namespaces are used because C++ has no notion of scoped modules, so all of
// the code you write in any file could conflict with other code.
// Namespaces are generally a great idea in C++ because it helps scale and
// clearly organize your application.
namespace object_sync {

Napi::FunctionReference HelloObject::constructor; // NOLINT

// Triggered from Javascript world when calling "new HelloObject(name)"
HelloObject::HelloObject(Napi::CallbackInfo const& info)
    : Napi::ObjectWrap<HelloObject>(info)
{
    Napi::Env env = info.Env();
    std::size_t length = info.Length();
    if (length != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return;
    }
    name_ = info[0].As<Napi::String>();
    // ^^ uses std::string() operator to convert to UTF-8 encoded string
    // alternatively Utf8Value() method can be used e.g
    // name_ = info[0].As<Napi::String>().Utf8Value();
    if (name_.empty())
    {
        Napi::TypeError::New(env, "arg must be a non-empty string").ThrowAsJavaScriptException();
    }
}

Napi::Value HelloObject::hello(Napi::CallbackInfo const& info)
{
    Napi::Env env = info.Env();
    return Napi::String::New(env, name_);
}

Napi::Object HelloObject::Init(Napi::Env env, Napi::Object exports)
{

    Napi::Function func = DefineClass(env, "HelloObject", {InstanceMethod("helloMethod", &HelloObject::hello)});
    // Create a peristent reference to the class constructor. This will allow
    // a function called on a class prototype and a function
    // called on instance of a class to be distinguished from each other.
    constructor = Napi::Persistent(func);
    // Call the SuppressDestruct() method on the static data prevent the calling
    // to this destructor to reset the reference when the environment is no longer
    // available.
    constructor.SuppressDestruct();
    exports.Set("HelloObject", func);
    return exports;
}

} // namespace object_sync
