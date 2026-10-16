import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, readFileSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';
import { cachePlan, prepareCache } from '../../.github/scripts/vcpkg-cache.mjs';

const script = fileURLToPath(new URL('../../.github/scripts/vcpkg-cache.mjs', import.meta.url));
const revision = '0699a19d0c6386247ce50d4dedbb8217d484d536';

function fixture(t) {
  const root = mkdtempSync(resolve(tmpdir(), 'ark-vcpkg-cache-'));
  t.after(() => rmSync(root, { recursive: true, force: true }));
  const write = (name, value) => {
    const file = resolve(root, name);
    mkdirSync(dirname(file), { recursive: true });
    writeFileSync(file, value);
  };
  write('vcpkg.json', JSON.stringify({ 'builtin-baseline': revision, dependencies: ['asio'] }));
  write('vcpkg-configuration.json', '{"overlay-ports":["ports"]}');
  write('ports/asio/portfile.cmake', 'vcpkg_from_github()\n');
  write('ports/asio/vcpkg.json', '{"name":"asio","version":"1.38.2"}');
  write('environment', 'EXISTING_VALUE=preserved\n');
  write('output', 'existing_output=preserved\n');
  const env = {
    RUNNER_OS: 'Linux', RUNNER_ARCH: 'X64', RUNNER_TEMP: resolve(root, 'runner temp'),
    ImageOS: 'ubuntu24', ImageVersion: '20261014.1',
    VCPKG_CACHE_REVISION: revision, VCPKG_CACHE_FEATURES: 'boost',
    VCPKG_CACHE_DEFAULT_FEATURES: 'true', VCPKG_CACHE_PROFILE: 'network-none',
    GITHUB_ENV: resolve(root, 'environment'), GITHUB_OUTPUT: resolve(root, 'output'),
    VCPKG_BINARY_SOURCES: 'clear;x-gha,readwrite',
  };
  return { root, env, write, plan: extra => cachePlan({ root, env: { ...env, ...extra } }) };
}

test('Unix and Windows plans use only the runner temporary archives directory', t => {
  const f = fixture(t);
  for (const [os, temp, expected] of [
    ['Linux', '/home/runner/work/_temp', '/home/runner/work/_temp/ark-vcpkg-archives'],
    ['macOS', '/Users/runner/work/_temp with spaces', '/Users/runner/work/_temp with spaces/ark-vcpkg-archives'],
    ['Windows', 'D:\\a\\_temp', 'D:\\a\\_temp\\ark-vcpkg-archives'],
    ['Windows', 'D:/a/_temp', 'D:\\a\\_temp\\ark-vcpkg-archives'],
  ]) {
    const plan = f.plan({ RUNNER_OS: os, RUNNER_TEMP: temp });
    assert.equal(plan.enabled, true);
    assert.equal(plan.directory, expected);
    assert.equal(plan.binarySources, `clear;files,${expected},readwrite`);
    assert.match(plan.key, /^ark-vcpkg-files-v1-[A-Za-z]+-[A-Z0-9]+-[a-f0-9]{64}$/);
    assert.ok(plan.key.length < 512);
  }
});

test('unsafe path separators, globs, control characters and relative paths explicitly stay cold', t => {
  const f = fixture(t);
  for (const path of ['/tmp/a,b', '/tmp/a;b', '/tmp/a`b', '/tmp/a\nb', '/tmp/a\rb', '/tmp/a\0b',
    '/tmp/a*b', '/tmp/a?b', '/tmp/a[b]', '/tmp/a{b}', '/tmp/a!b', 'relative/temp']) {
    const plan = f.plan({ RUNNER_TEMP: path });
    assert.equal(plan.enabled, false, path);
    assert.equal(plan.binarySources, 'clear', path);
  }
  for (const path of ['D:relative', '\\temp', '\\\\server\\share\\temp']) {
    assert.equal(f.plan({ RUNNER_OS: 'Windows', RUNNER_TEMP: path }).binarySources, 'clear');
  }
});

