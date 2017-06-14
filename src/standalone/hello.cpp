#include "hello.hpp"

// Standalone function (because it's not a class)
NAN_METHOD(hello) {

    // info comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(Nan::New<v8::String>("world").ToLocalChecked());

}