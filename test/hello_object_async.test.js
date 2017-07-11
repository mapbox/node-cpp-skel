var test = require('tape');
var module = require('../lib/index.js');

test('success: prints expected string with default name', function(t) {
  var H = new module.HelloObjectAsync();
  H.helloAsync({ louder: false }, function(err, result) {
    if (err) throw err;
    t.equal(result, '...threads are busy async bees...hello world');
    t.end();
  });
});

test('success: prints expected string via custom constructor', function(t) {
  var H = new module.HelloObjectAsync('carol');
  H.helloAsync({ louder: false }, function(err, result) {
    if (err) throw err;
    t.equal(result, '...threads are busy async bees...hello carol');
    t.end();
  });
});

test('success: prints loud busy world', function(t) {
  var H = new module.HelloObjectAsync();
  H.helloAsync({ louder: true }, function(err, result) {
    if (err) throw err;
    t.equal(result, '...threads are busy async bees...hello world!!!!');
    t.end();
  });
});

test('error: throws when missing "new"', function(t) {
  try {
    var H = module.HelloObjectAsync();
  } catch(err) {
    t.ok(err);
    t.equal(err.message, 'Cannot call constructor as function, you need to use \'new\' keyword', 'expected error message')
    t.end();
  }
});

test('fail: handles invalid louder value', function(t) {
  var H = new module.HelloObjectAsync();
  H.helloAsync({ louder: 'oops' }, function(err, result) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('option \'louder\' must be a boolean') > -1, 'expected error message');
    t.end();
  });
});

test('fail: handles invalid options value', function(t) {
  var H = new module.HelloObjectAsync();
  H.helloAsync('oops', function(err, result) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('first arg \'options\' must be an object') > -1, 'expected error message');
    t.end();
  });
});

test('fail: handles missing callback', function(t) {
  var H = new module.HelloObjectAsync();
  try {
    H.helloAsync({ louder: false}, {});
  } catch (err) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('second arg \'callback\' must be a function') > -1, 'expected error message');
    t.end();
  }
});