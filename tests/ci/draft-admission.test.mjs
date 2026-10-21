import assert from 'node:assert/strict';
import { execFileSync, spawnSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { runInNewContext } from 'node:vm';
import test from 'node:test';
import { admissionState, AdmissionDeferredError, selectScope, validateGate, validatePreview } from '../../.github/scripts/ci-policy.mjs';

const script = fileURLToPath(new URL('../../.github/scripts/ci-policy.mjs', import.meta.url));
const workflow = readFileSync(new URL('../../.github/workflows/ci.yml', import.meta.url), 'utf8').replaceAll('\r\n', '\n');
const jobs = ['network', 'applications', 'database'];
const site = { present: true, available: true };
const plan = (files, eventName = 'pull_request') => selectScope({ files, eventName, site });
const results = scope => ({
  scope: { result: 'success', outputs: Object.fromEntries(Object.entries(scope).map(([key, value]) => [key, String(value)])) },
  ...Object.fromEntries(jobs.map(job => [job, { result: scope[`${job}_check`] ? 'success' : 'skipped' }])),
  validation: { result: 'success', outputs: {
    documentation_outcome: scope.docs_check ? 'success' : 'skipped',
    readme_outcome: scope.readme_check ? 'success' : 'skipped',
    formatting_outcome: scope.format_check ? 'success' : 'skipped',
  } },
});
const jobBlock = name => workflow.split(/(?=^  [a-z][a-z0-9_-]*:$)/m).find(block => block.startsWith(`  ${name}:\n`));

// Evaluate the actual workflow guard with its JSON conversion and the subset of
// expression operators it uses. Do not duplicate the guard as a test-only policy.
function nativeRuns(job, scope, eventName, draft, scopeResult = 'success') {
  const expression = jobBlock(job).match(/^    if: \$\{\{ (.+) \}\}$/m)?.[1];
  assert.ok(expression, `${job}: explicit native admission guard`);
  return runInNewContext(expression, {
    cancelled: () => false, toJSON: JSON.stringify,
    github: { event_name: eventName, event: { pull_request: { draft } } },
    needs: { scope: { result: scopeResult, outputs: results(scope).scope.outputs } },
  });
}

function fixture(t) {
  const root = mkdtempSync(resolve(tmpdir(), 'ark-draft-admission-'));
  t.after(() => rmSync(root, { recursive: true, force: true }));
  const git = (...args) => execFileSync('git', args, { cwd: root, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] }).trim();
  const write = (file, text) => { mkdirSync(dirname(resolve(root, file)), { recursive: true }); writeFileSync(resolve(root, file), text); };
  const commit = () => { git('add', '-A'); git('-c', 'commit.gpgsign=false', 'commit', '-m', 'Draft admission fixture'); return git('rev-parse', 'HEAD'); };
  git('init', '--initial-branch=main');
  git('config', 'user.name', 'ark CI tests');
  git('config', 'user.email', 'ci-tests@example.invalid');
  git('config', 'core.hooksPath', resolve(root, 'empty-hooks'));
  write('docs/site/package.json', JSON.stringify({ scripts: { check: 'node check.mjs' } }));
  write('docs/site/package-lock.json', '{}');
  write('docs/guide.md', '# Guide\n');
  write('tests/network/example.cpp', 'int example;\n');
  const base = commit();
  const metadata = mkdtempSync(resolve(tmpdir(), 'ark-draft-event-'));
  t.after(() => rmSync(metadata, { recursive: true, force: true }));
  function invoke(command, eventName, event, needs) {
    const eventPath = resolve(metadata, 'event.json');
    const summaryPath = resolve(metadata, 'summary');
    const outputPath = resolve(metadata, 'outputs');
    writeFileSync(eventPath, typeof event === 'string' ? event : JSON.stringify(event));
    writeFileSync(summaryPath, '');
    writeFileSync(outputPath, '');
    const env = { ...process.env, GITHUB_EVENT_NAME: eventName, GITHUB_EVENT_PATH: eventPath,
      GITHUB_STEP_SUMMARY: summaryPath, GITHUB_OUTPUT: outputPath, CI_NEEDS_JSON: JSON.stringify(needs) };
    delete env.GITHUB_TOKEN;
    delete env.GH_TOKEN;
    const run = spawnSync(process.execPath, [script, command], { cwd: root, env, encoding: 'utf8' });
    assert.ifError(run.error);
    return { ...run, summary: readFileSync(summaryPath, 'utf8'), output: readFileSync(outputPath, 'utf8') };
  }
  return { root, write, commit, base, invoke };
}

