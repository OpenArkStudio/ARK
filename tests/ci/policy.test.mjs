import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, readFileSync, renameSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import test from 'node:test';
import { changedPaths, decodePaths, inspectSite, isDocumentationPath, selectScope, validateGate as validateActualGate } from '../../.github/scripts/ci-policy.mjs';

const validateGate = needs => validateActualGate(needs, needs.scope?.outputs?.event_name);

const site = { present: true, available: true };
const decision = (files, extra = {}) => selectScope({ eventName: 'pull_request', files, site, ...extra });

function repository(t) {
  const root = mkdtempSync(resolve(tmpdir(), 'ark-ci-policy-'));
  t.after(() => rmSync(root, { recursive: true, force: true }));
  const git = (...args) => execFileSync('git', args, { cwd: root, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] }).trim();
  const write = (file, text) => { mkdirSync(dirname(resolve(root, file)), { recursive: true }); writeFileSync(resolve(root, file), text); };
  const commit = () => { git('add', '-A'); git('-c', 'commit.gpgsign=false', 'commit', '-m', 'CI policy fixture'); return git('rev-parse', 'HEAD'); };
  git('init', '--initial-branch=main');
  git('config', 'user.name', 'ark CI tests');
  git('config', 'user.email', 'ci-tests@example.invalid');
  git('config', 'core.hooksPath', resolve(root, 'empty-hooks'));
  write('src/main.cpp', 'int main() {}\n');
  write('docs/guide.md', '# Guide\n');
  const base = commit();
  return { root, git, write, commit, base };
}

function addSite(repo) {
  repo.write('docs/site/package.json', JSON.stringify({ scripts: { check: 'node check.mjs' } }));
  repo.write('docs/site/package-lock.json', '{}');
  return repo.commit();
}

function classifyEvent(repo, eventName, payload) {
  const metadata = mkdtempSync(resolve(tmpdir(), 'ark-ci-event-'));
  try {
    const event = resolve(metadata, 'event.json');
    writeFileSync(event, typeof payload === 'string' ? payload : JSON.stringify(payload));
    const env = { ...process.env, GITHUB_EVENT_NAME: eventName, GITHUB_EVENT_PATH: event,
      GITHUB_OUTPUT: resolve(metadata, 'outputs'), GITHUB_STEP_SUMMARY: resolve(metadata, 'summary') };
    delete env.GITHUB_TOKEN;
    delete env.GH_TOKEN;
    const script = fileURLToPath(new URL('../../.github/scripts/ci-policy.mjs', import.meta.url));
    const plan = JSON.parse(execFileSync(process.execPath, [script, 'classify'], { cwd: repo.root, env, encoding: 'utf8' }));
    return { plan, output: readFileSync(env.GITHUB_OUTPUT, 'utf8'), summary: readFileSync(env.GITHUB_STEP_SUMMARY, 'utf8') };
  } finally {
    rmSync(metadata, { recursive: true, force: true });
  }
}

test('only recognized documentation paths qualify for the lightweight plan', () => {
  for (const file of ['README.md', 'README_CN.md', 'docs/guide.md', 'docs/site/src/Header.astro', 'docs/assets/chart.svg']) {
    assert.equal(isDocumentationPath(file), true, file);
    assert.equal(decision([file]).native_check, false, file);
  }
  assert.equal(decision(['README.md']).readme_check, true);
  assert.equal(decision(['docs/guide.md']).readme_check, false);
  assert.equal(decision(['README.md', 'docs/guide.md']).docs_check, true);
});

test('code, build, dependency, CI and unknown paths cannot skip native regressions', () => {
  const paths = ['include/ark/xml.hpp', 'CMakeLists.txt', 'cmake/FindZstd.cmake', 'vcpkg.json', '.gitmodules',
    '.github/workflows/ci.yml', '.github/scripts/ci-policy.mjs', 'tests/core/templates.cpp', 'scripts/format.sh',
    'LICENSE', 'README_CN.md.bak', 'docs/example.cpp', 'docs/example.h', 'docs/message.proto', 'docs/CMakeLists.txt',
    'docs/build.cmake', 'docs/CMakePresets.json', 'docs/CMakeUserPresets.json', 'docs/CMakeCache.txt',
    'docs/action.yml', 'docs/site/package.json', 'docs/site/package-lock.json', 'docs/unknown.bin',
    'docs/.github/workflows/example.yml', 'docs/requirements.txt', 'docs/../README.md', './docs/guide.md', 'docs//guide.md'];
  for (const file of paths) {
    // requirements.txt is a dependency file even though ordinary text is documentation.
    assert.equal(decision(['docs/guide.md', file]).native_check, true, file);
  }
});

