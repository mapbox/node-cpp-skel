var test = require('tape');
var HelloWorld = require('../lib/index.js');
var HW = new HelloWorld();

test('HellowWorld error - throw exception during construction', function(t) {
  var never = '';
  try {
    var HWuhoh = new HelloWorld('uhoh');
    never = 'neverland';
  } catch (err) {
      t.ok(err, 'expected error');
  }
  t.equals(never, '', 'constructor definitely throws');
  t.end();
});

test('HellowWorld error - throw type error during construction', function(t) {
  var never = '';
  try {
    var HWuhoh = new HelloWorld(24);
    never = 'neverland';
  } catch (err) {
      t.ok(err, 'expected error');
      t.equals(err.message, 'arg must be a string')
  }
  t.equals(never, '', 'constructor definitely throws');
  t.end();
});

test('HellowWorld error - invalid constructor', function(t) {
  var never = '';
  try {
    var HWuhoh = HelloWorld();
    never = 'neverland';
  } catch (err) {
      t.ok(err, 'expected error');
      t.equals(err.message, "Cannot call constructor as function, you need to use 'new' keyword");
  }
  t.equals(never, '', 'constructor definitely throws');
  t.end();
});

test('HellowWorld success - valid constructor', function(t) {
  var never = '';
  try {
    var HWyay = new HelloWorld('hello');
    never = 'neverland';
  } catch (err) {
      if (err) throw err;
  }
  t.equals(never, 'neverland', 'constructor definitely succeeds');
  t.end();
});


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


test('shout error - not enough rawr', function(t) {
  HW.shout('tiny moo', { louder: true }, function(err, shout) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('rawr all the time') > -1, 'rawrs all the time are way nicer');
    t.end();
  });
});

test('shout error - non string phrase', function(t) {
  HW.shout(4, {}, function(err, shout) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('phrase') > -1, 'proper error message');
    t.end();
  });
});

test('shout error - no options object', function(t) {
  HW.shout('rawr', true, function(err, shout) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('options') > -1, 'proper error message');
    t.end();
  });
});

test('shout error - options.louder non boolean', function(t) {
  HW.shout('rawr', { louder: 3 }, function(err, shout) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('louder') > -1, 'proper error message');
    t.end();
  });
});

// we have to try/catch since a missing callback results in a throw
test('shout error - no callback', function(t) {
  try {
    HW.shout('rawr', {});
  } catch (err) {
    t.ok(err, 'expected error');
    t.ok(err.message.indexOf('callback') > -1, 'proper error message');
    t.end();
  }
});