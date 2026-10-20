import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { chmodSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { resolve } from 'node:path';
import test from 'node:test';
import { selectScope, validateGate } from '../../.github/scripts/ci-policy.mjs';

const workflow = readFileSync(new URL('../../.github/workflows/ci.yml', import.meta.url), 'utf8');
const job = name => workflow.split(/(?=^  [a-z][a-z0-9_-]*:$)/m).find(block => block.startsWith(`  ${name}:\n`));
const validation = job('validation');
const checks = { documentation: 'docs_check', readme: 'readme_check', formatting: 'format_check' };
const bindings = [...validation.matchAll(/^      (\w+): \$\{\{ steps\.(\w+)\.outcome \}\}$/gm)];

function results(plan) {
  return {
    scope: { result: 'success', outputs: Object.fromEntries(Object.entries(plan).map(([key, value]) => [key, String(value)])) },
    ...Object.fromEntries(['network', 'applications', 'database'].map(name =>
      [name, { result: plan[`${name}_check`] ? 'success' : 'skipped' }])),
    validation: { result: 'success', outputs: Object.fromEntries(bindings.map(([, output, step]) =>
      [output, plan[checks[step]] ? 'success' : 'skipped'])) },
  };
}

test('one shared validation runner leaves native jobs free to start as soon as scope completes', () => {
  assert.deepEqual([...workflow.split('jobs:\n')[1].matchAll(/^  ([a-z][a-z0-9_-]*):$/gm)].map(match => match[1]),
    ['scope', 'validation', 'network', 'applications', 'database', 'required']);
  for (const name of ['validation', 'network', 'applications', 'database']) {
    assert.match(job(name), /^    needs: scope$/m);
  }
  assert.equal([...validation.matchAll(/uses: actions\/checkout@v4/g)].length, 1);
  assert.equal([...validation.matchAll(/uses: actions\/setup-node@v4/g)].length, 1);
  assert.match(validation, /cache: \$\{\{ needs.scope.outputs.docs_check != 'false' && needs.scope.outputs.site_available == 'true' && 'npm' \|\| '' \}\}/);
  assert.match(validation, /cache-dependency-path: docs\/site\/package-lock.json/);
  assert.match(validation, /node .github\/scripts\/ci-policy.mjs require-site\n          npm ci --prefix docs\/site\n          npm run check --prefix docs\/site/);
});

test('each actual check exports its raw outcome and runs independently of earlier check failures', () => {
  assert.deepEqual(bindings.map(([, output, step]) => [output, step]),
    Object.keys(checks).map(name => [`${name}_outcome`, name]));
  for (const [name, flag] of Object.entries(checks)) {
    assert.ok(validation.includes(`id: ${name}\n        if: \${{ !cancelled() && (needs.scope.result != 'success' || needs.scope.outputs.${flag} != 'false') }}`));
  }
  assert.match(validation, /run: CLANG_FORMAT=clang-format-18 bash scripts\/format.sh --check/);
  assert.doesNotMatch(validation, /continue-on-error:|\.conclusion/);
});

test('a successful shared job cannot hide missing failed or unexpectedly skipped check outcomes', () => {
  for (const eventName of ['pull_request', 'merge_group', 'workflow_dispatch']) {
    for (const files of [['README.md'], ['docs/guide.md'], ['tests/network/rudp.cpp'], ['.github/workflows/ci.yml']]) {
      for (const site of [{ present: true, available: true }, { present: false, available: false }]) {
        const plan = selectScope({ eventName, files, site });
        const valid = results(plan);
        validateGate(valid, eventName);
        for (const [name, flag] of Object.entries(checks)) {
          const output = `${name}_outcome`;
          assert.equal(valid.validation.outputs[output], plan[flag] ? 'success' : 'skipped');
          for (const outcome of ['success', 'skipped', 'failure', 'cancelled', 'unknown', 'neutral', '', null, undefined]) {
            if (outcome === valid.validation.outputs[output]) continue;
            const invalid = structuredClone(valid);
            invalid.validation.outputs[output] = outcome;
            assert.throws(() => validateGate(invalid, eventName), `${eventName}/${files}/${name}/${outcome}`);
          }
          const absent = structuredClone(valid);
          delete absent.validation.outputs[output];
          assert.throws(() => validateGate(absent, eventName));
        }
        for (const result of ['failure', 'cancelled', 'skipped', undefined]) {
          const invalid = structuredClone(valid);
          invalid.validation.result = result;
          assert.throws(() => validateGate(invalid, eventName));
        }
        const absent = structuredClone(valid);
        delete absent.validation.outputs;
        assert.throws(() => validateGate(absent, eventName));
      }
    }
  }
});

test('the actual tool setup avoids apt when clang-format 18 is already installed', t => {
  const root = mkdtempSync(resolve(tmpdir(), 'ark-ci-formatter-'));
  t.after(() => rmSync(root, { recursive: true, force: true }));
  const script = validation.match(/      - name: Ensure clang-format 18 is available\n[\s\S]*?        run: \|\n([\s\S]*?)(?=      - name:)/)?.[1]
    .replace(/^          /gm, '');
  assert.ok(script);
  for (const [name, body] of Object.entries({
    'clang-format-18': '#!/bin/sh\necho "clang-format version 18.1.8"\n',
    sudo: '#!/bin/sh\necho "apt should not run" >&2\nexit 99\n',
  })) {
    writeFileSync(resolve(root, name), body);
    chmodSync(resolve(root, name), 0o755);
  }
  assert.match(execFileSync('/bin/bash', ['-e', '-c', script], { env: { PATH: root }, encoding: 'utf8' }), /version 18/);
  rmSync(resolve(root, 'clang-format-18'));
  assert.throws(() => execFileSync('/bin/bash', ['-e', '-c', script],
    { env: { PATH: root }, stdio: ['ignore', 'pipe', 'pipe'] }), error => error.status === 99);
});