const pr = (base, head, draft = false, action = 'synchronize') => ({ action, pull_request: { draft, base: { sha: base }, head: { sha: head } } });

test('only explicit boolean draft metadata can defer a pull request', () => {
  assert.equal(admissionState('pull_request', { pull_request: { draft: true } }), 'deferred');
  assert.equal(admissionState('pull_request', { pull_request: { draft: false } }), 'ready');
  for (const draft of [undefined, null, 'true', 'false', 0, 1, {}, []]) {
    assert.throws(() => admissionState('pull_request', { pull_request: { draft } }), /draft metadata/);
  }
  for (const event of [undefined, {}, { pull_request: null }]) assert.throws(() => admissionState('pull_request', event));
  for (const eventName of ['merge_group', 'workflow_dispatch']) {
    assert.equal(admissionState(eventName, { pull_request: { draft: true } }), 'ready');
  }
  assert.throws(() => admissionState('push', {}), /unsupported/);
});

test('workflow admits fresh ready events and keeps existing concurrency and strict gate', () => {
  assert.match(workflow, /^    types: \[opened, synchronize, reopened, ready_for_review, converted_to_draft\]$/m);
  assert.match(workflow, /^  cancel-in-progress: false$/m);
  assert.match(jobBlock('required'), /^    if: always\(\)$/m);
  assert.match(jobBlock('required'), /^    name: \$\{\{ .* && 'CI - draft preview' \|\| 'CI - required' \}\}$/m);
  assert.doesNotMatch(jobBlock('required'), /continue-on-error:|if:.*draft/);
  assert.match(jobBlock('validation'), /^    if: \$\{\{ !cancelled\(\) \}\}$/m, 'light validation is not draft-gated');
  assert.doesNotMatch(workflow, /pull_request_target:|gh pr |enable-auto-merge|merge-queue/);
});

test('actual native guards suppress drafts but preserve ready source selection and full queue matrices', () => {
  for (const files of [['tests/network/example.cpp'], ['README.md'], ['.github/workflows/ci.yml'], []]) {
    const scope = plan(files);
    for (const job of jobs) {
      assert.equal(nativeRuns(job, scope, 'pull_request', true), false, `${job}: draft ${files}`);
      assert.equal(nativeRuns(job, scope, 'pull_request', true, 'failure'), false, `${job}: failed draft scope`);
      assert.equal(nativeRuns(job, scope, 'pull_request', false), scope[`${job}_check`], `${job}: ready ${files}`);
      for (const draft of [undefined, null, 'true', 1]) {
        assert.equal(nativeRuns(job, plan([]), 'pull_request', draft), true, `${job}: invalid metadata must not suppress full fallback`);
      }
      assert.equal(nativeRuns(job, scope, 'pull_request', false, 'failure'), true, `${job}: failed ready scope runs conservatively`);
    }
  }
  const queue = plan(['tests/network/example.cpp'], 'merge_group');
  assert.equal(queue.validation_stage, 'full');
  let cells = 0;
  for (const job of jobs) {
    assert.equal(nativeRuns(job, queue, 'merge_group', true), true, `${job}: PR metadata cannot defer queue`);
    const matrices = jobBlock(job).match(/^      matrix: .* && '([^']+)' \|\| '([^']+)'\) \}\}$/m);
    assert.ok(matrices);
    cells += JSON.parse(matrices[2]).include.length;
  }
  assert.equal(cells, 12);
});