test('missing runner, image and dependency identity cannot enable a cache with an incomplete key', t => {
  const f = fixture(t);
  for (const name of ['RUNNER_OS', 'RUNNER_ARCH', 'RUNNER_TEMP', 'ImageOS', 'ImageVersion',
    'VCPKG_CACHE_REVISION', 'VCPKG_CACHE_FEATURES', 'VCPKG_CACHE_DEFAULT_FEATURES', 'VCPKG_CACHE_PROFILE']) {
    for (const value of [undefined, '', '\n']) {
      const plan = f.plan({ [name]: value });
      assert.equal(plan.enabled, false, name);
      assert.equal(plan.binarySources, 'clear', name);
      assert.equal(plan.key, '');
    }
  }
  for (const extra of [{ RUNNER_OS: 'unknown' }, { RUNNER_ARCH: 'unknown' },
    { VCPKG_CACHE_REVISION: 'main' }, { VCPKG_CACHE_DEFAULT_FEATURES: 'yes' },
    { VCPKG_CACHE_FEATURES: 'boost;../bad' }, { VCPKG_CACHE_FEATURES: 'boost;' }]) {
    assert.equal(f.plan(extra).binarySources, 'clear');
  }
});

test('keys are stable across workspaces, feature ordering and existing provider settings', t => {
  const f = fixture(t);
  const g = fixture(t);
  assert.equal(f.plan().key, g.plan().key);
  assert.equal(f.plan({ VCPKG_CACHE_FEATURES: 'database;boost;boost' }).key,
    f.plan({ VCPKG_CACHE_FEATURES: 'boost;database' }).key);
  assert.equal(f.plan({ VCPKG_BINARY_SOURCES: 'some previous provider' }).key, f.plan().key);
  assert.equal(f.plan({ RUNNER_TEMP: '/different/temporary/location' }).key, f.plan().key);
});

test('runner/compiler, pinned revision, manifest features and build profile changes invalidate the key', t => {
  const f = fixture(t);
  for (const extra of [{ RUNNER_OS: 'macOS' }, { RUNNER_ARCH: 'ARM64' }, { ImageOS: 'ubuntu26' },
    { ImageVersion: '20261015.2' }, { VCPKG_CACHE_REVISION: 'a'.repeat(40) },
    { VCPKG_CACHE_FEATURES: 'application;boost' }, { VCPKG_CACHE_DEFAULT_FEATURES: 'false' },
    { VCPKG_CACHE_PROFILE: 'network-address' }, { CC: '/other/cc' }, { CXX: '/other/cxx' },
    { CFLAGS: '-O2' }, { CXXFLAGS: '-O3' }, { LDFLAGS: '-flto' },
    { VCPKG_DEFAULT_TRIPLET: 'arm64-linux' }, { VCPKG_DEFAULT_HOST_TRIPLET: 'arm64-linux' },
    { VCPKG_TARGET_TRIPLET: 'arm64-linux' }, { VCPKG_HOST_TRIPLET: 'arm64-linux' }]) {
    const plan = f.plan(extra);
    assert.equal(plan.enabled, true);
    assert.notEqual(plan.key, f.plan().key, JSON.stringify(extra));
  }
});

test('manifest, configuration and local overlay content all affect the key', t => {
  const f = fixture(t);
  for (const [file, value] of [
    ['vcpkg.json', '{"dependencies":["asio","zlib"]}'],
    ['vcpkg-configuration.json', '{ "overlay-ports": ["ports"] }'],
    ['ports/asio/portfile.cmake', '# updated recipe\n'],
    ['ports/asio/vcpkg.json', '{"name":"asio","version":"1.39.0"}'],
    ['ports/extra/portfile.cmake', '# newly added port\n'],
  ]) {
    const before = f.plan().key;
    f.write(file, value);
    assert.notEqual(f.plan().key, before, file);
  }
});

test('missing/invalid dependency files or unsupported external overlay layouts stay cold', t => {
  const f = fixture(t);
  rmSync(resolve(f.root, 'vcpkg.json'));
  assert.equal(f.plan().binarySources, 'clear');
  f.write('vcpkg.json', '{}');
  for (const configuration of ['not json', '{"overlay-ports":["../external"]}',
    '{"overlay-ports":["ports"],"overlay-triplets":["triplets"]}',
    '{"overlay-ports":["ports"],"registries":[]}',
    '{"overlay-ports":["ports"],"default-registry":{"kind":"filesystem"}}']) {
    f.write('vcpkg-configuration.json', configuration);
    assert.equal(f.plan().binarySources, 'clear');
  }
});