test('a code path beyond 3000 documentation entries cannot be truncated out of the decision', () => {
  const files = Array.from({ length: 3001 }, (_, index) => `docs/page-${index}.md`);
  files.push('src/last.cpp');
  const decoded = decodePaths(Buffer.from(files.join('\0') + '\0'));
  assert.equal(decoded.length, 3002);
  assert.equal(decision(decoded).native_check, true);
});

test('empty, missing, truncated and invalid UTF-8 path lists never grant a lightweight result', () => {
  assert.equal(decision([]).native_check, true);
  assert.equal(decision(undefined, { enumerationFailed: true }).native_check, true);
  assert.throws(() => decodePaths(Buffer.from('docs/guide.md')));
  assert.throws(() => decodePaths(Buffer.from([0xff, 0])));
  assert.throws(() => decodePaths(Buffer.from('docs/a.md\0docs/a.md\0')));
  assert.throws(() => decodePaths(Buffer.from('\0')));
  assert.deepEqual(decodePaths(Buffer.from('docs/a.md\0README.md\0')), ['docs/a.md', 'README.md']);
  assert.equal(decision(['docs/a\nname.md']).native_check, true);
});

test('events without changed-path scoping cannot grant a lightweight classifier result', () => {
  for (const eventName of ['push', 'workflow_dispatch', 'unknown']) {
    const plan = decision(['docs/guide.md'], { eventName });
    assert.equal(plan.native_check, true);
    assert.equal(plan.docs_check, true);
  }
});

test('a missing legacy site forces full C++ and does not falsely report a site check', () => {
  const plan = decision(['README.md'], { site: { present: false, available: false } });
  assert.equal(plan.native_check, true);
  assert.equal(plan.docs_check, false);
  assert.equal(plan.readme_check, true);
  assert.equal(decision(['src/main.cpp'], { site: { present: false, available: false } }).readme_check, false);
});

test('CI policy changes also validate the site when it has been introduced', () => {
  const plan = decision(['.github/workflows/ci.yml', 'tests/ci/policy.test.mjs']);
  assert.equal(plan.native_check, true);
  assert.equal(plan.docs_check, true);
  assert.equal(plan.readme_check, false);
});

test('an introduced, incomplete or removed site requires a failing preflight instead of a silent skip', () => {
  const incomplete = { present: true, available: false };
  assert.equal(decision(['docs/guide.md'], { site: incomplete }).docs_check, true);
  const removed = decision(['docs/site/package-lock.json'], { site: { present: false, available: false } });
  assert.equal(removed.native_check, true);
  assert.equal(removed.docs_check, true);
  assert.equal(decision(['.github/workflows/docs.yml', 'docs/site/package-lock.json']).native_check, true);
});

test('site availability requires its actual lockfile and npm check command', t => {
  const repo = repository(t);
  assert.deepEqual(inspectSite(repo.root), { present: false, available: false });
  repo.write('docs/site/package.json', JSON.stringify({ scripts: { check: 'node check.mjs' } }));
  assert.deepEqual(inspectSite(repo.root), { present: true, available: false });
  repo.write('docs/site/package-lock.json', '{}');
  assert.deepEqual(inspectSite(repo.root), site);
  repo.write('docs/site/package.json', '{}');
  assert.equal(inspectSite(repo.root).available, false);
});

test('a real Git rename exposes both the removed code path and added documentation path', t => {
  const repo = repository(t);
  renameSync(resolve(repo.root, 'src/main.cpp'), resolve(repo.root, 'docs/moved.md'));
  const head = repo.commit();
  const files = changedPaths({ root: repo.root, base: repo.base, head });
  assert.deepEqual(files.sort(), ['docs/moved.md', 'src/main.cpp']);
  assert.equal(decision(files).native_check, true);
});

