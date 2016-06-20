# HelloWorld

Main class, called HelloWorld

**Examples**

```javascript
var HelloWorld = require('index.js');
var HW = new HelloWorld();
```

## wave

Say howdy to the world

**Examples**

```javascript
var wave = HW.wave();
console.log(wave); // => 'howdy world!'
```

Returns **[String](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String)** a happy-go-lucky string saying hi

## shout

Shout a phrase really loudly by adding an exclamation to the end, asynchronously

**Parameters**

-   `phrase` **[String](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String)** to shout
-   `different` **[Object](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object)** ways to shout
-   `callback` **[Function](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/function)** from whence the shout comes, returns a string

**Examples**

```javascript
var HW = new HelloWorld();
HW.shout('rawr', {}, function(err, shout) {
  if (err) throw err;
  console.log(shout); // => 'rawr!'
});
```
