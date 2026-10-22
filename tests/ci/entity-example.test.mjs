// Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import test from 'node:test';
const read = path => readFileSync(new URL(`../../${path}`, import.meta.url), 'utf8');

test('entity benchmark stays optional with an independent CLI smoke entry', () => {
  const cmake = read('benchmarks/CMakeLists.txt');
  assert.match(cmake, /if\(ARK_BUILD_BENCHMARKS AND ARK_BUILD_CONFIG AND TARGET ark_kernel_plugin AND TARGET ark_utility_plugin\)/);
  assert.match(cmake, /add_executable\(ark_entity_operations_benchmark EXCLUDE_FROM_ALL/);
  assert.doesNotMatch(read('tests/core/CMakeLists.txt'), /check-entity-benchmark-smoke|ark_entity_operations_benchmark/);
  assert.match(cmake, /add_custom_target\(check-entity-benchmark-smoke[\s\S]*?-P "\$\{PROJECT_SOURCE_DIR\}\/benchmarks\/entity_operations_smoke\.cmake"/);
  assert.match(read('benchmarks/entity_operations_smoke.cmake'), /COMMAND "\$\{PROGRAM\}" --plugin-dir "\$\{PLUGIN_DIR\}" --smoke/);
});
