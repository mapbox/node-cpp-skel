'use strict';

const test = require('tape');
const { helloPromise } = require('../lib/index.js');

test('success: no options object', async (assert) => {
  const result = await helloPromise();
  assert.equal(result, 'hello');
  assert.end();
});

test('success: empty options object', async (assert) => {
  const result = await helloPromise({});
  assert.equal(result, 'hello');
  assert.end();
});

test('success: options.phrase', async (assert) => {
  const result = await helloPromise({ phrase: 'Waka' });
  assert.equal(result, 'Waka');
  assert.end();
});

test('success: options.multiply', async (assert) => {
  const result = await helloPromise({ multiply: 4 });
  assert.equal(result, 'hellohellohellohello');
  assert.end();
});

test('success: options.phrase and options.multiply', async (assert) => {
  const result = await helloPromise({ phrase: 'Waka', multiply: 5 });
  assert.equal(result, 'WakaWakaWakaWakaWaka');
  assert.end();
});

test('error: invalid options type', async (assert) => {
  try {
    await helloPromise('not an object');
    assert.fail();
  } catch (err) {
    assert.ok(err);
    assert.equal(err.message, 'options must be an object');
  }
  assert.end();
});

test('error: invalid options.phrase', async (assert) => {
  try {
    await helloPromise({ phrase: 10 });
    assert.fail();
  } catch (err) {
    assert.ok(err);
    assert.equal(err.message, 'options.phrase must be a string');
  }
  assert.end();
});

test('error: invalid options.multiply', async (assert) => {
  try {
    await helloPromise({ multiply: 'not a number' });
    assert.fail();
  } catch (err) {
    assert.ok(err);
    assert.equal(err.message, 'options.multiply must be a number');
  }
  assert.end();
});