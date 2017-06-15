var test = require('tape');
var module = require('../lib/index.js');

test('async: prints loud busy world', function(t) {
  module.hello_async({ louder: true }, function(err, result) {
    if (err) throw err;
    t.equal(result, '...threads are busy bees...world!!!!');
    t.end();
  });
});

test('async: prints regular busy world', function(t) {
  module.hello_async({ louder: false }, function(err, result) {
    if (err) throw err;
    t.equal(result, '...threads are busy bees...world');
    t.end();
  });
});

test('async: prints regular busy world', function(t) {
  module.hello_async({ louder: 'oops' }, function(err, result) {
    t.ok(err, 'expected error');
    console.log(err);
    t.ok(err.message.indexOf('option \'louder\' must be a boolean') > -1, 'expected error message');
    t.end();
  });
});