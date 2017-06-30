var test = require('tape');
var module = require('../lib/index.js');

test('success: prints expected string', function(t) {
  var H = new module.HelloObject();
  var check = H.hello();
  t.equal(check, '...initialized an object...hello world', 'returned expected string');
  t.end();
});

test('success: prints expected string', function(t) {
  var H = new module.HelloObject('carol');
  var check = H.hello();
  t.equal(check, '...initialized an object...hello carol', 'returned expected string');
  t.end();
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