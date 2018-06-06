# Workflow

So your code is compiling, tested, benched, and ready to be shared. How to get it into the wild? This document will go through what's next.

Generally speaking, the workflow for a node-addon isn't much different than publishing a regular Javascript node module. The main difference is an added step and a bit more configuration in order to handle the binary. If the concept of a binary file is new, give [this doc a gander](https://github.com/mapbox/node-cpp-skel/blob/dbc48924b3e30bba903e6b9220b0cdf2854f717f/docs/extended-tour.md#builds). 

The typical workflow for a regular node module may look something like this:

1. merge to master
2. git tag
3. npm publish (Now it's ready to be npm installed)

The workflow for a node add-on looks very similar:

1. merge to master
2. git tag
3. publish binaries
4. npm publish

Let's talk generally about the relationship between the third and fourth steps. Since your code is in C++, any projects that `npm install` your module as a dependency will need the C++ code precompiled so that Node can use your module in Javascript world. Before publishing your module to npm, you will publish your binaries by putting them on s3 (**Note**: you will likely publish multiple binaries, one for each Node version and various operating systems). This s3 location is reflected in your module's [package.json file](https://github.com/mapbox/node-cpp-skel/blob/dbc48924b3e30bba903e6b9220b0cdf2854f717f/package.json#L35). Your package.json file is also redefining [the install command](https://github.com/mapbox/node-cpp-skel/blob/dbc48924b3e30bba903e6b9220b0cdf2854f717f/package.json#L14), by running node-pre-gyp instead. 

[Node-pre-gyp](https://github.com/mapbox/node-pre-gyp) is responsible for installing the binary by pulling the relevant binary from s3 and placing it in the specified and expected location defined by your module's [main index.js file](https://github.com/mapbox/node-cpp-skel/blob/dbc48924b3e30bba903e6b9220b0cdf2854f717f/lib/index.js#L3). So when a project runs `require()` on your module, they are directly accessing the binary. In a bit more detail, node-pre-gyp will detect [what version of Node is being used and which operating system](https://github.com/mapbox/node-cpp-skel/blob/dbc48924b3e30bba903e6b9220b0cdf2854f717f/package.json#L37), then go to s3 to retrieve the binary that matches.

Continue reading below to learn how to publish your binaries to s3 so they're ready to be installed.


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
 
 **Adding to travis UI settings**
 
 - Go to https://travis-ci.org/<your user or org>/<your module>/settings
 - Scroll to the bottom and find the `Environment Variables` section
 - Add a variable called `AWS_ACCESS_KEY_ID` and put the value of the `AccessKeyId` in it
 - CRITICAL: Choose `OFF` for `Display value in build log` to ensure the variables are not shown in the logs
 - Click `Add`
 - Add a variable called `AWS_SECRET_ACCESS_KEY` and put the value of the `SecretAccessKey` in it
 - CRITICAL: Choose `OFF` for `Display value in build log` to ensure the variables are not shown in the logs
 - Click `Add`

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

### Dev releases

You may want to test your module works correctly, in downstream dependencies, before formally publishing. To do this we recommend you:

1. Create a branch of your node c++ module

2. Modify the `version` string in your `package.json` like:

```diff
diff --git a/package.json b/package.json
index e00b7b5..22f7cd9 100644
--- a/package.json
+++ b/package.json
@@ -1,6 +1,6 @@
 {
   "name": "@mapbox/node-cpp-skel",
-  "version": "0.1.0",
+  "version": "0.1.0-alpha",
   "description": "Skeleton for bindings to C++ libraries for Node.js using NAN",
   "url": "http://github.com/mapbox/node-cpp-skel",
   "main": "./lib/index.js",
```

3. Publishing C++ binaries by pushing a commit with `[publish binary]` per https://github.com/mapbox/node-cpp-skel/blob/master/docs/publishing-binaries.md#7-all-done


4. **Option A)** Require your module in downstream applications like:

```js
"your-module": "https://github.com/<your-org>/<your-module>/tarball/<your-branch>",
```

If you're publishing from a private repo, generate a dev release and then reference the url in the appropriate `package.json` file. For example, `zip` the repo, put to S3, and then reference the S3 url in `package.json`. 

**Option B)** Issue a npm dev release after the binary is published. 

Run `npm publish --tag dev` and then require your module in downstream applications like: 

```js
"your-module": "0.1.0-alpha",
```

For npm dev releases, it’s good to use the `--tag <something>` to avoid publishing to the latest tag. If you run `npm publish` then `0.1.0-alpha` is what anyone running `npm install <your module> --save` will receive. 


#### Before `npm publish`

Before publishing to npm, you can ensure the final packaged tarball will include what you expect. For instance, you want to avoid a large accidental file being packaged by npm and make sure the package contains all needed dependencies.

Take a peek at what npm will publish by running:

```
make
make testpack
```

This will create a tarball locally and print every file included. A couple basic checks:
  - make sure `.mason` is not included
  - make sure `node-pre-gyp` directory is included, because it is responsible for knowing where to grab the binary from s3.

**Hot tips** :hot_pepper: 
- Node and npm versions can have differing `npm pack` and `npm publish` behaviour, so be mindful of what your environment is using
- You can use the resulting tarball locally and install it within another local repo to make sure it works: 
```
npm install <path to tarball>
```
