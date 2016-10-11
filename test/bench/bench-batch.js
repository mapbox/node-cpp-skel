var fs = require('fs');
var path = require('path');
var argv = require('minimist')(process.argv.slice(2));
var tape = require('tape');
var HelloWorld = require('../../lib/index.js');

if (!argv.iterations || !argv.concurrency) {
  process.stdout.write('Please provide desired iterations and concurrency\n');
  process.stdout.write('Example: \nbench/bench-batch.js --iterations 5000 --concurrency 10\n');
  process.exit(1);
}

var opts = { skip: false };

if (process.env.NPM_FLAGS === '--debug' || process.env.COVERAGE === true) {
  console.log('# SKIP benchmark: tests are in debug or coverage mode');
  opts.skip = true;
}

tape('shout batch bench', opts, function(assert) {
    var HW = new HelloWorld();
    var d3_queue = require('d3-queue');

    var iterations = argv.iterations;
    var concurrency = argv.concurrency;
    var queue = d3_queue.queue(concurrency);
    var runs = 0;

    function run(cb) {
        HW.shout('rawr', {}, function(err, result) {
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
      var rate = time/runs;

      assert.equal(rate < 1, true, 'avg time per iteration ( ' + rate + 'ms ) takes less than 1 ms');
      assert.end();
    });
});