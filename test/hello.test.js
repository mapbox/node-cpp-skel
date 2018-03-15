var test = require('tape');
var module = require('../lib/index.js');

test('prints world', function(t) {
  var check = module.convert();
  t.equal(check, 'hello world', 'returned world');
  t.end();
});

