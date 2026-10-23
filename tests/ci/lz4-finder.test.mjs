// Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

const finder = fileURLToPath(new URL('../../cmake/FindLZ4.cmake', import.meta.url)).replaceAll('\\', '/');

function selectLibrary(t, { configuration, preferredName, fallbackName, preferredDirectory = 'debug' }) {
  const root = mkdtempSync(join(tmpdir(), 'ark-lz4-finder-'));
  t.after(() => rmSync(root, { recursive: true, force: true }));
  const preferred = join(root, preferredDirectory);
  const fallback = join(root, 'fallback');
  for (const [prefix, name] of [[preferred, preferredName], [fallback, fallbackName]]) {
    mkdirSync(join(prefix, 'lib'), { recursive: true });
    mkdirSync(join(prefix, 'include'));
    writeFileSync(join(prefix, 'lib', name), 'fake import library; must not be linked\n');
    writeFileSync(join(prefix, 'include', 'lz4.h'), '/* finder-only fixture */\n');
  }
  writeFileSync(join(root, 'CMakeLists.txt'), `cmake_minimum_required(VERSION 3.21)
project(lz4_finder NONE)
# Fake Windows import libraries: no compiler or ABI/runtime validation.
set(CMAKE_FIND_LIBRARY_PREFIXES "")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH FALSE)
set(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)
set(CMAKE_FIND_USE_INSTALL_PREFIX FALSE)
# The real version probe compiles C; fake files test only search selection.
set(LZ4_GOOD_VERSION TRUE CACHE BOOL "Skip fake-library compilation")
include("${finder}")
if(NOT LZ4_FOUND)
  message(FATAL_ERROR "Fixture library was not found")
endif()
file(WRITE "\${CMAKE_BINARY_DIR}/selected.txt" "\${LZ4_LIBRARY}")
`);
  const result = spawnSync('cmake', ['-S', root, '-B', join(root, 'build'),
    '-DCMAKE_SYSTEM_NAME=Windows', `-DCMAKE_BUILD_TYPE=${configuration}`,
    `-DCMAKE_PREFIX_PATH=${preferred};${fallback}`], { encoding: 'utf8', timeout: 30_000 });
  assert.equal(result.status, 0, result.stdout + result.stderr);
  assert.equal(readFileSync(join(root, 'build', 'selected.txt'), 'utf8'),
    join(preferred, 'lib', preferredName).replaceAll('\\', '/'));
}

test('Debug selects suffixed lz4d before a later release lz4 import library', t => {
  selectLibrary(t, { configuration: 'Debug', preferredName: 'lz4d.lib', fallbackName: 'lz4.lib' });
});

test('Debug retains configuration precedence when both libraries have the lz4 basename', t => {
  selectLibrary(t, { configuration: 'Debug', preferredName: 'lz4.lib', fallbackName: 'lz4.lib' });
});

test('Release selects its lz4 import library before a later debug lz4d library', t => {
  selectLibrary(t, { configuration: 'Release', preferredDirectory: 'release', preferredName: 'lz4.lib', fallbackName: 'lz4d.lib' });
});

test('configured directory precedence outranks alternative library basename order', t => {
  selectLibrary(t, { configuration: 'Release', preferredDirectory: 'preferred prefix', preferredName: 'lz4d.lib', fallbackName: 'lz4.lib' });
});