test('deferred gate never passes even documentation or successful-looking native results', () => {
  for (const files of [['README.md'], ['tests/network/example.cpp'], ['.github/workflows/ci.yml']]) {
    const scope = plan(files);
    for (const skipped of [false, true]) {
      const needs = results(scope);
      if (skipped) for (const job of jobs) needs[job].result = 'skipped';
      assert.throws(() => validateGate(needs, 'pull_request', 'deferred'), AdmissionDeferredError);
    }
    validateGate(results(scope), 'pull_request', 'ready');
  }
  assert.throws(() => validateGate({}, 'merge_group', 'deferred'), /only draft/);
  assert.throws(() => validateGate({}, 'pull_request', 'unknown'), /invalid admission/);
});

test('real draft classification preserves required flags and ready_for_review requires real native success', t => {
  const repo = fixture(t);
  repo.write('tests/network/example.cpp', 'int changed;\n');
  const head = repo.commit();
  const draftEvent = pr(repo.base, head, true, 'opened');
  const draftRun = repo.invoke('classify', 'pull_request', draftEvent);
  assert.equal(draftRun.status, 0, draftRun.stderr);
  const draft = JSON.parse(draftRun.stdout);
  assert.equal(draft.admission, 'deferred');
  assert.equal(draft.network_check, true);
  assert.equal(draft.native_check, true);
  assert.equal(draft.validation_stage, 'modules');
  assert.equal(draft.format_check, true);
  assert.match(draftRun.summary, /Light checks do not admit this draft/);
  const held = results(draft);
  held.network.result = 'skipped';
  const gate = repo.invoke('gate', 'pull_request', draftEvent, held);
  assert.equal(gate.status, 1);
  assert.match(gate.stderr, /\[DRAFT_ADMISSION_DEFERRED\]/);
  assert.match(gate.summary, /not a passing check/);
  assert.match(gate.summary, /not admitted/);
  assert.doesNotMatch(gate.stdout, /Every required job/);

  const readyEvent = pr(repo.base, head, false, 'ready_for_review');
  const ready = JSON.parse(repo.invoke('classify', 'pull_request', readyEvent).stdout);
  assert.equal(ready.admission, 'ready');
  const { admission: draftAdmission, ...draftScope } = draft;
  const { admission: readyAdmission, ...readyScope } = ready;
  assert.deepEqual(readyScope, draftScope, 'ready event changes admission, not scope requirements');
  assert.equal(repo.invoke('gate', 'pull_request', readyEvent, held).status, 1, 'skipped native cannot admit ready PR');
  assert.equal(repo.invoke('gate', 'pull_request', readyEvent, results(ready)).status, 0);
  for (const outcome of ['failure', 'cancelled', 'skipped']) {
    const needs = results(ready);
    needs.network.result = outcome;
    assert.equal(repo.invoke('gate', 'pull_request', readyEvent, needs).status, 1);
  }
});

test('documentation drafts stay unadmitted even when every real light step succeeds', t => {
  const repo = fixture(t);
  repo.write('docs/guide.md', '# Updated guide\n');
  const head = repo.commit();
  const event = pr(repo.base, head, true, 'converted_to_draft');
  const scope = JSON.parse(repo.invoke('classify', 'pull_request', event).stdout);
  assert.equal(scope.native_check, false);
  assert.equal(scope.docs_check, true);
  const gate = repo.invoke('gate', 'pull_request', event, results(scope));
  assert.equal(gate.status, 1);
  assert.match(gate.stderr, /DRAFT_ADMISSION_DEFERRED/);
  const readyEvent = pr(repo.base, head, false, 'ready_for_review');
  assert.equal(repo.invoke('gate', 'pull_request', readyEvent, results(scope)).status, 0);
});

