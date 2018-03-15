#pragma once
#include <nan.h>
// specify #include with carrots, ex: <nan.h> --> look for header in global
// specify #include with quotes, ex: "hello.hpp" --> look for header in location
// relative to this file

/*

  Namespace is an organizational method that helps to clearly show where a
  method is coming from.
  Namespaces are generally a great idea in C++ because they help us scale
  things. C++ has no notion of scoped modules,
  so all of the code you write in any file could potentially conflict with other
  classes/functions/etc.
  Namespaces help to differentiate pieces of your code.

  The convention In this skeleton is to name the namespace to match the name of
  the subdirectory where it lives.
  So in this case, the namespace is called "standalone" because this method
  lives within the "standalone" subdirectory.
  If there is another "hello" function used for another example, the compiler
  will know the difference between the two:

  standalone::hello

       VS

  potato::hello

*/
namespace standalone {

// hello, custom sync method tied to module.cpp
// method's logic lives in hello.cpp
NAN_METHOD(hello);
} // namespace standalone