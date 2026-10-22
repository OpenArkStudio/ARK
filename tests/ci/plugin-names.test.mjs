import assert from 'node:assert/strict';
import { readFileSync, readdirSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { resolve } from 'node:path';
import test from 'node:test';

const root = fileURLToPath(new URL('../../', import.meta.url));

function pluginDeclarations(directory) {
  const names = [];
  for (const entry of readdirSync(directory, { withFileTypes: true })) {
    const file = resolve(directory, entry.name);
    if (entry.isDirectory()) {
      names.push(...pluginDeclarations(file));
    } else if (entry.name === 'CMakeLists.txt') {
      const source = readFileSync(file, 'utf8').replace(/^\s*#.*$/gm, '');
      for (const match of source.matchAll(/\bBUILD_(?:PLUGIN|SERVER)_MACRO\(\s*"?([A-Za-z0-9_]+)/g)) {
        if (match[1] !== 'project_name') names.push({ name: match[1], file });
      }
    }
  }
  return names;
}

test('official, server and example shared plugin targets use snake_case library stems', () => {
  const declarations = ['src/plugins', 'src/server', 'examples'].flatMap(directory =>
    pluginDeclarations(resolve(root, directory)));
  assert.ok(declarations.length > 0, 'No plugin declarations were inspected');
  for (const { name, file } of declarations) {
    assert.match(name, /^[a-z][a-z0-9]*(_[a-z0-9]+)*$/, file);
  }
  const names = new Set(declarations.map(entry => entry.name));
  for (const expected of ['ark_mysql_plugin', 'ark_redis_plugin', 'ark_utility_plugin', 'timers_plugin']) {
    assert.ok(names.has(expected), `Missing declared target ${expected}`);
  }
});

test('capability examples preserve the enabled set and retain disabled examples without numbered paths', () => {
  const names = ['timers', 'module_lifecycle', 'entity_operations', 'physics_scene', 'http_server'];
  for (const entry of readdirSync(resolve(root, 'examples'))) assert.doesNotMatch(entry, /^sample[1-5]$/);
  const cmake = readFileSync(resolve(root, 'examples/CMakeLists.txt'), 'utf8');
  for (const [index, name] of names.entries()) {
    assert.match(cmake, new RegExp(`^${index < 3 ? '' : '#'}add_subdirectory\\(${name}\\)$`, 'm'));
    const header = readFileSync(resolve(root, `examples/${name}/${name}_module.hpp`), 'utf8');
    assert.match(header, new RegExp(`class ${name}_module final`));
    const manifest = readFileSync(resolve(root, `config/${name}.app.toml`), 'utf8');
    assert.ok(manifest.includes(`name = "${name}_plugin"`));
  }
  const physics = readFileSync(resolve(root, 'examples/physics_scene/CMakeLists.txt'), 'utf8');
  assert.match(physics, /FATAL_ERROR[\s\S]*unsupported legacy PhysX 3/);
});
