var fs = require('fs');
var path = require('path');
var HelloWorld = require('../../lib/index.js');

console.time('constructor');
var HW = new HelloWorld();
console.timeEnd('constructor');

console.time('shout');
// memory usage for this single process
var mem_before = process.memoryUsage();

HW.shout('rawr', {}, function(err, result) {
  if (err) throw err;
  console.timeEnd('shout');
  
  // heap: a memory segment dedicated to storing explicitly referenced types like objects and strings
  var mem_after = process.memoryUsage();
  var mem_used = (mem_after.heapUsed - mem_before.heapUsed) / 10000;
  console.log('total memory used: ' + mem_used + ' MB');
});

// Going to not worry about benchmarking sync functions for now 
// since the most common usecase of a C++ in a Node module is 
// optimizing processes within the threadpool (async)
