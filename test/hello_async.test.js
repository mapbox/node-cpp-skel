var test = require('tape');
var module = require('../lib/index.js');

test('success: prints loud busy world', function(t) {
  module.helloAsync({ louder: true }, function(err, result) {
    if (err) throw err;
    t.equal(result, '...threads are busy async bees...hello world!!!!');
    t.end();
  });
});

test('success: prints regular busy world', function(t) {
  module.helloAsync({ louder: false }, function(err, result) {
    if (err) throw err;
    t.equal(result, '...threads are busy async bees...hello world');
    t.end();
  });
});

test('error: handles invalid louder value', function(t) {
  module.helloAsync({ louder: 'oops' }, function(err, result) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('option \'louder\' must be a boolean') > -1, 'expected error message');
    t.end();
  });
});

test('error: handles invalid options value', function(t) {
  module.helloAsync('oops', function(err, result) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('first arg \'options\' must be an object') > -1, 'expected error message');
    t.end();
  });
});

test('error: handles missing callback', function(t) {
  try {
    module.helloAsync({ louder: 'oops' }, {});
  } catch (err) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('second arg \'callback\' must be a function') > -1, 'expected error message');
    t.end();
  }
});