var test = require('tape');
var module = require('../lib/index.js');

test('success: prints expected string', function(t) {
  var H = new module.HelloObject('carol');
  var check = H.helloMethod();
  t.equal(check, 'carol', 'returned expected string');
  t.end();
});

test('error: throws when passing empty string', function(t) {
  try {
    var H = new module.HelloObject('');
  } catch(err) {
    t.ok(err, 'expected error');
    t.equal(err.message, 'arg must be a non-empty string', 'expected error message');
    t.end();
  }
});

test('error: throws when missing "new"', function(t) {
  try {
    var H = module.HelloObject();
  } catch(err) {
    t.ok(err);
    t.equal(err.message, 'Class constructors cannot be invoked without \'new\'', 'expected error message');
    t.end();
  }
});

test('error: handles non-string arg within constructor', function(t) {
  try {
    var H = new module.HelloObject(24);
  } catch(err) {
    t.ok(err, 'expected error');
    t.ok(err.message, 'A string was expected', 'expected error message');
    t.end();
  }
});

test('error: handles missing arg', function(t) {
  try {
    var H = new module.HelloObject();
  } catch (err) {
    t.ok(err, 'expected error');
    t.ok(err.message, 'must provide string arg', 'expected error message');
    t.end();
  }
});
