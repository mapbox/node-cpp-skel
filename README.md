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

# Code coverage

Code coverage is critical for knowing how well your tests actually test all your code. To see code coverage you can view current results online at [![codecov](https://codecov.io/gh/mapbox/node-cpp-skel/branch/master/graph/badge.svg)](https://codecov.io/gh/mapbox/node-cpp-skel) or you can build in a customized way and display coverage locally like:

```
make coverage
```

For more details about what `make coverage` is doing under the hood see https://github.com/mapbox/cpp#code-coverage


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

# Publishing Binaries

**Setup**

It's a good idea to publish pre-built binaries of your module if you want others to be able to install it properly on their own systems, which can very likely not have a compiler like `gcc` or `clang`. Node-pre-gyp does a lot of the heavy lifting for us (like detecting which system you are building on and deploying to s3) but you'll need a few things configured to get started.

1. In the `package.json`, update the `"binary"` field to the appropriate s3 bucket `host`.
1. If you plan on publishing binaries via Travis-CI, [setup your AWS security credentials in the `.travis.yml`](https://github.com/mapbox/node-pre-gyp#travis-automation) - this gives the travis environment the proper access to the bucket named above.

**Publishing on Travis CI**

This project includes a `script/publish.sh` command that runs on travis if the environment variable `PUBLISHABLE` is set to `true`. You can set the environment variable like this. This command checks your commit message for either `[publish binary]` or `[republish binary]` in order to begin publishing. This allows you to publish binaries according to the version specified in your `package.json` like this:

```
git commit -m 'releasing 0.1.0 [publish binary]'
```

Republishing a binary overrides the current version and must be specified with `[republish binary]`.

# Benchmark Performance

This project includes [bench tests](https://github.com/mapbox/node-cpp-skel/tree/master/test/bench) you can use to experiment with and measure performance. We've included a couple different scenarios that demonstrate the affects of concurrency and threads within a process or processes.

For example, you can run:

```
node test/bench/bench-batch.js --iterations 50 --concurrency 10
```

This will run a batch/bundle/basket of calls to HelloWorld's `shout()` function. You can control two things:

- iterations: number of times to call `shout()`
- concurrency: max number of threads the test can utilize, by setting `UV_THREADPOOL_SIZE`. When running the bench-batch test, you can see this number of threads reflected in your [Activity Monitor](https://github.com/springmeyer/profiling-guide#activity-monitorapp-on-os-x)/[htop window](https://hisham.hm/htop/). 

This bench-batch test can demonstrate various performance scenarios:

1. An async function that is super CPU intensive and takes a while to finish (expensive allocation of std::map). This scenario demonstrates when worker threads are busy doing a lot of work, and the main loop is relatively idle. Depending on how many threads (concurrency) you enable, you may see your CPU% sky-rocket and your cores max out. Yeaahhh!!!

2. An async function that sleeps in the thread pool. This scenario demonstrates when worker threads are busy, but aren't doing much work and causing a bottlenech. Typically in this situation, the callstack of your process will show your workers spending most of their time in some kind of 'cond_wait' state.

### Activity Monitor will display a few different kinds of threads:
- main thread (this is the event loop)
- [worker threads (libuv)](https://github.com/libuv/libuv/blob/1a96fe33343f82721ba8bc93adb5a67ddcf70ec4/src/threadpool.c#L64-L104) will include `worker (in node)` in the callstack. These are usually unnamed: `Thread_2206161` (some of these might not actually be running your code)
- V8 WorkerThread: we dont really need to care about these right now. They dont actually run your code.

To learn more about what exactly is happening with threads behind the scenes in Node and how `UV_THREADPOOL_SIZE` is involved, check out [this great blogpost](https://www.future-processing.pl/blog/on-problems-with-threads-in-node-js/).

Feel free to play around with these bench tests and profile to get a better idea of how threading can affect the performance of your code. We are in the process of [adding more benchmarks](https://github.com/mapbox/node-cpp-skel/issues/30) that demonstrate a number of other scenarios.