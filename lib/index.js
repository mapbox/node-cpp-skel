"use strict";

const {
  hello,
  helloAsync,
  helloPromise,
  HelloObject,
  HelloObjectAsync
} = require('./binding/module.node');

module.exports = {
  /**
   * This is a synchronous standalone function that logs a string.
   * @name hello
   * @returns {string}
   * @example
   * const { hello } = require('@mapbox/node-cpp-skel');
   * const check = hello();
   * console.log(check); // => "hello world"
   */
  hello,

  /**
   * This is an asynchronous standalone function that logs a string.
   * @name helloAsync
   * @param {Object} args - different ways to alter the string
   * @param {boolean} args.louder - adds exclamation points to the string
   * @param {boolean} args.buffer - returns value as a node buffer rather than a string
   * @param {Function} callback - from whence the hello comes, returns a string
   * @returns {string}
   * @example
   * const { helloAsync } = require('@mapbox/node-cpp-skel');
   * helloAsync({ louder: true }, function(err, result) {
   *   if (err) throw err;
   *   console.log(result); // => "...threads are busy async bees...hello
   * world!!!!"
   * });
   */
  helloAsync,

  /**
   * This is a function that returns a promise. It multiplies a string N times.
   * @name helloPromise
   * @param {Object} [options] - different ways to alter the string
   * @param {string} [options.phrase=hello] - the string to multiply
   * @param {Number} [options.multiply=1] - duplicate the string this number of times
   * @returns {Promise}
   * @example
   * const { helloPromise } = require('@mapbox/node-cpp-skel');
   * const result = await helloAsync({ phrase: 'Howdy', multiply: 3 });
   * console.log(result); // HowdyHowdyHowdy
   */
  helloPromise,

  /**
   * Synchronous class, called HelloObject
   * @class HelloObject
   * @example
   * const { HelloObject } = require('@mapbox/node-cpp-skel');
   * const Obj = new HelloObject('greg');
   */

  /**
   * Say hello
   *
   * @name hello
   * @memberof HelloObject
   * @returns {String}
   * @example
   * const x = Obj.hello();
   * console.log(x); // => '...initialized an object...hello greg'
   */
   HelloObject,

  /**
   * Asynchronous class, called HelloObjectAsync
   * @class HelloObjectAsync
   * @example
   * const { HelloObjectAsync } = require('@mapbox/node-cpp-skel');
   * const Obj = new module.HelloObjectAsync('greg');
   */

  /**
   * Say hello while doing expensive work in threads
   *
   * @name helloAsync
   * @memberof HelloObjectAsync
   * @param {Object} args - different ways to alter the string
   * @param {boolean} args.louder - adds exclamation points to the string
   * @param {buffer} args.buffer - returns object as a node buffer rather then string
   * @param {Function} callback - from whence the hello comes, returns a string
   * @returns {String}
   * @example
   * const { HelloObjectAsync } = require('@mapbox/node-cpp-skel');
   * const Obj = new HelloObjectAsync('greg');
   * Obj.helloAsync({ louder: true }, function(err, result) {
   *   if (err) throw err;
   *   console.log(result); // => '...threads are busy async bees...hello greg!!!'
   * });
   */
   HelloObjectAsync
};