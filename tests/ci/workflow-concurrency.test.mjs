import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import test from 'node:test';

const workflow = readFileSync(new URL('../../.github/workflows/ci.yml', import.meta.url), 'utf8').replaceAll('\r\n', '\n');

test('PR, merge-group and manual runs retain the bounded per-PR/ref workflow group', () => {
  const concurrency = workflow.match(/^concurrency:\n((?:  [^\n]+\n)+)/m)?.[1];
  assert.equal(concurrency,
    '  group: ${{ github.workflow }}-${{ github.event.pull_request.number || github.ref }}\n' +
    '  cancel-in-progress: false\n');
  assert.equal([...workflow.matchAll(/^ *concurrency:/gm)].length, 1, 'keep one workflow-level group');
  assert.match(workflow, /^  pull_request:\n    branches: \[main\]$/m);
  assert.match(workflow, /^  merge_group:\n    types: \[checks_requested\]$/m);
  assert.match(workflow, /^  workflow_dispatch:$/m);
});

test('protected admission is validated before merge without repeating CI on a main push', () => {
  assert.doesNotMatch(workflow, /^  push:/m);
  const publishing = readFileSync(new URL('../../.github/workflows/docs.yml', import.meta.url), 'utf8').replaceAll('\r\n', '\n');
  assert.match(publishing, /^  push:\n    branches: \[main\]$/m, 'keep documentation publication after merge');
  for (const name of ['README.md', 'README_CN.md']) {
    const readme = readFileSync(new URL('../../' + name, import.meta.url), 'utf8');
    assert.match(readme, /ci\.yml\/badge\.svg\?event=merge_group/);
    assert.doesNotMatch(readme, /ci\.yml\/badge\.svg\?branch=main/);
  }
});

test('the required gate still runs after failed needs and invokes the fatal result validator', () => {
  const required = workflow.split(/(?=^  [a-z][a-z0-9_-]*:$)/m)
    .find(block => block.startsWith('  required:\n'));
  assert.ok(required, 'retain the required gate');
  assert.match(required, /^    name: CI - required$/m);
  assert.match(required, /^    if: always\(\)$/m);
  assert.match(required, /^    needs: \[scope, network, applications, database, formatting, documentation, readme\]$/m);
  assert.match(required, /^          CI_NEEDS_JSON: \$\{\{ toJSON\(needs\) \}\}$/m);
  assert.match(required, /^        run: node \.github\/scripts\/ci-policy\.mjs gate$/m);
  assert.doesNotMatch(required, /^ *continue-on-error:/m, 'gate errors must fail the job');
});
