var test = require('tape');
var module = require('../lib/index.js');

test('prints world', function(t) {
  var check = module.hello();
  t.equal(check, 'world', 'returned world');
  t.end();
});

