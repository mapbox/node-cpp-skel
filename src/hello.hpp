#pragma once
// carrots --> look for header in global
// quotes --> look for header in location relative to this file
#include <nan.h>

// hello, custom sync method tied to module.cpp
// method's logic lives in hello.cpp
NAN_METHOD(hello);