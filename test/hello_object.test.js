var test = require('tape');
var module = require('../lib/index.js');

test('success: prints expected string', function(t) {
  var H = new module.HelloObject();
  var check = H.hello();
  t.equal(check, '...initialized an object...world', 'returned expected string');
  t.end();
});