test('merge-base comparison excludes unrelated changes made on the base branch', t => {
  const repo = repository(t);
  repo.git('checkout', '-b', 'topic');
  repo.write('docs/guide.md', '# Changed topic guide\n');
  const head = repo.commit();
  repo.git('checkout', 'main');
  repo.write('src/main.cpp', 'int main() { return 1; }\n');
  const base = repo.commit();
  assert.deepEqual(changedPaths({ root: repo.root, base, head }), ['docs/guide.md']);
  assert.equal(decision(changedPaths({ root: repo.root, base, head })).native_check, false);
});

test('Git errors, shallow history, bad SHAs and command-looking arguments cannot approve skipping C++', () => {
  const identity = 'a'.repeat(40);
  const input = { root: '/unused', base: identity, head: 'b'.repeat(40) };
  assert.throws(() => changedPaths(input, () => { throw new Error('Git unavailable'); }));
  assert.throws(() => changedPaths(input, () => Buffer.from('true\n')));
  for (const base of ['', '0000000000000000000000000000000000000000', 'main; exit 0', '--help']) {
    assert.throws(() => changedPaths({ ...input, base }, () => assert.fail('invalid input must not invoke Git')));
  }
});

test('the real classifier handles a fork PR from pinned local history without an API or token', t => {
  const repo = repository(t);
  repo.write('docs/site/package.json', JSON.stringify({ scripts: { check: 'node check.mjs' } }));
  repo.write('docs/site/package-lock.json', '{}');
  const base = repo.commit();
  repo.write('docs/guide.md', '# Fork change\n');
  const head = repo.commit();
  const event = resolve(repo.root, 'event.json');
  writeFileSync(event, JSON.stringify({ pull_request: { base: { sha: base }, head: { sha: head, repo: { fork: true } } } }));
  const env = { ...process.env, GITHUB_EVENT_NAME: 'pull_request', GITHUB_EVENT_PATH: event,
    GITHUB_OUTPUT: resolve(repo.root, 'outputs'), GITHUB_STEP_SUMMARY: resolve(repo.root, 'summary') };
  delete env.GITHUB_TOKEN;
  delete env.GH_TOKEN;
  const script = fileURLToPath(new URL('../../.github/scripts/ci-policy.mjs', import.meta.url));
  const result = JSON.parse(execFileSync(process.execPath, [script, 'classify'], { cwd: repo.root, env, encoding: 'utf8' }));
  assert.equal(result.native_check, false);
  assert.equal(result.docs_check, true);
  assert.equal(result.readme_check, false);
  assert.match(readFileSync(env.GITHUB_OUTPUT, 'utf8'), /native_check=false\n/);
});

function results(plan) {
  return {
    scope: { result: 'success', outputs: Object.fromEntries(Object.entries(plan).map(([key, value]) => [key, String(value)])) },
    network: { result: plan.network_check ? 'success' : 'skipped' },
    applications: { result: plan.applications_check ? 'success' : 'skipped' },
    database: { result: plan.database_check ? 'success' : 'skipped' },
    validation: { result: 'success', outputs: {
      formatting_outcome: plan.format_check ? 'success' : 'skipped',
      documentation_outcome: plan.docs_check ? 'success' : 'skipped',
      readme_outcome: plan.readme_check ? 'success' : 'skipped',
    } },
  };
}

