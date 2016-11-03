"use strict";

var argv = require('minimist')(process.argv.slice(2));
if (!argv.iterations || !argv.concurrency) {
  process.stdout.write('Please provide desired iterations and concurrency\n');
  process.stdout.write('Example: \nbench/bench-batch.js --iterations 50 --concurrency 10\n');
  process.exit(1);
}

var concurrency = argv.concurrency;

// This env var sets the libuv threadpool size.
// This value is locked in once a function interacts with the threadpool
// Therefore we need to set this value either in the shell or at the very
// top of a JS file (like we do here)
process.env.UV_THREADPOOL_SIZE = concurrency;

var fs = require('fs');
var path = require('path');
var argv = require('minimist')(process.argv.slice(2));
var assert = require('assert')
var d3_queue = require('d3-queue');

var HelloWorld = require('../../lib/index.js');
var HW = new HelloWorld();
var queue = d3_queue.queue(concurrency);
var iterations = argv.iterations;
var runs = 0;

var HW = new HelloWorld();

var options = {};

if (argv.sleep) options.sleep = argv.sleep;

function run(cb) {
    HW.shout('rawr', options, function(err, result) {
      if (err) {
        return cb(err);
      }
      ++runs;
      return cb();
    });
}

for (var i = 1; i <= iterations; i++) {
    queue.defer(run);
}

var time = +(new Date());

queue.awaitAll(function(error) {
  if (error) throw error;
  if (runs != iterations) {
    throw new Error("Error: did not run as expected");
  }
  // check rate
  time = +(new Date()) - time;

  // number of milliseconds per iteration
  var rate = runs/(time/1000);

  console.log('speed: ' + rate.toFixed(0) + ' runs/s (runs:' + runs + ' ms:' + time + ' )');
  
  // There may be instances when you want to assert some performance metric
  //assert.equal(rate < 1, true, 'avg time per iteration ( ' + rate + 'ms ) takes less than 1 ms');

});