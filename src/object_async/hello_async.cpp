#include "hello_async.hpp"
#include "../module_utils.hpp"

#include <exception>
#include <iostream>
#include <map>
#include <stdexcept>

/**
 * Asynchronous class, called HelloObjectAsync
 * @class HelloObjectAsync
 * @example
 * var module = require('index.js');
 * var Obj = new module.HelloObjectAsync('greg');
 */

/**
 * Say hello while doing expensive work in threads
 *
 * @name helloAsync
 * @memberof HelloObjectAsync
 * @param {Object} args - different ways to alter the string
 * @param {boolean} args.louder - adds exclamation points to the string
 * @param {Function} callback - from whence the hello comes, returns a string
 * @returns {String}
 * @example
 * var module = require('index.js');
 * var Obj = new module.HelloObjectAsync('greg');
 * Obj.helloAsync({ louder: true }, function(err, result) {
 *   if (err) throw err;
 *   console.log(result); // => '...threads are busy async bees...hello greg!!!'
 * });
 */

// If this was not defined within a namespace, it would be in the global scope.
namespace object_async {

// Custom constructor, assigns custom name arg passed in from Javascript world.
// This constructor uses member init list via the colon, aka "direct
// initialization", which is more efficient than using assignment operators.
// This constructor is using move semantics to literally "move" the value of
// name to a new place in memory (to the "name_" variable).
// This avoids copying the value and duplicating memory allocation, which can
// negatively affect performance.
HelloObjectAsync::HelloObjectAsync(std::string&& name)
    : name_(std::move(name)) {}

// Triggered from Javascript world when calling "new HelloObjectAsync(name)"
NAN_METHOD(HelloObjectAsync::New) {
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
                    **/
                    auto* const self = new HelloObjectAsync(std::move(name));
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

// This function performs expensive allocation of std::map, querying, and string
// comparison, therefore threads are nice & busy.
// Also, notice that name is passed by reference (std::string const& name)
std::string do_expensive_work(bool louder, std::string const& name) {

    std::map<std::size_t, std::string> container;
    std::size_t work_to_do = 100000;

    for (std::size_t i = 0; i < work_to_do; ++i) {
        container.emplace(i, std::to_string(i));
    }

    for (std::size_t i = 0; i < work_to_do; ++i) {
        std::string const& item = container[i];

        if (item != std::to_string(i)) {

            // AsyncHelloWorker's Execute function will take care of this error
            // and return it to js-world via callback
            // Marked NOLINT to avoid clang-tidy cert-err60-cpp error which we cannot
            // avoid on some linux distros where std::runtime_error is not properly
            // marked noexcept. Details at https://www.securecoding.cert.org/confluence/display/cplusplus/ERR60-CPP.+Exception+objects+must+be+nothrow+copy+constructible
            throw std::runtime_error("Uh oh, this should never happen"); // NOLINT
        }
    }

    std::string result = "...threads are busy async bees...hello " + name;

    if (louder) {
        result += "!!!!";
    }

    return result;
}

// This is the worker running asynchronously and calling a user-provided
// callback when done.
// Consider storing all C++ objects you need by value or by shared_ptr to keep
// them alive until done.
// Nan AsyncWorker docs:
// https://github.com/nodejs/nan/blob/master/doc/asyncworker.md
struct AsyncHelloWorker : Nan::AsyncWorker // NOLINT to disable cppcoreguidelines-special-member-functions
{

    using Base = Nan::AsyncWorker;
    // Make this class noncopyable
    AsyncHelloWorker(AsyncHelloWorker const&) = delete;
    AsyncHelloWorker& operator=(AsyncHelloWorker const&) = delete;

    AsyncHelloWorker(bool louder, const std::string* name,
                     Nan::Callback* cb)
        : Base(cb), result_{""}, louder_{louder}, name_{name} {}

    // The Execute() function is getting called when the worker starts to run.
    // - You only have access to member variables stored in this worker.
    // - You do not have access to Javascript v8 objects here.
    void Execute() override {
        try {
            result_ = do_expensive_work(louder_, *name_);
        } catch (const std::exception& e) {
            SetErrorMessage(e.what());
        }
    }

    // The HandleOKCallback() is getting called when Execute() successfully
    // completed.
    // - In case Execute() invoked SetErrorMessage("") this function is not
    // getting called.
    // - You have access to Javascript v8 objects again
    // - You have to translate from C++ member variables to Javascript v8 objects
    // - Finally, you call the user's callback with your results
    void HandleOKCallback() override {
        Nan::HandleScope scope;

        const auto argc = 2u;
        v8::Local<v8::Value> argv[argc] = {
            Nan::Null(), Nan::New<v8::String>(result_).ToLocalChecked()};

        // Static cast done here to avoid 'cppcoreguidelines-pro-bounds-array-to-pointer-decay' warning with clang-tidy
        callback->Call(argc, static_cast<v8::Local<v8::Value>*>(argv));
    }

    std::string result_;
    const bool louder_;
    // We use a pointer here to avoid copying the string data.
    // This works because we know that the original string we are
    // pointing to will be kept in scope/alive for the time while AsyncHelloWorker
    // is using it. If we could not guarantee this then we would need to either
    // copy the string or pass a shared_ptr<std::string>.
    const std::string* name_;
};

NAN_METHOD(HelloObjectAsync::helloAsync) {
    // "info" comes from the NAN_METHOD macro, which returns differently according
    // to the Node version
    // "What is node::ObjectWrap???" The short version is that node::ObjectWrap
    // and wrapping/unwrapping objects
    // is the somewhat clumsy way it is possible to bind Node and C++. The main
    // points to remember:
    // - To access a class instance inside a C++ static method, you must unwrap
    // the object.
    // - The C++ methods must be static to make them available at startup across
    // the language boundary (JS <-> C++).
    auto* h =
        Nan::ObjectWrap::Unwrap<HelloObjectAsync>(info.Holder());

    bool louder = false;

    // Check second argument, should be a 'callback' function.
    // This allows us to set the callback so we can use it to return errors
    // instead of throwing.
    // Also, "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    if (!info[1]->IsFunction()) {
        return Nan::ThrowTypeError("second arg 'callback' must be a function");
    }
    v8::Local<v8::Function> callback = info[1].As<v8::Function>();

    // Check first argument, should be an 'options' object
    if (!info[0]->IsObject()) {
        return utils::CallbackError("first arg 'options' must be an object",
                                    callback);
    }
    v8::Local<v8::Object> options = info[0].As<v8::Object>();

    // Check options object for the "louder" property, which should be a boolean
    // value
    if (options->Has(Nan::New("louder").ToLocalChecked())) {
        v8::Local<v8::Value> louder_val =
            options->Get(Nan::New("louder").ToLocalChecked());
        if (!louder_val->IsBoolean()) {
            return utils::CallbackError("option 'louder' must be a boolean",
                                        callback);
        }
        louder = louder_val->BooleanValue();
    }

    // Create a worker instance and queues it to run asynchronously invoking the
    // callback when done.
    // - Nan::AsyncWorker takes a pointer to a Nan::Callback and deletes the
    // pointer automatically.
    // - Nan::AsyncQueueWorker takes a pointer to a Nan::AsyncWorker and deletes
    // the pointer automatically.
    auto* worker =
        new AsyncHelloWorker{louder, &h->name_, new Nan::Callback{callback}};
    Nan::AsyncQueueWorker(worker);
}

// Singleton
Nan::Persistent<v8::Function>& HelloObjectAsync::create_once() {
    static Nan::Persistent<v8::Function> init;
    return init;
}

void HelloObjectAsync::Init(v8::Local<v8::Object> target) {
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
    // inside the V8 local memory space, with the value 'HelloObjectAsync' "
    v8::Local<v8::String> whoami = Nan::New("HelloObjectAsync").ToLocalChecked();

    // Create the HelloObject
    auto fnTp = Nan::New<v8::FunctionTemplate>(
        HelloObjectAsync::New); // Passing the HelloObject::New method above
    fnTp->InstanceTemplate()->SetInternalFieldCount(
        1);                     // It's 1 when holding the ObjectWrap itself and nothing else
    fnTp->SetClassName(whoami); // Passing the Javascript string object above

    // Add custom methods here.
    // This is how helloAsync() is exposed as part of HelloObjectAsync.
    // This line is attaching the "helloAsync" method to a JavaScript function
    // prototype.
    // "helloAsync" is therefore like a property of the fnTp object
    // ex: console.log(HelloObjectAsync.helloAsync) --> [Function: helloAsync]
    SetPrototypeMethod(fnTp, "helloAsync", helloAsync);

    // Create an unique instance of the HelloObject function template,
    // then set this unique instance to the target
    const auto fn = Nan::GetFunction(fnTp).ToLocalChecked();
    create_once().Reset(fn); // calls the static &HelloObjectAsync::create_once
                             // method above. This ensures the instructions in
                             // this Init function are retained in memory even
                             // after this code block ends.
    Nan::Set(target, whoami, fn);
}
} // namespace object_async