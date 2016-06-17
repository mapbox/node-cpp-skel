var test = require('tape');
var HelloWorld = require('../lib/index.js');
var HW = new HelloWorld();

test('wave', function(t) {
  var hello = HW.wave();
  t.equal(hello, 'howdy world', 'output of HelloWorld.wave');
  t.end();
});

test('shout', function(t) {
  HW.shout('rawr', {}, function(err, shout) {
    if (err) throw err;
    t.equal(shout, 'rawr!');
    t.end();
  });
});

test('shout loud', function(t) {
  HW.shout('rawr', { louder: true }, function(err, shout) {
    if (err) throw err;
    t.equal(shout, 'rawr!!!!');
    t.end();
  });
});

// test('shout error - non string phrase', function(t) {
//   HW.shout(4, {}, function(err, shout) {
//     t.ok(err);
//     t.end();
//   });
// });

// test('shout error - no options object', function(t) {
//   HW.shout('rawr', true, function(err, shout) {
//     t.ok(err);
//     t.end();
//   });
// });

// test('shout error - no callback', function(t) {
//   t.ok(function() { W.shout('rawr', {}); });
// });