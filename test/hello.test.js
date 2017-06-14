var test = require('tape');
var module = require('../lib/index.js');

test('prints world', function(t) {
  console.log(module.hello());
  t.end();
});

