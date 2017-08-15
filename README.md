A skeleton for binding C++ libraries to Node.js using [Nan](https://github.com/nodejs/nan). This is a small, helper repository that generates simple `HelloWorld` Javascript example constructors. The examples have a number of methods to demonstrate different ways to use Nan for building particular types of functionality (i.e. asynchronous functions). Use this repo as both a template for your own code as well as a learning tool if you're just starting to develop Node/C++ Addons.

**Why port C++ to Node.js?**. That's a great question! C++ is a high performance language that allows you to execute operations without clogging up the event loop. Node.js is single-threaded, which blocks execution. Even in highly optimized javascript code it may be impossible to improve performance. Passing heavy operations into C++ and subsequently into C++ workers can greatly improve the overall runtime of the code. Porting C++ code to Node.js is also referred to as creating an ["Addon"](https://github.com/mapbox/cpp/blob/master/node-cpp.md).

**Nan**: Nan is used in many C++ => Node.js port projects, such as [node-mapnik](https://github.com/mapnik/node-mapnik), [node-osrm](https://github.com/Project-OSRM/node-osrm), and [node-osmium](https://github.com/osmcode/node-osmium). More examples of how to port C++ libraries to node can be found at [nodejs.org/api/addons.html](https://nodejs.org/api/addons.html). See https://nodesource.com/blog/c-add-ons-for-nodejs-v4/ for a detailed summary of the origins of Nan.

[![Build Status](https://travis-ci.org/mapbox/node-cpp-skel.svg?branch=master)](https://travis-ci.org/mapbox/node-cpp-skel)
[![codecov](https://codecov.io/gh/mapbox/node-cpp-skel/branch/master/graph/badge.svg)](https://codecov.io/gh/mapbox/node-cpp-skel)

# What's in the box? :package:

This repository itself can be cloned and edited to your needs. The skeleton prepares a C++ port to Node.js and provides the following for quick development:

* **Tests**: created with [Tape](https://github.com/substack/tape) in the `test/` directory. Travis CI file is prepared to build and test your project on every push.
* **Documentation**: use this README as a template and customize for your own project. Also, this skeleton uses [documentation.js](http://documentation.js.org/) to generate API documentation from JSDOC comments in the `.cpp` files. Docs are located in `API.md`.
* **Build system**: [node-pre-gyp](https://github.com/mapbox/node-pre-gyp) generates binaries with the proper system architecture flags
* **[Publishing](./docs/publishing-binaries.md)**: Structured as a node module with a `package.json` that can be deployed to NPM's registry.
* **Learning resources**: Read the detailed inline comments within the example code to learn exactly what is happening behind the scenes. Also, check out the [extended tour](./docs/extended-tour.md) to learn more about Node/C++ Addon development, builds, Xcode, and more details about the configuration of this skeleton.

# Installation

Each `make` command is specified in [`Makefile`](./Makefile)

```bash
git clone git@github.com:mapbox/node-cpp-skel.git
cd node-cpp-skel

# Build binaries. This looks to see if there were changes in the C++ code. This does not reinstall deps.
make

# Run tests
make test

# Cleans your current builds and removes potential cache
make clean

# Cleans everything, including the things you download from the network in order to compile (ex: npm packages).
# This is useful if you want to nuke everything and start from scratch.
# For example, it's super useful for making sure everything works for Travis, production, someone else's machine, etc
make distclean

# This skel uses documentation.js to auto-generate API docs.
# If you'd like to generate docs for your code, you'll need to install documentation.js,
# and then add your subdirectory to the docs command in package.json
npm install -g documentation
npm run docs
```

Note: by default the build errors on compiler warnings. To disable this do:

```
WERROR=false make
```

# Add Custom Code

`node-cpp-skel` was designed to make adding custom code simple and scalable, to form to whatever usecase you may need. Here's how:

- Create a dir in `./src` to hold your custom code. See the example code within `/src` for reference.
- Add your new method or class to `./src/module.cpp`, and `#include` it at the top
- Add your new file-to-be-compiled to the list of target sources in `./binding.gyp`

# Code coverage

Code coverage is critical for knowing how well your tests actually test all your code. To see code coverage you can view current results online at [![codecov](https://codecov.io/gh/mapbox/node-cpp-skel/branch/master/graph/badge.svg)](https://codecov.io/gh/mapbox/node-cpp-skel) or you can build in a customized way and display coverage locally like:

```
make coverage
```

For more details about what `make coverage` is doing under the hood see https://github.com/mapbox/cpp#code-coverage

# Benchmarking

This project includes [bench tests](https://github.com/mapbox/node-cpp-skel/tree/master/bench) you can use to experiment with and measure performance. We've included two bench tests for the async examples, demonstrating the affects of concurrency and threads within a process or processes.

For example, you can run:

```
node bench/hello_async.bench.js --iterations 50 --concurrency 10
```

This will run a bunch of calls to the module's `helloAsync()` function. You can control two things:

- iterations: number of times to call `helloAsync()`
- concurrency: max number of threads the test can utilize, by setting `UV_THREADPOOL_SIZE`. When running the bench script, you can see this number of threads reflected in your [Activity Monitor](https://github.com/springmeyer/profiling-guide#activity-monitorapp-on-os-x)/[htop window](https://hisham.hm/htop/). 

Note: These bench tests are not completely "realworld" yet since the code does not do much. Their main benefit is allowing you to adapt them to your custom code and enabling you to monitor the performance of your code.

### Ideal Benchmarks

**Ideally, you want your workers to run your code ~99% of the time.** This reflects a healthy node c++ addon and what you would expect to see when you've picked a good problem to solve with node.

The bench tests and async functions that come with `node-cpp-skel` out of the box demonstrate this behaviour:
- An async function that is CPU intensive and takes a while to finish (expensive creation and querying of a `std::map` and string comparisons). 
- Worker threads are busy doing a lot of work, and the main loop is relatively idle. Depending on how many threads (concurrency) you enable, you may see your CPU% sky-rocket and your cores max out. Yeaahhh!!!
- If you bump up `--iterations` to 500 and profile in Activity Monitor.app, you'll see the main loop is idle as expected since the threads are doing all the work. You'll also see the threads busy doing work in AsyncHelloWorker roughly 99% of the time :tada:

![](https://user-images.githubusercontent.com/1209162/29333300-e7c483e2-81c8-11e7-8253-1beb12173841.png)
