import { spawnSync } from 'node:child_process';
import { appendFileSync, lstatSync, mkdirSync } from 'node:fs';
import { posix, resolve, win32 } from 'node:path';
import { pathToFileURL } from 'node:url';

export const sccacheVersion = '0.16.0';

export function prepareCompilerCache({ env, run = spawnSync, start = true }) {
  if (!env.GITHUB_ENV || !env.GITHUB_OUTPUT) throw new Error('GitHub file-command channels are required');
  // The dependency step already fingerprints the runner, profile and dependency inputs.
  const identity = env.COMPILER_CACHE_IDENTITY ?? '';
  const validIdentity = /^ark-vcpkg-files-v2-(Linux|macOS|Windows)-(X64|ARM64|ARM)-[a-f0-9]{64}-[a-f0-9]{64}$/.test(identity) &&
    identity.startsWith(`ark-vcpkg-files-v2-${env.RUNNER_OS}-${env.RUNNER_ARCH}-`);
  const validSource = /^[a-f0-9]{40}$/.test(env.GITHUB_SHA ?? '');
  const paths = env.RUNNER_OS === 'Windows' ? win32 : posix;
  const compatible = validIdentity && validSource && env.RUNNER_OS !== 'Windows' &&
    paths.isAbsolute(env.RUNNER_TEMP ?? '') && !/[\x00-\x1f\x7f;,`*?\[\]{}!]/.test(env.RUNNER_TEMP ?? '');
  const directory = compatible ? paths.join(env.RUNNER_TEMP, 'ark-sccache') : '';
  const key = compatible ? identity.replace('vcpkg-files-v2', `sccache-${sccacheVersion}-v1`) : '';
  // Immutable snapshots can grow across source revisions without invalidating object compatibility.
  const selection = { archiveKey: key ? `${key}-${env.GITHUB_SHA}` : '', restoreKeys: key ? `${key}-` : '' };
  let enabled = false;
  if (directory && (!start || env.SCCACHE_RESTORE_OUTCOME === 'success') &&
      env.SCCACHE_SETUP_OUTCOME === 'success' && env.SCCACHE_PATH &&
      paths.isAbsolute(env.SCCACHE_PATH) && !/[\x00-\x1f\x7f;]/.test(env.SCCACHE_PATH)) {
    try {
      mkdirSync(directory, { recursive: true });
      if (!lstatSync(directory).isDirectory()) throw new Error('cache path is not a directory');
      const cacheEnv = { ...env, SCCACHE_DIR: directory, SCCACHE_GHA_ENABLED: 'false',
        SCCACHE_IGNORE_SERVER_IO_ERROR: '1', SCCACHE_CACHE_SIZE: '512M', SCCACHE_C_CUSTOM_CACHE_BUSTER: key };
      const options = { env: cacheEnv, encoding: 'utf8', timeout: 30_000 };
      const version = run(env.SCCACHE_PATH, ['--version'], options);
      if (version.status === 0 && version.stdout.trim() === `sccache ${sccacheVersion}`) {
        enabled = !start || run(env.SCCACHE_PATH, ['--start-server'], options).status === 0;
      }
    } catch {
      // Optional cache setup must not prevent a normal compiler invocation.
    }
  }
  const launcher = enabled ? env.SCCACHE_PATH.replaceAll('\\', '/') : '';
  appendFileSync(env.GITHUB_ENV, `ARK_COMPILER_LAUNCHER=${launcher}\nSCCACHE_DIR=${enabled ? directory : ''}\nSCCACHE_GHA_ENABLED=false\nSCCACHE_IGNORE_SERVER_IO_ERROR=1\nSCCACHE_CACHE_SIZE=512M\nSCCACHE_C_CUSTOM_CACHE_BUSTER=${key}\n`);
  appendFileSync(env.GITHUB_OUTPUT, `enabled=${enabled}\ndirectory=${enabled ? directory : ''}\nkey=${enabled ? key : ''}\narchive_key=${enabled ? selection.archiveKey : ''}\nrestore_keys=${enabled ? selection.restoreKeys : ''}\n`);
  return { enabled, directory, key, launcher, ...selection };
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  try {
    const plan = prepareCompilerCache({ env: process.env, start: !process.argv.includes('--prepare') });
    console.log(plan.enabled ? `sccache ${sccacheVersion} compiler launcher prepared` : 'Compiler cache unavailable; using the compiler directly');
  } catch {
    console.error('Unable to export compiler cache configuration to GitHub file-command channels');
    process.exitCode = 1;
  }
}
