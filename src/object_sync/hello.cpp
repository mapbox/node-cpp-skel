#include "hello.hpp"

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

// Custom constructor, assigns custom name passed in from Javascript world.
// This constructor uses member init list via the semicolon, aka "direct
// initialization"
// which is more efficient than using assignment operators.
HelloObject::HelloObject(std::string name) : name_(name) {}

// Triggered from Javascript world when calling "new HelloObjectAsync(name)"
NAN_METHOD(HelloObject::New)
{
    if (info.IsConstructCall())
    {
        try
        {
            if (info.Length() >= 1)
            {
                if (info[0]->IsString())
                {
                    std::string name = *v8::String::Utf8Value(info[0]->ToString());
                    auto* const self = new HelloObject(name);
                    self->Wrap(
                        info.This()); // Connects C++ object to Javascript object (this)
                }
                else
                {
                    return Nan::ThrowTypeError("arg must be a string");
                }
            }
            else
            {
                return Nan::ThrowTypeError("must provide string arg");
            }
        }
        catch (const std::exception& ex)
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

// NAN_METHOD is applicable to methods you want to expose to JS world
NAN_METHOD(HelloObject::hello)
{

    // Note: a HandleScope is automatically included inside NAN_METHOD. See the
    // docs at NAN that say:
    // 'Note that an implicit HandleScope is created for you on
    // JavaScript-accessible methods so you do not need to insert one yourself.'
    // at
    // https://github.com/nodejs/nan/blob/2dfc5c2d19c8066903a19ced6a72c06d2c825dec/doc/scopes.md#nanhandlescope

    // "What is node::ObjectWrap???" The short version is that node::ObjectWrap
    // and wrapping/unwrapping objects
    // is the somewhat clumsy way it is possible to bind Node and C++. The main
    // points to remember:
    // - To access a class instance inside a C++ static method, you must unwrap
    // the object.
    // - The C++ methods must be static to make them available at startup across
    // the language boundary (JS <-> C++).
    HelloObject* h = Nan::ObjectWrap::Unwrap<HelloObject>(info.Holder());

    // "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(
        Nan::New<v8::String>("...initialized an object...hello " + h->name_)
            .ToLocalChecked());
}

// This is a Singleton, which is a general programming design concept for
// creating an instance once within a process.
Nan::Persistent<v8::Function>& HelloObject::create_once()
{
    static Nan::Persistent<v8::Function> init;
    return init;
}

void HelloObject::Init(v8::Local<v8::Object> target)
{
    // A handlescope is needed so that v8 objects created in the local memory
    // space (this function in this case)
    // are cleaned up when the function is done running (and the handlescope is
    // destroyed)
    // Fun trivia: forgetting a handlescope is one of the most common causes of
    // memory leaks in node.js core
    // https://www.joyent.com/blog/walmart-node-js-memory-leak
    Nan::HandleScope scope;

    // This is saying:
    // "Node, please allocate a new Javascript string object
    // inside the V8 local memory space, with the value 'HelloObject' "
    v8::Local<v8::String> whoami = Nan::New("HelloObject").ToLocalChecked();

    // Officially create the HelloObject
    auto fnTp = Nan::New<v8::FunctionTemplate>(
        HelloObject::New); // Passing the HelloObject::New method above
    fnTp->InstanceTemplate()->SetInternalFieldCount(
        1);                     // It's 1 when holding the ObjectWrap itself and nothing else
    fnTp->SetClassName(whoami); // Passing the Javascript string object above

    // Add custom methods here.
    // This is how hello() is exposed as part of HelloObject.
    // This line is attaching the "hello" method to a JavaScript function
    // prototype.
    // "hello" is therefore like a property of the fnTp object
    // ex: console.log(HelloObject.hello) --> [Function: hello]
    SetPrototypeMethod(fnTp, "hello", hello);

    // Create an unique instance of the HelloObject function template,
    // then set this unique instance to the target
    const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
    create_once().Reset(fn); // calls the static &HelloObject::create_once method
                             // above. This ensures the instructions in this Init
                             // function are retained in memory even after this
                             // code block ends.
    Nan::Set(target, whoami, fn);
}

} // namespace object_sync