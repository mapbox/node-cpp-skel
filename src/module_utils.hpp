#pragma once
#include <memory>
#include <nan.h>
#include <string>

namespace utils {

/*
* This is an internal function used to return callback error messages instead of
* throwing errors.
* Usage:
*
* v8::Local<v8::Function> callback;
* return CallbackError("error message", callback);  // "return" is important to
* prevent duplicate callbacks from being fired!
*
*
* "inline" is important here as well. See for more contex:
* - https://github.com/mapbox/cpp/blob/master/glossary.md#inline-keyword
* - https://github.com/mapbox/node-cpp-skel/pull/52#discussion_r126847394 for
* context
*
*/
inline void CallbackError(std::string message, v8::Local<v8::Function> func) {
    Nan::Callback cb(func);
    v8::Local<v8::Value> argv[1] = {Nan::Error(message.c_str())};
    Nan::Call(cb, 1, argv);
}

inline Nan::MaybeLocal<v8::Object> NewBufferFrom(std::unique_ptr<std::string>&& ptr) {
    Nan::MaybeLocal<v8::Object> res = Nan::NewBuffer(
        &(*ptr)[0],
        ptr->size(),
        [](char*, void* hint) {
            delete static_cast<std::string*>(hint);
        },
        ptr.get());
    if (!res.IsEmpty()) {
        ptr.release();
    }
    return res;
}

} // namespace utils
