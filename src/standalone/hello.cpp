#include "hello.hpp"

namespace standalone {

  /**
  * This is a standalone function (sync) that logs a string.
  * @name hello
  * @returns {string} "world"
  * @example
  * var module = require('./path/to/lib/index.js');
  * var check = module.hello();
  * console.log(check); // => "world"
  */

  // hello is a "standalone function" because it's not a class.
  // If this function was not defined within a namespace, it would be in the global scope.
  NAN_METHOD(hello) {

    // "info" comes from the NAN_METHOD macro, which returns differently
    // according to the version of node
    info.GetReturnValue().Set(Nan::New<v8::String>("world").ToLocalChecked());
  
  }
} // namespace standalone