test('overlay symlinks are not followed while forming cache keys', { skip: process.platform === 'win32' }, t => {
  const f = fixture(t);
  symlinkSync(resolve(f.root, 'environment'), resolve(f.root, 'ports/asio/link'));
  assert.equal(f.plan().binarySources, 'clear');
  rmSync(resolve(f.root, 'ports'), { recursive: true });
  mkdirSync(resolve(f.root, 'external'));
  symlinkSync(resolve(f.root, 'external'), resolve(f.root, 'ports'));
  assert.equal(f.plan().binarySources, 'clear');
});

test('prepare exports a nonempty files provider before the action and preserves unrelated file commands', t => {
  const f = fixture(t);
  const plan = prepareCache(f);
  assert.equal(plan.enabled, true);
  const environment = readFileSync(f.env.GITHUB_ENV, 'utf8');
  assert.equal(environment, `EXISTING_VALUE=preserved\nVCPKG_BINARY_SOURCES=${plan.binarySources}\n`);
  assert.equal(readFileSync(f.env.GITHUB_OUTPUT, 'utf8'),
    `existing_output=preserved\nenabled=true\ndirectory=${plan.directory}\nkey=${plan.key}\n`);
  assert.doesNotMatch(environment, /x-gha|nuget/);
  assert.equal(f.env.VCPKG_BINARY_SOURCES, 'clear;x-gha,readwrite'); // Only later GitHub steps receive file commands.
});

test('cold fallback still explicitly exports clear instead of leaving the action default active', t => {
  for (const extra of [{ ImageVersion: '' }, { RUNNER_TEMP: '/tmp/separator,unsafe' }]) {
    const f = fixture(t);
    const plan = prepareCache({ root: f.root, env: { ...f.env, ...extra } });
    assert.equal(plan.enabled, false);
    assert.match(readFileSync(f.env.GITHUB_ENV, 'utf8'), /\nVCPKG_BINARY_SOURCES=clear\n$/);
    assert.match(readFileSync(f.env.GITHUB_OUTPUT, 'utf8'), /\nenabled=false\ndirectory=\nkey=\n$/);
  }
});

test('unavailable archive storage falls back to source builds without enabling persistence', t => {
  const f = fixture(t);
  f.write('runner temp', 'not a directory');
  const plan = prepareCache(f);
  assert.equal(plan.enabled, false);
  assert.equal(plan.reason, 'archive directory unavailable');
  assert.match(readFileSync(f.env.GITHUB_ENV, 'utf8'), /VCPKG_BINARY_SOURCES=clear/);
});

test('archive directory symlinks disable persistence', { skip: process.platform === 'win32' }, t => {
  const f = fixture(t);
  mkdirSync(f.env.RUNNER_TEMP);
  symlinkSync(f.root, resolve(f.env.RUNNER_TEMP, 'ark-vcpkg-archives'));
  assert.equal(prepareCache(f).binarySources, 'clear');
});

test('missing file-command channels fail explicitly rather than pretending to configure later steps', t => {
  const f = fixture(t);
  for (const name of ['GITHUB_ENV', 'GITHUB_OUTPUT']) {
    assert.throws(() => prepareCache({ root: f.root, env: { ...f.env, [name]: undefined } }), /file-command/);
  }
  assert.equal(readFileSync(f.env.GITHUB_ENV, 'utf8'), 'EXISTING_VALUE=preserved\n');
});

test('standalone CLI exports working/cold plans and does not print arbitrary inherited environment values', t => {
  const f = fixture(t);
  for (const image of ['20261014.1', '']) {
    const child = spawnSync(process.execPath, [script], {
      cwd: f.root, env: { ...f.env, ImageVersion: image, UNRELATED_SECRET: 'do-not-display-this-value' }, encoding: 'utf8',
    });
    assert.equal(child.status, 0, child.stderr);
    assert.match(child.stdout, image ? /cache prepared/ : /cache disabled/);
    assert.doesNotMatch(child.stdout + child.stderr, /do-not-display-this-value/);
  }
  const child = spawnSync(process.execPath, [script], { cwd: f.root, env: {}, encoding: 'utf8' });
  assert.equal(child.status, 1);
  assert.match(child.stderr, /file-command/);
});
