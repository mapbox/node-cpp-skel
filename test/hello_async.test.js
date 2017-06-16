var test = require('tape');
var module = require('../lib/index.js');

test('success: prints loud busy world', function(t) {
  module.hello_async({ louder: true }, function(err, result) {
    if (err) throw err;
    t.equal(result, '...threads are busy async bees...world!!!!');
    t.end();
  });
});

test('success: prints regular busy world', function(t) {
  module.hello_async({ louder: false }, function(err, result) {
    if (err) throw err;
    t.equal(result, '...threads are busy async bees...world');
    t.end();
  });
});

test('fail: handles invalid arg value', function(t) {
  module.hello_async({ louder: 'oops' }, function(err, result) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('option \'louder\' must be a boolean') > -1, 'expected error message');
    t.end();
  });
});