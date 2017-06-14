#pragma once
#include <nan.h>
// carrots, ex: <nan.h> --> look for header in global
// quotes, ex: "hello.hpp" --> look for header in location relative to this file

// hello, custom sync method tied to module.cpp
// method's logic lives in hello.cpp
NAN_METHOD(hello);