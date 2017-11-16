![dancing skel](https://mapbox.s3.amazonaws.com/cpp-assets/node-cpp-skel-readme_blue.png)

A skeleton for binding C++ libraries to Node.js using [Nan](https://github.com/nodejs/nan). This is a small, helper repository that generates simple `HelloWorld` Javascript example constructors. The examples have a number of methods to demonstrate different ways to use Nan for building particular types of functionality (i.e. asynchronous functions). Use this repo as both a template for your own code as well as a learning tool if you're just starting to develop Node/C++ Addons.

**Why port C++ to Node.js?**. That's a great question! C++ is a high performance language that allows you to execute operations without clogging up the event loop. Node.js is single-threaded, which blocks execution. Even in highly optimized javascript code it may be impossible to improve performance. Passing heavy operations into C++ and subsequently into C++ workers can greatly improve the overall runtime of the code. Porting C++ code to Node.js is also referred to as creating an ["Addon"](https://github.com/mapbox/cpp/blob/master/node-cpp.md).

**Nan**: Nan is used in many C++ => Node.js port projects, such as [node-mapnik](https://github.com/mapnik/node-mapnik), [node-osrm](https://github.com/Project-OSRM/node-osrm), and [node-osmium](https://github.com/osmcode/node-osmium). More examples of how to port C++ libraries to node can be found at [nodejs.org/api/addons.html](https://nodejs.org/api/addons.html). See https://nodesource.com/blog/c-add-ons-for-nodejs-v4/ for a detailed summary of the origins of Nan.

[![Build Status](https://travis-ci.org/mapbox/node-cpp-skel.svg?branch=master)](https://travis-ci.org/mapbox/node-cpp-skel)
[![codecov](https://codecov.io/gh/mapbox/node-cpp-skel/branch/master/graph/badge.svg)](https://codecov.io/gh/mapbox/node-cpp-skel)

# What's in the box? :package:

This repository itself can be cloned and edited to your needs. The skeleton prepares a C++ port to Node.js and provides the following for quick development:

* **Tests**: created with [Tape](https://github.com/substack/tape) in the `test/` directory. Travis CI file is prepared to build and test your project on every push.
* **Documentation**: use this README as a template and customize for your own project. Also, this skeleton uses [documentation.js](http://documentation.js.org/) to generate API documentation from JSDOC comments in the `.cpp` files. Docs are located in `API.md`.
* **[Benchmarking](./docs/benchmarking.md)**: Easily test the performance of your code using the built-in benchmark tests provided in this skeleton.
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

Run tests locally with compile flags enabled to catch hard-to-find bugs (see [this PR](https://github.com/mapbox/node-cpp-skel/pull/85) for more info):

```shell
make sanitize
```

The sanitizers [are part of the compiler](https://github.com/mapbox/cpp/blob/master/glossary.md#sanitizers) and are also run in a specific job on Travis.

# Add Custom Code

Depending on your usecase, there are a variety of ways to start using this skeleton for your project.

### Setup new project
Easily use this skeleton as a starting off point for a _new_ custom project:

```
# Clone node-cpp-skel locally

git clone git@github.com:mapbox/node-cpp-skel.git
cd node-cpp-skel/

# Create your new repo on GitHub and have the remote repo url handy for liftoff
# Then run the liftoff script from within your local node-cpp-skel root directory.
#
# This will:
# - prompt you for the new name of your project and the new remote repo url
# - automatically create a new directory for your new project repo
# - create a new branch called "node-cpp-skel-port" within your new repo directory
# - add, commit, and push that branch to your new repo

./scripts/liftoff.sh

```

### Add your code
Once your project has ported node-cpp-skel, follow these steps to integrate your own code:

- Create a dir in `./src` to hold your custom code. See the example code within `/src` for reference.
- Add your new method or class to `./src/module.cpp`, and `#include` it at the top
- Add your new file-to-be-compiled to the list of target sources in `./binding.gyp`
- Run `make` and see what surprises await on your new journey :boat:

# Code coverage

Code coverage is critical for knowing how well your tests actually test all your code. To see code coverage you can view current results online at [![codecov](https://codecov.io/gh/mapbox/node-cpp-skel/branch/master/graph/badge.svg)](https://codecov.io/gh/mapbox/node-cpp-skel) or you can build in a customized way and display coverage locally like:

```
make coverage
```

For more details about what `make coverage` is doing under the hood see https://github.com/mapbox/cpp#code-coverage

# Contributing and License

Contributors are welcome! :sparkles: This repo exists as a place to gather C++/Node Addon knowledge that will benefit the larger community. Please contribute your knowledge if you'd like.

Node-cpp-skel is licensed under [CC0](https://creativecommons.org/share-your-work/public-domain/cc0/). Attribution is not required, but definitely welcome! If your project uses this skeleton, please add the node-cpp-skel badge to your readme so that others can learn about the resource.

[![badge](https://mapbox.s3.amazonaws.com/cpp-assets/node-cpp-skel-badge_blue.svg)](https://github.com/mapbox/node-cpp-skel)

To include the badge, paste this into your README.md file:
```
[![badge](https://mapbox.s3.amazonaws.com/cpp-assets/node-cpp-skel-badge_blue.svg)](https://github.com/mapbox/node-cpp-skel)
```

See [CONTRIBUTING](CONTRIBUTING.md) and [LICENSE](LICENSE.md) for more info.
