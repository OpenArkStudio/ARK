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
test('Applications CI compiles the optional entity benchmark and runs only its correctness smoke', () => {
  const workflow = read('.github/workflows/ci.yml').replaceAll('\r\n', '\n');
  const apps = workflow.split(/(?=^  [a-z][a-z0-9_-]*:$)/m).find(block => block.startsWith('  applications:\n'));
  assert.ok(apps);
  const configure = apps.match(/      - name: Configure applications[\s\S]*?(?=\n      - )/)?.[0];
  assert.match(configure ?? '', /(?:^|\s)-DARK_BUILD_BENCHMARKS=ON(?:\s|$)/);
  const steps = apps.split(/(?=^      - )/m).slice(1);
  const smoke = steps.filter(step => step.includes('run: cmake --build ci-build --target check-entity-benchmark-smoke'));
  assert.equal(smoke.length, 1);
  assert.doesNotMatch(smoke[0], /continue-on-error:|if:|--iterations/);
});
