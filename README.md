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
documentation build src/your_dir/*.cpp --polyglot -f md -o API.md
```

Note: by default the build errors on compiler warnings. To disable this do:

```
WERROR=false make
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

### Standalone sync function
```javascript
var module = require('./path/to/lib/index.js');
var check = module.hello();

console.log(check); // => world
```

#### When would you use a standalone synchronous function?
...

### Standalone async function
```javascript
var module = require('./path/to/lib/index.js');

module.hello_async({ louder: true }, function(err, result) {
  if (err) throw err;

  console.log(result); // => "...threads are busy async bees...world!!!!""
});
```

#### When would you use a standalone async function?
...

### Object
```javascript
var module = require('./path/to/lib/index.js');
var Obj = new module.HelloObject();
var hi = Obj.hello();

console.log(hi); // => "...initialized an object...hello world"
```

#### When would you use an object or class?
- You need to do some preprocessing of data before going into the thread pool. So you're writing the code that makes sense to do that preprocessing once as a separate operation, then going through to the thread pool after the object is ready. Examples:
  - [vt-shaver](https://github.com/mapbox/vt-shaver-cpp/blob/a5908ccf61c04d54c0b01415ec986d736f5c6c4f/src/filters.hpp#L10): we create the Filter once, then pass the Filter object into Shaver multiple times. 
  - [node-mapnik](https://github.com/mapnik/node-mapnik/blob/fe80ce5d79c0e90cfbb5a2b992bf0ae2b8f88198/src/mapnik_map.hpp#L20): we create a Map object once, then use the object multiple times for rendering each vector tile.
- Classes are useful when you start doing more complex operations and need to consider performance more heavily, when performance constraints start to matter. Use classes to compute the value of something once, rather than every time you call a function.
- Another [example](https://github.com/nodejs/node-addon-examples/tree/master/6_object_wrap/nan)
- Say something about ObjectWrap?

# Add Custom Code
`node-cpp-skel` was designed to make adding custom code simple and scalable, to form to whatever use-case you may need. Here's how!

- Create a dir in `./src` to hold your custom code. See `./src/standalone` as an example.
- Add your new method or class to `./src/module.cpp`, and `#include` it at the top
- Add your new file-to-be-compiled to the list of target sources in `./binding.gyp`

# Builds

### What does "build" mean?

When you "build" your module, you are compiling and linking your C++ [source code](https://github.com/mapbox/cpp/blob/master/glossary.md#source-code) to produce a binary file that allows Node.js to load and run your module. Once loaded your C++ module can be used as if it were a Javascript module. A large list of tools come together to make this possible. In the below section we'll describe:

 - [Software build components](#software-build-components)
 - [Listing of [configuration files](#configuration-files) for the build
 - [Listing of [autogenerated files](#autogenerated-files) created when you run a build
 - Overall [build flow](#build-flow) of what happens when you run a build

#### Software build components

The primary components involved in the build

##### node-pre-gyp

node-pre-gyp is a javascript command line tool used in this project as a [front end](https://github.com/mapbox/cpp/blob/master/glossary.md#front-end) to [node-gyp](#node-gyp) to either compile your code or install it from binaries.

It is installed as a dependency in the [package.json](./package.json).

Learn more about node-pre-gyp [here](https://github.com/mapbox/node-pre-gyp)

##### node-gyp

node-gyp is a javascript command line tool used in this project as a [front end](https://github.com/mapbox/cpp/blob/master/glossary.md#front-end) to [gyp](#gyp).

node-gyp is bundled inside [npm](#npm) and does not need to be installed separately. Although, if installed in [package.json](./package.json), that version will be used by [node-pre-gyp](#node-pre-gyp).

Learn more about node-gyp [here](https://github.com/nodejs/node-gyp)

##### gyp

gyp is a python command line tool used in this project as a [front end](https://github.com/mapbox/cpp/blob/master/glossary.md#front-end) to [make](#make).

Learn more about gyp [here](https://github.com/mapbox/cpp/blob/master/glossary.md#gyp)

##### make

make is a command line tool, written in C, that is installed by default on most unix systems. It is used in this project in two ways:

- We provide a `Makefile` that acts as a simple entry point for developers wanting to source compile node-pre-gyp
- When [node-gyp](#node-gyp) is run, it generates a set of `Makefile`s automatically which are used to call out to the [compiler](#compiler) and [linker](#linker) to assemble your binary C++ module.

Learn more about make [here](https://github.com/mapbox/cpp/blob/master/glossary.md#make)

##### compiler

The command line program able to compile C++ source code, in this case `clang++`.

Learn more about what a compiler is [here](https://github.com/mapbox/cpp/blob/master/glossary.md#compiler)

##### linker

The command line program able to link C++ source code, in this case also `clang++`, which acts as a [front end](https://github.com/mapbox/cpp/blob/master/glossary.md#front-end) to the [system linker](https://github.com/mapbox/cpp/blob/master/glossary.md#linker)

#### Configuration files

Files you will find inside this repo and their purpose. For more info look inside each file for detailed comments.

- [Makefile](./Makefile) - entry point to building from source. This is invoked when you type `make` in the root directory. By default the `default` target is run which maps to the `release` target. See the comments inside the Makefile for more detail.
- [binding.gyp](./binding.gyp) - JSON configuration file for [node-gyp](#node-gyp). Must be named `binding.gyp` and present in the root directory so that `npm` detects it. Will be passed to [gyp](#gyp) by [node-gyp](#node-gyp). Because [gyp](#gyp) is python and has less strict JSON parsing rules, code comments with `#` are allowed (this would not be the case if parsed with node.js).
- [common.gypi](./common.gypi) - "gypi" stands for gyp include file. This is referenced by the [binding.gyp](./binding.gyp)
- [package.json](./package.json) - configuration file for npm. But it also contains a custom `binary` object that is the configuration for [node-pre-gyp](#node-pre-gyp).
- [lib/index.js](./lib/index.js) - entry point for the javascript module. Referenced in the `main` property in the [package.json](./package.json). This is the file that is run when the module is loaded by `require` from another module.
- [scripts/setup.sh](./scripts/setup.sh) - script used to 1) install [Mason](https://github.com/mapbox/cpp/blob/master/glossary.md#mason) and [clang++](https://github.com/mapbox/cpp/blob/master/glossary.md#clang-1) and 2) create a `local.env` that can be sourced in `bash` in order to set up [Mason](https://github.com/mapbox/cpp/blob/master/glossary.md#mason) and [clang++](https://github.com/mapbox/cpp/blob/master/glossary.md#clang-1) on your PATH.
- [scripts/install_deps.sh](./scripts/install_deps.sh) - script that invokes [Mason](https://github.com/mapbox/cpp/blob/master/glossary.md#mason) to install mason packages
- [scripts/publish.sh](./scripts/publish.sh) - script to publish the C++ binary module with [node-pre-gyp](#node-pre-gyp). Designed to be run on [travisci.org](https://travis-ci.org/)
- [.travis.yml](./.travis.yml) - configuration for this module on [travisci.org](https://travis-ci.org/). Used to test the code and publish binaries for various node versions and compiler options.

#### Autogenerated files

Files you will notice are created when you build from source by running `make`:

- `build/` - a directory created by [node-gyp](#node-gyp) to hold a variety of autogenerated Makefiles, gyp files, and binary outputs.
- `build/Release/` - directory created to hold binary files for a `Release` build. A `Release` build is the default build when you run `make`. For more info on release builds see [this definition](https://github.com/mapbox/cpp/blob/master/glossary.md#release-build)
- `build/Release/module.node` - The C++ binary module, for a `Release build, in the form of a [loadable module](https://github.com/mapbox/cpp/blob/master/glossary.md#loadable-module). This file was ultimately created by the [linker](#linker) and ended up at this path thanks to [node-gyp](#node-gyp).
- `lib/binding/module.node` - the final resting place for the C++ binary module. Copied to this location from either `build/Release/module.node` or `build/Debug/module.node`. This location is configured via the `module_path` variable in the [package.json](./package.json) and is used by [node-pre-gyp](#node-pre-gyp) when assembling a package to publish remotely (to allow users to install via binaries).
- `build/Release/obj.target/module/src/module.o`: the [object file](https://github.com/mapbox/cpp/blob/master/glossary.md#object-file) that corresponds to the [`src/module.cpp`](./src/module.cpp).
- `build/Release/obj.target/module/standalone/hello.o`: the [object file](https://github.com/mapbox/cpp/blob/master/glossary.md#object-file) that corresponds to the [`src/standalone/hello.cpp`](./src/standalone/hello.cpp).
- `build/Debug/` - directory created to hold binary files for a `Debug` build. A `Debug` build is trigged when you run `make debug`. For more info on debug builds see [this definition](https://github.com/mapbox/cpp/blob/master/glossary.md#debug-build)
- `build/Debug/module.node` - The C++ binary module, for a `Debug build, in the form of a [loadable module](https://github.com/mapbox/cpp/blob/master/glossary.md#loadable-module). This file was ultimately created by the [linker](#linker) and ended up at this path thanks to [node-gyp](#node-gyp).


#### Build flow

#### Overall flow

The overall flow in terms of software components is:

```
make -> node-pre-gyp -> node-gyp -> gyp -> make -> compiler/linker
```

The overall flow, including operations, is:

```
Developer (you) runs 'make'
-> make reads Makefile
 -> Makefile has custom line that calls out to 'node-pre-gyp configure build'
  -> node-pre-gyp passes variables in the package.json along to 'node-gyp rebuild'
   -> node-gyp finds the `binding.gyp` and passes it to gyp
    -> gyp loads the `binding.gyp` and the `common.gypi` and generates Makefiles inside build/
     -> node-gyp runs make in the `build/` directory
      -> make invokes the compiler and linker
       -> compiler outputs object files in build/
        -> linker outputs the loadable module in build/
         -> make copies the 'module.node` from build/Release to `lib/binding`
```

Then the module is ready to use. What happens when it is used is:

```
User of your module runs 'npm install'
-> package.json for their module lists your module (e.g. module based on node-cpp-skel)
 -> npm fetches your module
  -> npm notices an 'install' target that calls out to node-pre-gyp
   -> node-pre-gyp downloads the C++ binary from remote url (as specified in the node-pre-gyp config in the package.json)
    -> node-pre-gyp places the C++ binary at the 'module_path' ('./lib/binding/module.node')
     -> the index.js reads './lib/binding/module.node'
```

This binary file ('./lib/binding/module.node') is what `require()` points to within Node.js.

# Publishing Binaries

It's a good idea to publish pre-built binaries of your module if you want others to be able to easily install it on their system without needing to install a compiler like g++ or clang++. Node-pre-gyp does a lot of the heavy lifting for us (like detecting which system you are building on and deploying to s3) but you'll need a few things configured to get started.

#### 1) In the `package.json`, update the `"binary"` field to the appropriate s3 bucket `host`.

For Mapbox staff we recommend using a host setting of `"host": "https://mapbox-node-binary.s3.amazonaws.com",` which will publish to `s3://mapbox-node-binary/<your module name>`.

Note: for namespaced modules the path will end up being `s3://mapbox-node-binary/@org/<your module name>`.

#### 2) Copy the ci.template.js

Copy the `ci.template.js` from this repo into your repo and place it at  `./cloudformation/ci.template.js`

#### 3) Install deps for validating and managing cloudformation templates

```bash
npm install -g @mapbox/cfn-config # deploying stacks
npm install -g @mapbox/cloudfriend # validating and building templates
```

#### 4) Create a user with permissions to upload to `s3://<bucket name>/<your module name>/`

First configure your AWS creds. You will need to set at least the `AWS_ACCESS_KEY_ID` and `AWS_SECRET_ACCESS_KEY` keys. And also `AWS_SESSION_TOKEN` if you are using 2-factor auth.

Then run:

```bash
validate-template cloudformation/ci.template.js
build-template cloudformation/ci.template.js > cloudformation/ci.template
```

Next we will actually create the user. But first let's discuss what happens here. In addition to creating the user we also write details about the user to a separate bucket (for east auditing purposes). In the below command we:

  - Create a `ci-binary-publish` user
  - Using the `cloudformation/ci.template`
  - And ask to save our stack configuration to the `cfn-config` bucket. If you are outside of Mapbox, see [you will need to pass a different bucket and also a `--template-bucket` option](https://github.com/mapbox/cfn-config#prerequisites)

Now, run the command to create the user:

```
cfn-config create ci-binary-publish cloudformation/ci.template -c cfn-configs
```

It will prompt you, choose:

  - New configuration
  - Ready to create the stack? Y

It will fail if the stack already exists. In this case you can recreate a new user by deleting the stack by running `./node_modules/.bin/cfn-config delete ci-binary-publish cloudformation/ci.template` and then creating a new one.

#### 5) Get the user keys

After the create step succeeds you will have a new user. You now need to get get the users `AccessKeyId` and `SecretAccessKey`.

You can do this in two ways: 1) finding the keys through the AWS console, or 2) using cfn-config to show the stack information

**Tokens via cfn-config**

Run the command `cfn-config info ci-binary-publish` and you'll see a JSON output with `AccessKeyId` and `SecretAccessKey`.

**Tokens via the AWS console**

 - Go to https://console.aws.amazon.com/cloudformation/home?region=us-east-1#/stacks?filter=active&tab=outputs
 - Search for `ci-binary-publish`. You should see a stack called `<your module name>-ci-binary-publish`
 - Click the checkbox beside your `<your module name>-ci-binary-publish` stack
 - Click the `Output` tab to access the `AccessKeyId` and `SecretAccessKey` for this new user.

#### 6) Add the keys to the travis

You can do this two ways: 1) add the keys to the travis UI settings, or 2) encode them as secure variables in your `.travis.yml`

**Adding to travis UI settings**

- Go to https://travis-ci.org/<your user or org>/<your module>/settings
- Scroll to the bottom and find the `Environment Variables` section
- Add a variable called `AWS_ACCESS_KEY_ID` and put the value of the `AccessKeyId` in it
- CRITICAL: Choose `OFF` for `Display value in build log` to ensure the variables are not shown in the logs
- Click `Add`
- Add a variable called `AWS_SECRET_ACCESS_KEY` and put the value of the `SecretAccessKey` in it
- CRITICAL: Choose `OFF` for `Display value in build log` to ensure the variables are not shown in the logs
- Click `Add`

**Encoding keys in yml**

Take the above `AccessKeyId` and `SecretAccessKey` variables and encode them into your `.travis.yml`. You will need to place these in your environment.

Then run:

```bash
# https://github.com/mapbox/node-pre-gyp/#2-create-secure-variables
travis encrypt node_pre_gyp_accessKeyId=${AccessKeyId}
travis encrypt node_pre_gyp_secretAccessKey=${SecretAccessKey}
```

Those will dump text with `secure: <some string>` you can then copy and paste into your .travis.yml like:

```yml
env:
  global:
    - secure: <string encoding aws key>
    - secure: <string encoding aws secret>
```

The strings can be quoted but do not need to be quoted.

Once set, these values will be propagated to the build environment in a secure way. If you look at your travis logs you see:

```bash
# Setting environment variables from .travis.yml
$ export node_pre_gyp_accessKeyId=[secure]
$ export node_pre_gyp_secretAccessKey=[secure]
```

They are printed in the order listed in the `global:` section, which allows you to know what each `- secure: <string>` represents.

Note: you can also pass the `--add` flag to `travis encrypt`. This will add the keys as secure variables to your `.travis.yml` automatically. However this is often not desirable since it will also reformat your `.travis.yml` indentation, hence we why recommend the manual copy/paste method.

#### 7) All done!

Now that you have generated keys for a user that can publish to s3 and provided these keys to travis in a secure way, you should be able to publish binaries. But this should be done in an automated way. See the next section below for how to do that with travis.ci.

**Publishing on Travis CI**

This project includes a `script/publish.sh` command that builds binaries and publishes them to s3. This script checks your commit message for either `[publish binary]` or `[republish binary]` in order to begin publishing. This allows you to publish binaries according to the version specified in your `package.json` like this:

```
git commit -m 'releasing 0.1.0 [publish binary]'
```

Republishing a binary overrides the current version and must be specified with `[republish binary]`.

**Adding new operating systems and node versions**

The `.travis.yml` file uses the `matrix` to set up each individual job, which specifies the operating system, node version, and other environment variables for running the scripts. To add more operating systems and node versions to the binaries you publish, add another job to the matrix like this:

```yaml
- os: {operating system}
  env: NODE="{your node version}" TARGET="Release"
  install: *setup
  script: *test
  after_script: *publish
```
