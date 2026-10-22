import { createHash } from 'node:crypto';
import { appendFileSync, lstatSync, mkdirSync, readFileSync, readdirSync } from 'node:fs';
import { posix, resolve, win32 } from 'node:path';
import { pathToFileURL } from 'node:url';

const cold = reason => ({ enabled: false, reason, directory: '', key: '', restoreKey: '', binarySources: 'clear' });
const lineValue = value => typeof value === 'string' && value.trim() !== '' && !/[\x00-\x1f\x7f]/.test(value);

function dependencyFiles(root) {
  const files = ['vcpkg.json', 'vcpkg-configuration.json'];
  const configuration = JSON.parse(readFileSync(resolve(root, 'vcpkg-configuration.json'), 'utf8'));
  // This project has one local overlay. New dependency layouts need explicit cache coverage.
  if (JSON.stringify(configuration['overlay-ports']) !== '["ports"]' ||
      configuration['overlay-triplets']?.length || configuration.registries || configuration['default-registry']) {
    throw new Error('unsupported dependency layout');
  }
  if (!lstatSync(resolve(root, 'ports')).isDirectory()) throw new Error('unsupported overlay directory');
  const visit = directory => {
    for (const entry of readdirSync(resolve(root, directory), { withFileTypes: true })) {
      const file = posix.join(directory, entry.name);
      if (entry.isSymbolicLink()) throw new Error('symlink in dependency overlay');
      if (entry.isDirectory()) visit(file);
      else if (entry.isFile()) files.push(file);
      else throw new Error('unsupported dependency entry');
    }
  };
  visit('ports');
  return files.sort().map(file => [file, createHash('sha256').update(readFileSync(resolve(root, file))).digest('hex')]);
}

export function cachePlan({ root, env }) {
  const required = ['RUNNER_OS', 'RUNNER_ARCH', 'RUNNER_TEMP', 'ImageOS', 'ImageVersion',
    'VCPKG_CACHE_REVISION', 'VCPKG_CACHE_FEATURES', 'VCPKG_CACHE_DEFAULT_FEATURES', 'VCPKG_CACHE_PROFILE'];
  if (required.some(name => !lineValue(env[name]))) return cold('missing runner or cache identity');
  if (!['Linux', 'macOS', 'Windows'].includes(env.RUNNER_OS) || !['X64', 'ARM64', 'ARM'].includes(env.RUNNER_ARCH) ||
      !/^[a-f0-9]{40}$/.test(env.VCPKG_CACHE_REVISION) ||
      !['true', 'false'].includes(env.VCPKG_CACHE_DEFAULT_FEATURES)) return cold('invalid cache identity');

  const paths = env.RUNNER_OS === 'Windows' ? win32 : posix;
  // Both the vcpkg provider and Actions cache parse paths. Unsupported delimiters/globs stay cold.
  if (/[`;,*?\[\]{}!]/.test(env.RUNNER_TEMP) || !paths.isAbsolute(env.RUNNER_TEMP) ||
      (env.RUNNER_OS === 'Windows' && !/^[a-z]:[\\/]/i.test(env.RUNNER_TEMP))) return cold('unsupported archive path');
  const directory = paths.join(env.RUNNER_TEMP, 'ark-vcpkg-archives');
  const features = [...new Set(env.VCPKG_CACHE_FEATURES.split(';').map(value => value.trim()))].sort();
  if (features.some(value => !/^[a-z][a-z0-9-]*$/.test(value))) return cold('invalid manifest features');

  let dependencies;
  try {
    dependencies = dependencyFiles(root);
  } catch {
    return cold('dependency fingerprint unavailable');
  }
  const compiler = Object.fromEntries(['CC', 'CXX', 'CFLAGS', 'CXXFLAGS', 'LDFLAGS',
    'VCPKG_DEFAULT_TRIPLET', 'VCPKG_DEFAULT_HOST_TRIPLET', 'VCPKG_TARGET_TRIPLET', 'VCPKG_HOST_TRIPLET']
    .map(name => [name, env[name] ?? '']));
  const identity = {
    schema: 2, os: env.RUNNER_OS, arch: env.RUNNER_ARCH,
    imageOS: env.ImageOS, revision: env.VCPKG_CACHE_REVISION,
    features, defaults: env.VCPKG_CACHE_DEFAULT_FEATURES, profile: env.VCPKG_CACHE_PROFILE,
    compiler, dependencies,
  };
  const digest = createHash('sha256').update(JSON.stringify(identity)).digest('hex');
  const imageDigest = createHash('sha256').update(env.ImageVersion).digest('hex');
  // Restore only archives from the same dependency/build family. vcpkg still selects
  // each package by its full ABI hash; an image roll never authorizes an ABI bypass.
  const restoreKey = `ark-vcpkg-files-v2-${env.RUNNER_OS}-${env.RUNNER_ARCH}-${digest}-`;
  return {
    enabled: true, reason: '', directory,
    key: `${restoreKey}${imageDigest}`, restoreKey,
    binarySources: `clear;files,${directory},readwrite`,
  };
}

export function prepareCache({ root, env }) {
  // These file-command channels are required to configure later steps, even for a cold build.
  if (!lineValue(env.GITHUB_ENV) || !lineValue(env.GITHUB_OUTPUT)) throw new Error('GitHub file-command channels are required');
  let plan = cachePlan({ root, env });
  if (plan.enabled) {
    try {
      mkdirSync(plan.directory, { recursive: true });
      if (!lstatSync(plan.directory).isDirectory()) throw new Error('archive path is not a directory');
    } catch {
      plan = cold('archive directory unavailable');
    }
  }
  // run-vcpkg preserves an existing nonempty value. Always export clear on fallback to suppress x-gha.
  appendFileSync(env.GITHUB_ENV, `VCPKG_BINARY_SOURCES=${plan.binarySources}\n`, 'utf8');
  appendFileSync(env.GITHUB_OUTPUT,
    `enabled=${plan.enabled}\ndirectory=${plan.directory}\nkey=${plan.key}\nrestore-key=${plan.restoreKey}\n`, 'utf8');
  return plan;
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  try {
    const plan = prepareCache({ root: process.cwd(), env: process.env });
    console.log(plan.enabled ? 'vcpkg files archive cache prepared' : `vcpkg cache disabled: ${plan.reason}; building dependencies from source`);
  } catch {
    console.error('Unable to export vcpkg cache configuration to GitHub file-command channels');
    process.exitCode = 1;
  }
}
