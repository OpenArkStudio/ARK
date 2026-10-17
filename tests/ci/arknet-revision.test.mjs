import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import test from 'node:test';

// Inspect this repository's literal CMake cache default and database job steps;
// this is not a general CMake/YAML parser and never reads a user's CMake cache.
function matchingRevision(cmake, workflow) {
  const source = cmake.replaceAll('\r\n', '\n');
  const declarations = source.match(/^[ \t]*set\([ \t]*ARKNET_REVISION\b[^\n]*/gm) ?? [];
  assert.equal(declarations.length, 1, 'expected one ARKNET_REVISION cache default');
  const definition = declarations[0].match(/^\s*set\(\s*ARKNET_REVISION\s+(?:"([^"\n]*)"|([^\s()]+))\s+CACHE\s+STRING\s+"[^"\n]*"\s*\)\s*(?:#.*)?$/);
  assert.ok(definition, 'ARKNET_REVISION must remain a literal CACHE STRING default without FORCE');
  const revision = definition[1] ?? definition[2];

  const jobs = workflow.replaceAll('\r\n', '\n').split(/(?=^  [a-z][a-z0-9_-]*:$)/m);
  const database = jobs.filter(block => block.startsWith('  database:\n'));
  assert.equal(database.length, 1, 'expected one database CI job');
  const steps = database[0].split(/(?=^      - )/m).slice(1);
  const checkouts = steps.filter(step => /^          repository:[ \t]*(?:OpenArkStudio\/arknet|"OpenArkStudio\/arknet"|'OpenArkStudio\/arknet')[ \t]*(?:#.*)?$/m.test(step));
  assert.equal(checkouts.length, 1, 'expected one explicit arknet checkout in database CI');
  const checkout = checkouts[0];
  assert.match(checkout, /^(?: {6}- | {8})uses:[ \t]+['"]?actions\/checkout@[^\s'"]+['"]?[ \t]*(?:#.*)?$/m);
  function scalar(name) {
    const fields = [...checkout.matchAll(new RegExp(`^          ${name}:[ \\t]*(.*)$`, 'gm'))];
    assert.equal(fields.length, 1, `expected one ${name} field in the arknet checkout`);
    const value = fields[0][1].trim().match(/^(?:"([^"]*)"|'([^']*)'|([^#\s]+))(?:[ \t]+#.*)?$/);
    assert.ok(value, `${name} must be one explicit scalar`);
    return value[1] ?? value[2] ?? value[3];
  }
  assert.equal(scalar('repository'), 'OpenArkStudio/arknet');
  const ref = scalar('ref');
  for (const [name, value] of [['CMake default', revision], ['database CI ref', ref]]) {
    assert.match(value, /^[a-f0-9]{40}$/i, `${name} must use an immutable full 40-character commit SHA`);
    assert.notEqual(value, '0'.repeat(40), `${name} cannot use the null object ID`);
  }
  assert.equal(revision.toLowerCase(), ref.toLowerCase(), 'CMake and database CI must use the same arknet commit');
  return revision.toLowerCase();
}

const first = 'a'.repeat(40);
const second = 'b'.repeat(40);
const cmakeDefault = (value = first) => `set(ARKNET_REVISION "${value}" CACHE STRING "Pinned arknet revision")\n`;
const checkoutStep = (value = first) => `      - name: Check out pinned arknet
        uses: actions/checkout@v4
        with:
          repository: OpenArkStudio/arknet
          ref: ${value}
          path: ci-arknet
`;
const databaseJob = (steps = checkoutStep()) => `jobs:\n  database:\n    steps:\n${steps}`;

test('the repository CMake default and database checkout use the same immutable arknet commit', () => {
  const cmake = readFileSync(new URL('../../CMakeLists.txt', import.meta.url), 'utf8');
  const workflow = readFileSync(new URL('../../.github/workflows/ci.yml', import.meta.url), 'utf8');
  matchingRevision(cmake, workflow);
});

test('literal quoting, comments, CRLF and unrelated job checkouts do not change pin selection', () => {
  const cmake = `# set(ARKNET_REVISION main)\r\n${cmakeDefault().replace('\n', ' # repository default\r\n')}`;
  const checkout = checkoutStep(`'${first}' # full commit`);
  const workflow = `jobs:\n  unrelated:\n    steps:\n${checkoutStep(second)}  database:\n    steps:\n${checkout}`.replaceAll('\n', '\r\n');
  assert.equal(matchingRevision(cmake, workflow), first);
});

test('a valid but different database revision fails instead of testing another library commit', () => {
  assert.throws(() => matchingRevision(cmakeDefault(), databaseJob(checkoutStep(second))), /same arknet commit/);
});

test('mutable, abbreviated, malformed, dynamic and null revisions fail on either side', () => {
  for (const value of ['main', 'v0.2.2', 'a'.repeat(39), 'g'.repeat(40), '0'.repeat(40), '${ARKNET_REVISION}']) {
    assert.throws(() => matchingRevision(cmakeDefault(value), databaseJob()), value);
    assert.throws(() => matchingRevision(cmakeDefault(), databaseJob(checkoutStep(value))), value);
  }
});

test('missing, duplicate or forced CMake defaults cannot silently select a pin', () => {
  for (const cmake of ['', `${cmakeDefault()}${cmakeDefault(second)}`, cmakeDefault().replace(')', ' FORCE)')]) {
    assert.throws(() => matchingRevision(cmake, databaseJob()));
  }
});

test('missing or ambiguous database checkout fields fail even with an unrelated valid pin', () => {
  const checkout = checkoutStep();
  for (const workflow of ['', 'jobs:\n  database:\n    steps:\n',
    databaseJob(checkout + checkoutStep()), databaseJob(checkout.replace(/          ref:.*\n/, '')),
    databaseJob(checkout.replace(`          ref: ${first}`, `          ref: ${first}\n          ref: ${second}`)),
    databaseJob(checkout.replace('repository: OpenArkStudio/arknet', 'repository: example/other')),
    databaseJob(checkout.replace('uses: actions/checkout@v4', 'uses: example/other@v1'))]) {
    assert.throws(() => matchingRevision(cmakeDefault(), workflow));
  }
});

test('the repository policy does not treat documented development or cache overrides as defaults', () => {
  const cmake = [cmakeDefault(), 'set(ARKNET_SOURCE_DIR "" CACHE PATH "Local development checkout")',
    `# A developer may configure -DARKNET_REVISION=${second} or -DARKNET_SOURCE_DIR=/local/arknet.`,
    'if(ARKNET_SOURCE_DIR)', '  add_subdirectory("${ARKNET_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/arknet")',
    'endif()', ''].join('\n');
  assert.equal(matchingRevision(cmake, databaseJob()), first);
});
