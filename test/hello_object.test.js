var test = require('tape');
var module = require('../lib/index.js');

test('success: prints expected string', function(t) {
  var H = new module.HelloObject('carol');
  var check = H.hello();
  t.equal(check, '...initialized an object...hello carol', 'returned expected string');
  t.end();
});

test('error: throws when passing empty string', function(t) {
  try {
    var H = new module.HelloObject('');
  } catch(err) {
    t.ok(err, 'expected error');
    t.equal(err.message, 'arg must be a non-empty string', 'expected error message')
    t.end();
  }
});

test('error: throws when missing "new"', function(t) {
  try {
    var H = module.HelloObject();
  } catch(err) {
    t.ok(err);
    t.equal(err.message, 'Cannot call constructor as function, you need to use \'new\' keyword', 'expected error message')
    t.end();
  }
});

test('error: handles non-string arg within constructor', function(t) {
  try {
    var H = new module.HelloObject(24);
  } catch(err) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('arg must be a string') > -1, 'expected error message');
    t.end();
  }
});

test('error: handles missing arg', function(t) {
  try {
    var H = new module.HelloObject();
  } catch (err) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('must provide string arg') > -1, 'expected error message');
    t.end();
  }
});