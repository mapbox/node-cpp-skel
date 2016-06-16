A skeleton for bindings to C++ libraries for Node.js us [Nan](https://github.com/nodejs/nan). This is a small, hypothetical library called Hello World. It has a `HelloWorld` class that can be constructed with a single method called `wave`.

More examples: https://nodejs.org/api/addons.html

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