test('invalid draft metadata and invalid ready ranges cannot enable a lightweight bypass', t => {
  const repo = fixture(t);
  repo.write('docs/guide.md', '# Updated guide\n');
  const head = repo.commit();
  for (const draft of [undefined, null, 'true', 'false', 0, 1]) {
    const event = pr(repo.base, head, draft);
    if (draft === undefined) delete event.pull_request.draft;
    const run = repo.invoke('classify', 'pull_request', event);
    const scope = JSON.parse(run.stdout);
    assert.equal(scope.admission, 'unknown');
    assert.equal(scope.validation_stage, 'full');
    assert.equal(scope.reason, 'enumeration-unavailable');
    for (const job of jobs) assert.equal(scope[`${job}_check`], true);
    assert.match(run.summary, /Invalid draft metadata/);
    const gate = repo.invoke('gate', 'pull_request', event, results(scope));
    assert.equal(gate.status, 1, 'even successful-looking results cannot replace real draft metadata');
    assert.match(gate.stderr, /draft metadata/);
  }
  for (const base of [undefined, 'invalid', '0'.repeat(40), 'f'.repeat(40)]) {
    const event = pr(base, head, false, 'ready_for_review');
    const scope = JSON.parse(repo.invoke('classify', 'pull_request', event).stdout);
    assert.equal(scope.validation_stage, 'full');
    assert.equal(scope.reason, 'enumeration-unavailable');
    const incomplete = results(scope);
    incomplete.database.result = 'skipped';
    assert.equal(repo.invoke('gate', 'pull_request', event, incomplete).status, 1);
  }
  assert.equal(repo.invoke('gate', 'pull_request', '{not JSON', results(plan(['README.md']))).status, 1);
});

test('queue code remains full and cannot reuse source module acceptance', t => {
  const repo = fixture(t);
  repo.write('tests/network/example.cpp', 'int changed;\n');
  const head = repo.commit();
  const event = { merge_group: { base_sha: repo.base, head_sha: head }, pull_request: { draft: true } };
  const scope = JSON.parse(repo.invoke('classify', 'merge_group', event).stdout);
  assert.equal(scope.admission, 'ready');
  assert.equal(scope.validation_stage, 'full');
  for (const job of jobs) assert.equal(scope[`${job}_check`], true);
  assert.equal(repo.invoke('gate', 'merge_group', event, results(scope)).status, 0);
  const source = plan(['tests/network/example.cpp']);
  assert.equal(repo.invoke('gate', 'merge_group', event, results(source)).status, 1);
  const incomplete = results(scope);
  incomplete.applications.result = 'skipped';
  assert.equal(repo.invoke('gate', 'merge_group', event, incomplete).status, 1);
});

function aggregateFor(eventName, draft) {
  const aggregate = jobBlock('required');
  const name = aggregate.match(/^    name: \$\{\{ (.+) \}\}$/m)?.[1];
  const command = aggregate.match(/^        run: node \.github\/scripts\/ci-policy\.mjs \$\{\{ (.+) \}\}$/m)?.[1];
  assert.ok(name);
  assert.ok(command);
  const predicate = "github.event_name == 'pull_request' && toJSON(github.event.pull_request.draft) == 'true'";
  assert.equal(name, `${predicate} && 'CI - draft preview' || 'CI - required'`);
  assert.equal(command, `${predicate} && 'preview' || 'gate'`);
  const context = { github: { event_name: eventName, event: { pull_request: { draft } } }, toJSON: JSON.stringify };
  // Both job name and run support github context. No step, matrix, env or
  // unavailable job-output context is involved in these admission expressions.
  return { name: runInNewContext(name, context), command: runInNewContext(command, context) };
}

const previewResults = scope => {
  const needs = results(scope);
  for (const job of jobs) needs[job].result = 'skipped';
  return needs;
};

test('actual aggregate expressions select only draft preview and leave the required name to ready or queue runs', () => {
  assert.deepEqual(aggregateFor('pull_request', true), { name: 'CI - draft preview', command: 'preview' });
  for (const draft of [false, undefined, null, 'true', 'false', 0, 1]) {
    assert.deepEqual(aggregateFor('pull_request', draft), { name: 'CI - required', command: 'gate' });
  }
  for (const eventName of ['merge_group', 'workflow_dispatch']) {
    assert.deepEqual(aggregateFor(eventName, true), { name: 'CI - required', command: 'gate' });
  }
});

