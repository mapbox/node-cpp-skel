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

It's a good idea to publish pre-built binaries of your module if you want others to be able to easily install it on their system without needing to install a compiler like g++ or clang++. Node-pre-gyp does a lot of the heavy lifting for us (like detecting which system you are building on and deploying to s3) but you'll need a few things configured to get started.

1) In the `package.json`, update the `"binary"` field to the appropriate s3 bucket `host`.

For Mapbox staff we recommend using a host setting of `"host": "https://mapbox-node-binary.s3.amazonaws.com",` which will publish to `s3://mapbox-node-binary/<your module name>`.

Note: for namespaced modules the path will end up being `s3://mapbox-node-binary/@org/<your module name>`.

2) Copy the ci.template.js

Copy the `ci.template.js` from this repo into your repo and place it at  `./cloudformation/ci.template.js`

3) Install deps

```
npm install @mapbox/cfn-config
npm install @mapbox/cloudfriend
```

4) Create a user with permissions to upload to `s3://mapbox-node-binary/<your module name>/`

First configure your AWS creds. You will need to set at least the `AWS_ACCESS_KEY_ID` and `AWS_SECRET_ACCESS_KEY` keys. And also `AWS_SESSION_TOKEN` if you are using 2-factor auth.

Then run:

```bash
./node_modules/.bin/validate-template cloudformation/ci.template.js
./node_modules/.bin/build-template cloudformation/ci.template.js > cloudformation/ci.template
```

Next we will actually create the user. But first let's discuss what happens here. In addition to creating the user we also write details about the user to a separate bucket (for east auditing purposes). In the below command we:

  - Create a `ci-binary-publish` user
  - Using the `cloudformation/ci.template`
  - And ask to save our stack configuration to the `cfn-config` bucket. If you are outside of Mapbox, see [you will need to pass a different bucket and also a `--template-bucket` option](https://github.com/mapbox/cfn-config#prerequisites)

Now, run the command to create the user:

```
./node_modules/.bin/cfn-config create ci-binary-publish cloudformation/ci.template -c cfn-configs
```

It will prompt you, choose:

  - New configuration
  - Ready to create the stack? Y

It will fail if the stack already exists. In this case you can recreate a new user by deleting the stack by running `./node_modules/.bin/cfn-config delete ci-binary-publish cloudformation/ci.template` and then creating a new one.

5) Get the user keys

After the create step succeeds you will have a new user. You now need to get get the users `AccessKeyId` and `SecretAccessKey`.

You can do this by:

 - Go to https://console.aws.amazon.com/cloudformation/home?region=us-east-1#/stacks?filter=active&tab=outputs
 - Search for `ci-binary-publish`. You should see a stack called `<your module name>-ci-binary-publish`
 - Click the checkbox beside your `<your module name>-ci-binary-publish` stack
 - Click the `Output` tab to access the `AccessKeyId` and `SecretAccessKey` for this new user.

6) Add the keys to the travis

You can do this two ways: 1) add the keys to the travis UI settings, or 2) encode them as secure variables in your `.travis.yml`

#### Adding to travis UI settings

- Go to https://travis-ci.org/<your user or org>/<your module>/settings
- Scroll to the bottom and find the `Environment Variables` section
- Add a variable called `AWS_ACCESS_KEY_ID` and put the value of the `AccessKeyId` in it
- CRITICAL: Choose `OFF` for `Display value in build log` to ensure the variables are now shown in the logs
- Click `Add`
- Add a variable called `AWS_SECRET_ACCESS_KEY` and put the value of the `SecretAccessKey` in it
- CRITICAL: Choose `OFF` for `Display value in build log` to ensure the variables are now shown in the logs
- Click `Add`

#### Encoding keys in yml

Take the above `AccessKeyId` and `SecretAccessKey` variables and encode them into your `.travis.yml`. You will need to place these in your environment.

Then run:

```
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
``

The strings can be quoted but do not need to be quoted.

Once set, these values will be propagated to the build environment in a secure way. If you look at your travis logs you see:

```
Setting environment variables from .travis.yml
$ export node_pre_gyp_accessKeyId=[secure]
$ export node_pre_gyp_secretAccessKey=[secure]
```

They are printed in the order listed in the `global:` section, which allows you to know what each `- secure: <string>` represents.

Note: you can also pass the `--add` flag to `travis encrypt`. This will add the keys as secure variables to your `.travis.yml` automatically. However this is often not desirable since it will also reformat your `.travis.yml` indentation, hence we why recommend the manual copy/paste method.

7) All done!

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