test('the real classifier distinguishes module PR validation from full queue and unavailable-range validation', t => {
  const repo = repository(t);
  const base = addSite(repo);
  repo.write('tests/network/probe.cpp', 'int network_probe() { return 0; }\n');
  const head = repo.commit();
  const source = classifyEvent(repo, 'pull_request', { pull_request: { base: { sha: base }, head: { sha: head } } }).plan;
  assert.equal(source.validation_stage, 'modules');
  assert.equal(source.network_check, true);
  assert.equal(source.applications_check, false);
  assert.equal(source.database_check, false);
  validateGate(results(source));
  const queue = classifyEvent(repo, 'merge_group', { merge_group: { base_sha: base, head_sha: head } }).plan;
  assert.equal(queue.validation_stage, 'full');
  for (const key of ['network_check', 'applications_check', 'database_check']) assert.equal(queue[key], true);
  validateGate(results(queue));
  const missing = classifyEvent(repo, 'pull_request', {}).plan;
  assert.equal(missing.validation_stage, 'full');
  assert.equal(missing.files_known, false);
  for (const key of ['network_check', 'applications_check', 'database_check']) assert.equal(missing[key], true);
  validateGate(results(missing));
});

test('a documentation-only merge queue candidate runs its required site check and explicitly skips native jobs', t => {
  const repo = repository(t);
  repo.write('docs/site/package.json', JSON.stringify({ scripts: { check: 'node check.mjs' } }));
  repo.write('docs/site/package-lock.json', '{}');
  const base = repo.commit();
  repo.write('docs/guide.md', '# Queued documentation change\n');
  const head = repo.commit();
  const event = resolve(repo.root, 'event.json');
  writeFileSync(event, JSON.stringify({ action: 'checks_requested', merge_group: { base_sha: base, head_sha: head } }));
  const script = fileURLToPath(new URL('../../.github/scripts/ci-policy.mjs', import.meta.url));
  const env = { ...process.env, GITHUB_EVENT_NAME: 'merge_group', GITHUB_EVENT_PATH: event,
    GITHUB_OUTPUT: resolve(repo.root, 'outputs'), GITHUB_STEP_SUMMARY: resolve(repo.root, 'summary') };
  delete env.GITHUB_TOKEN;
  delete env.GH_TOKEN;
  const plan = JSON.parse(execFileSync(process.execPath, [script, 'classify'], { cwd: repo.root, env, encoding: 'utf8' }));
  assert.equal(plan.native_check, false);
  assert.equal(plan.docs_check, true);
  assert.equal(plan.readme_check, false);
  assert.equal(plan.files_known, true);
  assert.equal(plan.reason, 'documentation-only');
  const valid = results(plan);
  execFileSync(process.execPath, [script, 'gate'], { cwd: repo.root,
    env: { ...env, CI_NEEDS_JSON: JSON.stringify(valid) }, stdio: ['ignore', 'pipe', 'pipe'] });
  for (const job of ['network', 'applications', 'database', 'validation']) {
    for (const result of [job === 'validation' ? 'skipped' : 'success', 'failure', 'cancelled', undefined]) {
      const invalid = structuredClone(valid);
      invalid[job].result = result;
      assert.throws(() => validateGate(invalid), `${job}/${result}`);
    }
  }
});

test('a merge queue candidate includes earlier queued code even when the last PR changes only documentation', t => {
  const repo = repository(t);
  const base = addSite(repo);
  repo.git('checkout', '-b', 'queued-code');
  repo.write('src/main.cpp', 'int main() { return 1; }\n');
  repo.commit();
  repo.git('checkout', '-b', 'queued-docs', base);
  repo.write('docs/guide.md', '# Queued documentation\n');
  const docsHead = repo.commit();
  repo.git('checkout', '-b', 'queue-candidate', base);
  for (const branch of ['queued-code', 'queued-docs']) {
    repo.git('-c', 'commit.gpgsign=false', 'merge', '--no-ff', branch, '-m', 'Merge queue fixture');
  }
  const head = repo.git('rev-parse', 'HEAD');
  const sourcePlan = classifyEvent(repo, 'pull_request', { pull_request: { base: { sha: base }, head: { sha: docsHead } } }).plan;
  assert.equal(sourcePlan.native_check, false);
  const { plan } = classifyEvent(repo, 'merge_group', { merge_group: { base_sha: base, head_sha: head } });
  assert.deepEqual(changedPaths({ root: repo.root, base, head, pullRequest: false }), ['docs/guide.md', 'src/main.cpp']);
  assert.equal(plan.native_check, true);
  assert.equal(plan.files_known, true);
  assert.equal(plan.reason, 'code-build-or-unknown-path');
  validateGate(results(plan));
  for (const job of ['network', 'applications', 'database']) {
    const skipped = results(plan);
    skipped[job].result = 'skipped';
    assert.throws(() => validateGate(skipped));
  }
});

