A skeleton for binding C++ libraries to Node.js using [Nan](https://github.com/nodejs/nan). This is a small, helper repository that generates a Javascript constructor called `HelloWorld`. `HelloWorld` has a number of methods to demonstrate different ways to use Nan for building particular types of functionality (i.e. asynchronous functions).

Nan is used in many C++ => Node.js port projects, such as [node-mapnik](https://github.com/mapnik/node-mapnik), [node-osrm](https://github.com/Project-OSRM/node-osrm), and [node-osmium](https://github.com/osmcode/node-osmium). More examples of how to port C++ libraries to node can be found at [nodejs.org/api/addons.html](https://nodejs.org/api/addons.html). See https://nodesource.com/blog/c-add-ons-for-nodejs-v4/ for a detailed summary of the origins of Nan.

**Why port C++ to Node.js?**. That's a great question! C++ is a high performance language that allows you to execute operations without clogging up the event loop. Node.js is single-threaded, which blocks execution. Even in highly optimized javascript code it may be impossible to improve performance. Passing heavy operations into C++ and subsequently into C++ workers can greatly improve the overall runtime of the code.

[![Build Status](https://travis-ci.org/mapbox/node-cpp-skel.svg?branch=master)](https://travis-ci.org/mapbox/node-cpp-skel)
[![codecov](https://codecov.io/gh/mapbox/node-cpp-skel/branch/master/graph/badge.svg)](https://codecov.io/gh/mapbox/node-cpp-skel)

# What's in the box? :package:

The skeleton prepares a C++ port to Node.js and provides the following for quick development:

* **Tests**: created with [Tape](https://github.com/substack/tape) in the `test/` directory. Travis CI file is prepared to build and test your project on every push.
* **Documentation**: uses [documentation.js](http://documentation.js.org/) to generate API documentation from JSDOC comments in the `.cpp` files. Docs are located in `API.md`.
* **Build system**: [node-pre-gyp](https://github.com/mapbox/node-pre-gyp) generates binaries with the proper system architecture flags
* **Publishing**: Structured as a node module with a `package.json` that can be deployed to NPM's registry.

# Installation

This repository itself can be cloned and edited to your needs.

```
git clone git@github.com:mapbox/node-cpp-skel.git
cd node-cpp-skel
make # build binaries
make test
```

# Xcode

![npm-test scheme in Xcode](https://cloud.githubusercontent.com/assets/52399/16448893/4967a454-3df4-11e6-8bb5-701fe46174ef.png)

If you're developing on macOS and have Xcode installed, you can also type `make xcode` to generate and open an Xcode project. In the dropdown, choose `npm test` to run the npm tests. You can also add more targets by adding the appropriate lines in `Makefile`, and rerunning `make xcode`. If you are modifying `binding.gyp`, e.g. by adding more source files, make sure to rerun `make xcode` so that Xcode knows about the new source files.

# Usage

```javascript
var HelloWorld = require('./path/to/lib/index.js');
var HW = new HelloWorld();
var hi = HW.wave();
console.log(hi); // => howdy world!
```
