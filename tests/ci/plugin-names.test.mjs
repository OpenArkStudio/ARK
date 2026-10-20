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
  for (const expected of ['ark_mysql_plugin', 'ark_redis_plugin', 'ark_utility_plugin', 'sample1_plugin']) {
    assert.ok(names.has(expected), `Missing declared target ${expected}`);
  }
});