test('merge group enumeration compares its exact base and head rather than a PR merge base', t => {
  const repo = repository(t);
  const ancestor = addSite(repo);
  repo.git('checkout', '-b', 'docs-topic');
  repo.write('docs/guide.md', '# Topic documentation\n');
  const head = repo.commit();
  repo.git('checkout', 'main');
  repo.write('src/main.cpp', 'int main() { return 2; }\n');
  const base = repo.commit();
  assert.equal(repo.git('merge-base', base, head), ancestor);
  assert.deepEqual(changedPaths({ root: repo.root, base, head }), ['docs/guide.md']);
  const { plan } = classifyEvent(repo, 'merge_group', { merge_group: { base_sha: base, head_sha: head } });
  assert.equal(plan.files_known, true);
  assert.equal(plan.native_check, true);
  assert.equal(plan.file_count, 2);
  assert.equal(plan.reason, 'code-build-or-unknown-path');
});

test('merge queue documentation renames cannot hide a removed code path', t => {
  const repo = repository(t);
  const base = addSite(repo);
  renameSync(resolve(repo.root, 'src/main.cpp'), resolve(repo.root, 'docs/moved.md'));
  const head = repo.commit();
  const { plan } = classifyEvent(repo, 'merge_group', { merge_group: { base_sha: base, head_sha: head } });
  assert.equal(plan.files_known, true);
  assert.equal(plan.native_check, true);
  assert.equal(plan.file_count, 2);
});

test('missing, invalid and unavailable merge group objects fall back to full checks', t => {
  const repo = repository(t);
  const base = addSite(repo);
  repo.write('docs/guide.md', '# Candidate documentation\n');
  const head = repo.commit();
  for (const payload of [{}, { merge_group: { base_sha: base } },
    { merge_group: { base_sha: base, head_sha: '0'.repeat(40) } },
    { merge_group: { base_sha: base, head_sha: 'f'.repeat(40) } },
    { merge_group: { base_sha: '--help', head_sha: head } }, '{invalid JSON']) {
    const { plan, output } = classifyEvent(repo, 'merge_group', payload);
    assert.equal(plan.native_check, true);
    assert.equal(plan.docs_check, true);
    assert.equal(plan.files_known, false);
    assert.equal(plan.reason, 'enumeration-unavailable');
    assert.match(output, /native_check=true\n/);
    validateGate(results(plan));
  }
});

test('empty merge ranges and unavailable documentation sites never skip the native matrix', t => {
  const repo = repository(t);
  const base = addSite(repo);
  const empty = classifyEvent(repo, 'merge_group', { merge_group: { base_sha: base, head_sha: base } }).plan;
  assert.equal(empty.files_known, true);
  assert.equal(empty.native_check, true);
  assert.equal(empty.reason, 'empty-diff');
  repo.git('rm', 'docs/site/package-lock.json');
  const head = repo.commit();
  const unavailable = classifyEvent(repo, 'merge_group', { merge_group: { base_sha: base, head_sha: head } }).plan;
  assert.equal(unavailable.native_check, true);
  assert.equal(unavailable.docs_check, true);
  assert.equal(unavailable.site_available, false);
  assert.equal(unavailable.reason, 'site-unavailable');
  const skippedSite = results(unavailable);
  skippedSite.validation.outputs.documentation_outcome = 'skipped';
  assert.throws(() => validateGate(skippedSite));
});

test('a known README-only merge candidate requires both site and README validation', t => {
  const repo = repository(t);
  const base = addSite(repo);
  repo.write('README.md', '# Candidate README\n');
  const head = repo.commit();
  const { plan, output } = classifyEvent(repo, 'merge_group', { merge_group: { base_sha: base, head_sha: head } });
  assert.equal(plan.native_check, false);
  assert.equal(plan.docs_check, true);
  assert.equal(plan.readme_check, true);
  assert.equal(plan.files_known, true);
  assert.match(output, /readme_check=true\n/);
  validateGate(results(plan));
  const skippedReadme = results(plan);
  skippedReadme.validation.outputs.readme_outcome = 'skipped';
  assert.throws(() => validateGate(skippedReadme));
});

