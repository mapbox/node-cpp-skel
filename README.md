A skeleton for binding C++ libraries to Node.js using [Nan](https://github.com/nodejs/nan). This is a small, helper repository that generates a Javascript constructor called `HelloWorld`. `HelloWorld` has a number of methods to demonstrate different ways to use Nan for building particular types of functionality (i.e. asynchronous functions).

Nan is used in many C++ => Node.js port projects, such as Node Mapnik, Node OSRM, and Node Osmium. More examples of how to port C++ libraries to node can be found at [nodejs.org/api/addons.html](https://nodejs.org/api/addons.html).

**Why port C++ to Node.js?**. That's a great question! C++ is a highly performant language that allows you to execute operations without clogging up the event loop. Node.js is single-threaded, which blocks execution. Even in highly optimized javascript code it may be impossible to improve performance. Passing heavy operations into C++ and subsequently into C++ workers can greatly improve the overral runtime of the code.

[![Build Status](https://travis-ci.org/mapbox/node-cpp-skel.svg?branch=master)](https://travis-ci.org/mapbox/node-cpp-skel)
[![codecov](https://codecov.io/gh/mapbox/node-cpp-skel/branch/master/graph/badge.svg)](https://codecov.io/gh/mapbox/node-cpp-skel)

# What's in the box? :package:

The skeleton prepares a C++ port to Node.js and provides the following tools to enable quick development:

* **Tests**: tests are written on the JS side with Tape in the `test/` directory. Travis CI file is prepared to build and test your project on every push.
* **Documentation**: Using documentation.js `--polyglot` option to write JSDOC comments directly in the `.cpp` files and generate Markdown output of the API
* **Build system**: largely relies on the Makefile and node-pre-gyp to generate your project with the proper system architecture flags
* **Publishing**: Largely reliant on NPM to host JS dependencies and to publish your module to NPM's package registry.

# Architecture :hammer:

#### Class constructors

#### Methods

#### Asynchronous Methods


# Installation

```
git clone git@github.com:mapbox/node-cpp-skel.git
cd node-cpp-skel
npm install --build-from-source
npm test
```

# Usage

```javascript
var HelloWorld = require('./path/to/lib/index.js');
var HW = new HelloWorld();
var hi = HW.wave();
console.log(hi); // => howdy world!
```
