#pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-parameter"
// #pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

class HelloWorld: public Nan::ObjectWrap 
{
    public:
        static NAN_MODULE_INIT(Init);
        static NAN_METHOD(New);
        static NAN_METHOD(wave);
        static Nan::Persistent<v8::Function> &constructor();
};