test('preview succeeds only for genuine draft scope and real selected light successes with every native job skipped', () => {
  for (const files of [['README.md'], ['tests/network/example.cpp'], ['README.md', 'tests/network/example.cpp'], []]) {
    const scope = plan(files);
    const needs = previewResults(scope);
    validatePreview(needs, 'pull_request', 'deferred');
    assert.throws(() => validateGate(needs, 'pull_request', 'deferred'), AdmissionDeferredError);
    for (const admission of ['ready', 'unknown', undefined, true]) {
      assert.throws(() => validatePreview(needs, 'pull_request', admission), /genuine draft/);
    }
    for (const eventName of ['merge_group', 'workflow_dispatch']) {
      assert.throws(() => validatePreview(needs, eventName, 'deferred'), /genuine draft/);
    }
    for (const job of jobs) {
      for (const result of ['success', 'failure', 'cancelled', undefined, 'unknown']) {
        const invalid = structuredClone(needs);
        invalid[job].result = result;
        assert.throws(() => validatePreview(invalid, 'pull_request', 'deferred'), /expected skipped/);
      }
    }
  }
});

test('mixed preview light checks reject failures, skips, missing outcomes and contradictory scope', () => {
  const scope = plan(['README.md', 'tests/network/example.cpp']);
  for (const check of ['documentation', 'readme', 'formatting']) {
    for (const outcome of ['failure', 'cancelled', 'skipped', undefined, 'unknown']) {
      const needs = previewResults(scope);
      needs.validation.outputs[`${check}_outcome`] = outcome;
      assert.throws(() => validatePreview(needs, 'pull_request', 'deferred'), new RegExp(`${check}: expected success`));
    }
  }
  for (const job of ['scope', 'validation']) {
    for (const result of ['failure', 'cancelled', 'skipped', undefined]) {
      const needs = previewResults(scope);
      needs[job].result = result;
      assert.throws(() => validatePreview(needs, 'pull_request', 'deferred'));
    }
  }
  for (const [key, value] of [['network_check', undefined], ['format_check', 'false'], ['validation_stage', 'documentation'],
    ['event_name', 'merge_group'], ['native_check', 'false'], ['reason', 'documentation-only']]) {
    const needs = previewResults(scope);
    needs.scope.outputs[key] = value;
    assert.throws(() => validatePreview(needs, 'pull_request', 'deferred'));
  }
  const unselected = previewResults(plan(['docs/guide.md']));
  unselected.validation.outputs.formatting_outcome = 'success';
  assert.throws(() => validatePreview(unselected, 'pull_request', 'deferred'), /formatting: expected skipped/);
});

test('actual workflow command reports preview success without reporting required acceptance and rejects real light failure', t => {
  const repo = fixture(t);
  repo.write('docs/guide.md', '# Updated guide\n');
  repo.write('tests/network/example.cpp', 'int changed;\n');
  const head = repo.commit();
  const event = pr(repo.base, head, true, 'opened');
  const scope = JSON.parse(repo.invoke('classify', 'pull_request', event).stdout);
  const aggregate = aggregateFor('pull_request', event.pull_request.draft);
  assert.equal(aggregate.name, 'CI - draft preview');
  const needs = previewResults(scope);
  const preview = repo.invoke(aggregate.command, 'pull_request', event, needs);
  assert.equal(preview.status, 0, preview.stderr);
  assert.match(preview.summary, /Draft preview only/);
  assert.match(preview.summary, /not executed in this run/);
  assert.match(preview.summary, /not admitted/);
  assert.match(preview.summary, /does not report CI - required success/);
  assert.doesNotMatch(preview.stdout, /Every required job/);
  needs.validation.outputs.documentation_outcome = 'failure';
  const failed = repo.invoke(aggregate.command, 'pull_request', event, needs);
  assert.equal(failed.status, 1);
  assert.match(failed.stderr, /documentation: expected success/);
  assert.doesNotMatch(failed.summary, /Draft preview only/);
  const ready = pr(repo.base, head, false, 'ready_for_review');
  assert.equal(repo.invoke('preview', 'pull_request', ready, previewResults(scope)).status, 1);
  const full = plan(['tests/network/example.cpp'], 'merge_group');
  assert.equal(repo.invoke('preview', 'merge_group', { merge_group: {} }, previewResults(full)).status, 1);
  for (const draft of [undefined, 'true', 1]) {
    const invalid = pr(repo.base, head, draft);
    if (draft === undefined) delete invalid.pull_request.draft;
    assert.equal(repo.invoke('preview', 'pull_request', invalid, previewResults(scope)).status, 1);
  }
});
