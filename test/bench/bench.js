var fs = require('fs');
var path = require('path');
var HelloWorld = require('../../lib/index.js');

console.time('constructor');
var HW = new HelloWorld();
console.timeEnd('constructor');

console.time('shout');
HW.shout('rawr', {}, function(err, result) {
  if (err) throw err;
  console.timeEnd('shout');
});

// How to benchmark sync functions...?
// If the event loop is running and HW.wave() could potentially return before the end timer triggers
// console.time('wave');
// var hello = HW.wave();
// console.timeEnd('wave');