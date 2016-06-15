var test = require('tape');
var HelloWorld = require('../lib/index.js');

test('wave hi', function(t) {
  var HW = new HelloWorld();
  var hello = HW.wave();
  t.equal(hello, 'howdy world!', 'output of HelloWorld.wave');
  t.end();
})