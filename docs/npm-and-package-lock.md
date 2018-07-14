## Using npm, package.json, and package-lock.json

With updated versions of npm, there is now such thing as a `package-lock.json` file, which is now included in node-cpp-skel. Read this doc for more info, also discussion at https://github.com/mapbox/node-cpp-skel/issues/124.

`package.json` defines the dependencies required by any given library. It often defines a range of acceptable version of those dependencies, for example:

```
"dependencies": {
    "eslint": "^4.3.0"
}
```

... means that any version of eslint >4.3.0 and <5 is acceptable.

`package-lock.json` defines _explicit versions of dependencies that should be used_. This is useful, because by using `package-lock.json` you can be confident that the version of your dependencies deployed in production matches exactly with the versions you install in your local testing environment.

Running `npm install` in a folder that contains a `package.json` file will create a `package-lock.json` file if one does not exist. **You should commit this file**.

If a `package-lock.json` file exists in the repository, but some of the versions of dependencies are out-of-sync with what's specified in `package.json` then running `npm install` will update the `package-lock.json` file. This may happen if you manually edited a version of a dependency in `package.json`, which you should not do (see below).

If you are creating a PR, you should notice if that PR contains changes to `package-lock.json`, and you should be able to explain in the PR why those changes are being made.

To take full advantage of the explicit dependency versions specified in `package-lock.json`, don't use `npm install`. Instead, run

```
npm ci
```

Note: this `ci` requires at least npm >= v5.7.1 (https://blog.npmjs.org/post/171556855892/introducing-npm-ci-for-faster-more-reliable).

This will remove your existing `node_modules` folder, and then install the exact versions defined by `package-lock.json`. It will **never** make changes to `package.json` or `package-lock.json`, unlike `npm install`.

### Avoid editing package.json directly

If you need to change the range of acceptable versions for a dependency:

```
npm install --save eslint@^4.4.8
```

If you want to upgrade a dependency to the most recent version specified by the existing range in `package.json`;

```
npm update eslint
```

If you want to remove a dependency:

```
npm remove eslint
```
