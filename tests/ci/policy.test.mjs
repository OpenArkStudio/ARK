import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, readFileSync, renameSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';
import { changedPaths, decodePaths, inspectSite, isDocumentationPath, selectScope, validateGate } from '../../.github/scripts/ci-policy.mjs';

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

test('only recognized documentation paths qualify for the lightweight plan', () => {
  for (const file of ['README.md', 'README_CN.md', 'docs/guide.md', 'docs/site/src/Header.astro', 'docs/assets/chart.svg']) {
    assert.equal(isDocumentationPath(file), true, file);
    assert.equal(decision([file]).full_cpp, false, file);
  }
  assert.equal(decision(['README.md']).readme_check, true);
  assert.equal(decision(['docs/guide.md']).readme_check, false);
  assert.equal(decision(['README.md', 'docs/guide.md']).docs_check, true);
});

test('code, build, dependency, CI and unknown paths force the entire C++ matrix', () => {
  const paths = ['include/ark/xml.hpp', 'CMakeLists.txt', 'cmake/FindZstd.cmake', 'vcpkg.json', '.gitmodules',
    '.github/workflows/ci.yml', '.github/scripts/ci-policy.mjs', 'tests/core/templates.cpp', 'scripts/format.sh',
    'LICENSE', 'README_CN.md.bak', 'docs/example.cpp', 'docs/example.h', 'docs/message.proto', 'docs/CMakeLists.txt',
    'docs/build.cmake', 'docs/CMakePresets.json', 'docs/CMakeUserPresets.json', 'docs/CMakeCache.txt',
    'docs/action.yml', 'docs/site/package.json', 'docs/site/package-lock.json', 'docs/unknown.bin',
    'docs/.github/workflows/example.yml', 'docs/requirements.txt', 'docs/../README.md', './docs/guide.md', 'docs//guide.md'];
  for (const file of paths) {
    // requirements.txt is a dependency file even though ordinary text is documentation.
    assert.equal(decision(['docs/guide.md', file]).full_cpp, true, file);
  }
});

test('a code path beyond 3000 documentation entries cannot be truncated out of the decision', () => {
  const files = Array.from({ length: 3001 }, (_, index) => `docs/page-${index}.md`);
  files.push('src/last.cpp');
  const decoded = decodePaths(Buffer.from(files.join('\0') + '\0'));
  assert.equal(decoded.length, 3002);
  assert.equal(decision(decoded).full_cpp, true);
});

test('empty, missing, truncated and invalid UTF-8 path lists never grant a lightweight result', () => {
  assert.equal(decision([]).full_cpp, true);
  assert.equal(decision(undefined, { enumerationFailed: true }).full_cpp, true);
  assert.throws(() => decodePaths(Buffer.from('docs/guide.md')));
  assert.throws(() => decodePaths(Buffer.from([0xff, 0])));
  assert.throws(() => decodePaths(Buffer.from('docs/a.md\0docs/a.md\0')));
  assert.throws(() => decodePaths(Buffer.from('\0')));
  assert.deepEqual(decodePaths(Buffer.from('docs/a.md\0README.md\0')), ['docs/a.md', 'README.md']);
  assert.equal(decision(['docs/a\nname.md']).full_cpp, true);
});

test('manual, main pushes and unsupported events retain full C++ coverage', () => {
  for (const eventName of ['push', 'workflow_dispatch', 'merge_group', 'unknown']) {
    const plan = decision(['docs/guide.md'], { eventName });
    assert.equal(plan.full_cpp, true);
    assert.equal(plan.docs_check, true);
  }
});

test('a missing legacy site forces full C++ and does not falsely report a site check', () => {
  const plan = decision(['README.md'], { site: { present: false, available: false } });
  assert.equal(plan.full_cpp, true);
  assert.equal(plan.docs_check, false);
  assert.equal(plan.readme_check, true);
  assert.equal(decision(['src/main.cpp'], { site: { present: false, available: false } }).readme_check, false);
});

test('CI policy changes also validate the site when it has been introduced', () => {
  const plan = decision(['.github/workflows/ci.yml', 'tests/ci/policy.test.mjs']);
  assert.equal(plan.full_cpp, true);
  assert.equal(plan.docs_check, true);
  assert.equal(plan.readme_check, false);
});

test('an introduced, incomplete or removed site requires a failing preflight instead of a silent skip', () => {
  const incomplete = { present: true, available: false };
  assert.equal(decision(['docs/guide.md'], { site: incomplete }).docs_check, true);
  const removed = decision(['docs/site/package-lock.json'], { site: { present: false, available: false } });
  assert.equal(removed.full_cpp, true);
  assert.equal(removed.docs_check, true);
  assert.equal(decision(['.github/workflows/docs.yml', 'docs/site/package-lock.json']).full_cpp, true);
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
  assert.equal(decision(files).full_cpp, true);
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
  assert.equal(decision(changedPaths({ root: repo.root, base, head })).full_cpp, false);
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
  assert.equal(result.full_cpp, false);
  assert.equal(result.docs_check, true);
  assert.equal(result.readme_check, false);
  assert.match(readFileSync(env.GITHUB_OUTPUT, 'utf8'), /full_cpp=false\n/);
});

function results(plan) {
  return {
    scope: { result: 'success', outputs: Object.fromEntries(Object.entries(plan).map(([key, value]) => [key, String(value)])) },
    network: { result: plan.full_cpp ? 'success' : 'skipped' },
    applications: { result: plan.full_cpp ? 'success' : 'skipped' },
    database: { result: plan.full_cpp ? 'success' : 'skipped' },
    documentation: { result: plan.docs_check ? 'success' : 'skipped' },
    readme: { result: plan.readme_check ? 'success' : 'skipped' },
  };
}

test('the required gate accepts only the exact full or explicitly lightweight job plan', () => {
  for (const plan of [decision(['README.md']), decision(['docs/guide.md']), decision(['src/main.cpp']),
    decision(['src/main.cpp'], { site: { present: false, available: false } }),
    decision(undefined, { enumerationFailed: true })]) validateGate(results(plan));
});

test('every required/optional job rejects all unexpected success, skipped, failure, cancellation and missing states', () => {
  for (const plan of [decision(['README.md']), decision(['src/main.cpp'])]) {
    const valid = results(plan);
    for (const job of ['network', 'applications', 'database', 'documentation', 'readme']) {
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
  for (const key of ['full_cpp', 'docs_check', 'readme_check', 'site_available', 'files_known']) {
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
