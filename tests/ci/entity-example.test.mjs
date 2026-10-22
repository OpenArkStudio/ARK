// Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import test from 'node:test';
const read = path => readFileSync(new URL(`../../${path}`, import.meta.url), 'utf8');

test('entity startup stays functional while timed loops require an explicit benchmark target', () => {
  const source = read('examples/entity_operations/entity_operations_module.cpp');
  assert.doesNotMatch(source, /5000000|system_clock|query_string_test|query_int_test|set_data_test/);
  assert.match(source, /Entity data demonstration: name=Xiaoming gender=11 custom=111/);
  const benchmark = read('benchmarks/entity_operations.cpp');
  assert.equal((benchmark.match(/for \(size_t i = 0; i < count; i\+\+\)/g) ?? []).length, 12);
  assert.match(benchmark, /timed_ \? std::chrono::system_clock::now/);
  assert.match(benchmark, /--iterations/);
  assert.match(benchmark, /timing disabled/);
  const cmake = read('benchmarks/CMakeLists.txt');
  assert.match(cmake, /if\(ARK_BUILD_BENCHMARKS AND ARK_BUILD_CONFIG AND TARGET ark_kernel_plugin AND TARGET ark_utility_plugin\)/);
  assert.match(cmake, /add_executable\(ark_entity_operations_benchmark EXCLUDE_FROM_ALL/);
  assert.doesNotMatch(read('tests/core/CMakeLists.txt'), /check-entity-benchmark-smoke|ark_entity_operations_benchmark/);
});

test('entity assembly preserves its visible module key, inherited update and explicit entry contract', () => {
  const header = read('examples/entity_operations/entity_operations_module.hpp');
  const plugin = read('examples/entity_operations/entity_operations_plugin.cpp');
  assert.match(header, /class entity_operations_module final : public module_base<module>/);
  assert.doesNotMatch(header, /bool update\(/);
  assert.doesNotMatch(plugin, /ARK_REGISTER_MODULE|ARK_UNREGISTER_MODULE|ARK_DECLARE_PLUGIN_DLL_FUNCTION/);
  assert.match(plugin, /ARK_EXPORT_FUNC void DllEntryPlugin/);
  assert.match(plugin, /ARK_EXPORT_FUNC void DllExitPlugin/);
  assert.match(plugin, /register_module\(make_entity_operations_module\(\), module_update::disabled\)/);
  assert.match(plugin, /unregister_module<entity_operations_module>\(\)/);
  const runtime = read('tests/app/typed_examples.cmake');
  assert.match(runtime, /foreach\(sample timers module_lifecycle entity_operations\)/);
  assert.match(runtime, /examples\/entity_operations\/resource/);
});
