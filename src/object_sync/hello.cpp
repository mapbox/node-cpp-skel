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
// This constructor uses member init list via the semicolon, aka "direct initialization"
// which is more efficient than using assignment operators.
HelloObject::HelloObject(std::string&& name) : name_(std::move(name)) {}

// Triggered from Javascript world when calling "new HelloObject(name)"
NAN_METHOD(HelloObject::New) {
    if (info.IsConstructCall()) {
        try {
            if (info.Length() >= 1) {
                if (info[0]->IsString()) {
                    // Don't want to risk passing a null string around, which might create unpredictable behavior.
                    Nan::Utf8String utf8_value(info[0]);
                    int len = utf8_value.length();
                    if (len <= 0) {
                        return Nan::ThrowTypeError("arg must be a non-empty string");
                    }

                    /**
                     * This line converts a V8 string to a C++ std::string.
                     * In the background, it triggers memory allocation (stack allocating, but std:string is also dynamically allocating memory in the heap)
                     * We want to avoid heap allocation to ensure more performant code.
                     * See https://github.com/mapbox/cpp/blob/master/glossary.md#stack-allocation
                     * and https://stackoverflow.com/questions/79923/what-and-where-are-the-stack-and-heap/80113#80113
                     * Also, providing the length allows the std::string constructor to avoid calculating the length internally
                     * and should be faster since it skips an operation.
                     */
                    std::string name(*utf8_value, static_cast<std::size_t>(len));

                    /** 
                     * This line is where HelloObjectAsync takes ownership of "name" with the use of move semantics.
                     * Then all later usage of "name" are passed by reference (const&), but the actual home or address in memory
                     * will always be owned by this instance of HelloObjectAsync. Generally important to know what has ownership of an object.
                     * When a object/value is a member of a class (like "name"), we know the class (HelloObjectAsync) has full control of the scope of the object/value.
                     * This avoids the scenario of "name" being destroyed or becoming out of scope.
                     * 
                     * Also, we're using "new" here to create a custom C++ class, based on node::ObjectWrap since this is a node addon.
                     * In this case, "new" allocates a C++ object (dynamically on the heap) and then passes ownership (control of when it gets deleted) 
                     * to V8, the javascript engine which decides when to clean up the object based on how its’ garbage collector works.
                     * In other words, the memory of HelloObjectAsync is expliclty deleted via node::ObjectWrap when it's gone out of scope 
                     * (the object needs to stay alive until the V8 garbage collector has decided it's done):
                     * https://github.com/nodejs/node/blob/7ec28a0a506efe9d1c03240fd028bea4a3d350da/src/node_object_wrap.h#L124
                     */
                    auto* const self = new HelloObject(std::move(name));
                    self->Wrap(info.This()); // Connects C++ object to Javascript object (this)
                } else {
                    return Nan::ThrowTypeError(
                        "arg must be a string");
                }
            } else {
                return Nan::ThrowTypeError(
                    "must provide string arg");
            }
        } catch (const std::exception& ex) {
            return Nan::ThrowTypeError(ex.what());
        }

        info.GetReturnValue().Set(info.This());
    } else {
        return Nan::ThrowTypeError(
            "Cannot call constructor as function, you need to use 'new' keyword");
    }
}

// NAN_METHOD is applicable to methods you want to expose to JS world
NAN_METHOD(HelloObject::hello) {
    /**
     * Note: a HandleScope is automatically included inside NAN_METHOD. See the
     * docs at NAN that say:
     * 'Note that an implicit HandleScope is created for you on
     * JavaScript-accessible methods so you do not need to insert one yourself.'
     * at
     * https://github.com/nodejs/nan/blob/2dfc5c2d19c8066903a19ced6a72c06d2c825dec/doc/scopes.md#nanhandlescope

     * "What is node::ObjectWrap???" The short version is that node::ObjectWrap
     * and wrapping/unwrapping objects
     * is the somewhat clumsy way it is possible to bind Node and C++. The main
     * points to remember:
     *   - To access a class instance inside a C++ static method, you must unwrap
     * the object.
     *   - The C++ methods must be static to make them available at startup across
     * the language boundary (JS <-> C++).
     */
    auto* h = Nan::ObjectWrap::Unwrap<HelloObject>(info.Holder());

    // "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(
        Nan::New<v8::String>("...initialized an object...hello " + h->name_)
            .ToLocalChecked());
}

// This is a Singleton, which is a general programming design concept for
// creating an instance once within a process.
Nan::Persistent<v8::Function>& HelloObject::create_once() {
    static Nan::Persistent<v8::Function> init;
    return init;
}

void HelloObject::Init(v8::Local<v8::Object> target) {
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