test('merge candidates with build, dependency, CI, unknown or embedded code paths require native validation', () => {
  for (const file of ['CMakeLists.txt', 'docs/CMakeLists.txt', 'docs/example.cpp', 'docs/build.cmake',
    'vcpkg.json', 'docs/site/package-lock.json', '.github/scripts/ci-policy.mjs', 'tests/ci/policy.test.mjs',
    'docs/unknown.bin', 'scripts/format.sh']) {
    const plan = decision(['docs/guide.md', file], { eventName: 'merge_group' });
    assert.equal(plan.native_check, true, file);
    assert.equal(plan.docs_check, true, file);
    assert.equal(plan.reason, 'code-build-or-unknown-path', file);
  }
});

test('a real shallow candidate checkout cannot grant documentation-only coverage', t => {
  const repo = repository(t);
  const base = addSite(repo);
  repo.write('docs/guide.md', '# Candidate documentation\n');
  const head = repo.commit();
  const shallow = mkdtempSync(resolve(tmpdir(), 'ark-ci-shallow-'));
  t.after(() => rmSync(shallow, { recursive: true, force: true }));
  execFileSync('git', ['clone', '--depth', '1', pathToFileURL(repo.root).href, shallow],
    { stdio: ['ignore', 'pipe', 'pipe'] });
  const { plan } = classifyEvent({ root: shallow }, 'merge_group', { merge_group: { base_sha: base, head_sha: head } });
  assert.equal(plan.native_check, true);
  assert.equal(plan.docs_check, true);
  assert.equal(plan.files_known, false);
  assert.equal(plan.reason, 'enumeration-unavailable');
  validateGate(results(plan));
});

test('the required gate accepts only the exact full or explicitly lightweight job plan', () => {
  for (const plan of [decision(['README.md']), decision(['docs/guide.md']), decision(['src/main.cpp']),
    decision(['src/main.cpp'], { site: { present: false, available: false } }),
    decision(undefined, { enumerationFailed: true })]) validateGate(results(plan));
});

test('every required/optional job rejects all unexpected success, skipped, failure, cancellation and missing states', () => {
  for (const plan of [decision(['README.md']), decision(['src/main.cpp']),
    decision(['docs/guide.md'], { eventName: 'merge_group' }), decision(['README.md'], { eventName: 'merge_group' })]) {
    const valid = results(plan);
    for (const job of ['network', 'applications', 'database', 'validation']) {
      for (const state of ['success', 'skipped', 'failure', 'cancelled', 'neutral', 'timed_out', '', undefined]) {
        if (state === valid[job].result) continue;
        const invalid = structuredClone(valid);
        invalid[job].result = state;
        assert.throws(() => validateGate(invalid), `${job}/${state}`);
      }
      const absent = structuredClone(valid);
      delete absent[job];
      assert.throws(() => validateGate(absent));
    }
  }
});

test('missing classifier results, non-string booleans and contradictory skip plans fail closed', () => {
  const valid = results(decision(['README.md']));
  for (const result of ['failure', 'cancelled', 'skipped', '', undefined]) {
    const invalid = structuredClone(valid);
    invalid.scope.result = result;
    assert.throws(() => validateGate(invalid));
  }
  for (const key of ['native_check', 'network_check', 'applications_check', 'database_check', 'format_check',
    'docs_check', 'readme_check', 'site_available', 'files_known']) {
    for (const value of [undefined, null, '', 'TRUE', 'False', '0', '1', true, false]) {
      const invalid = structuredClone(valid);
      invalid.scope.outputs[key] = value;
      assert.throws(() => validateGate(invalid), `${key}/${value}`);
    }
  }
  for (const key of ['docs_check', 'site_available', 'files_known']) {
    const invalid = structuredClone(valid);
    invalid.scope.outputs[key] = 'false';
    assert.throws(() => validateGate(invalid));
  }
  assert.throws(() => validateGate({}));
});
