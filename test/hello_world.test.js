var test = require('tape');
var HelloWorld = require('../lib/index.js');
var HW = new HelloWorld();

test('wave success', function(t) {
  var hello = HW.wave();
  t.equal(hello, 'howdy world', 'output of HelloWorld.wave');
  t.end();
});

test('shout success', function(t) {
  HW.shout('rawr', {}, function(err, shout) {
    if (err) throw err;
    t.equal(shout, 'rawr!');
    t.end();
  });
});

test('shout success - options.louder', function(t) {
  HW.shout('rawr', { louder: true }, function(err, shout) {
    if (err) throw err;
    t.equal(shout, 'rawr!!!!!');
    t.end();
  });
});

// we have to wrap these in try/catch statements right now
// https://github.com/mapbox/node-cpp-skel/issues/4
test('shout error - non string phrase', function(t) {
  try {
    HW.shout(4, {}, function(err, shout) {});
  } catch (err) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('phrase') > -1, 'proper error message');
    t.end();
  }
});

test('shout error - no options object', function(t) {
  try {
    HW.shout('rawr', true, function(err, shout) {});
  } catch (err) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('options') > -1, 'proper error message');
    t.end();
  }
});

test('shout error - options.louder non boolean', function(t) {
  try {
    HW.shout('rawr', { louder: 3 }, function(err, shout) {});
  } catch (err) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('louder') > -1, 'proper error message');
    t.end();
  }
});

test('shout error - no callback', function(t) {
  try {
    HW.shout('rawr', {});
  } catch (err) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('callback') > -1, 'proper error message');
    t.end();
  }
});