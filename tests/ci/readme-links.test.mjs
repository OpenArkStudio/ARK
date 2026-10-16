import assert from 'node:assert/strict';
import { mkdtempSync, mkdirSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, resolve } from 'node:path';
import test from 'node:test';
import { checkReadmeLinks, linkTargets } from '../../.github/scripts/check-readme-links.mjs';

test('README targets include inline, nested, reference and HTML links but ignore examples in code', () => {
  const source = '[guide](docs/guide.md) [space](<docs/a b.md>) [nested](docs/a(b).md)\n' +
    '[reference][ref]\n[ref]: docs/ref.md "title"\n<img src="docs/image.svg">\n' +
    '`[ignored](missing.md)`\n```md\n[ignored](missing.md)\n```\n<!-- [ignored](missing.md) -->';
  assert.deepEqual(linkTargets(source), ['docs/guide.md', 'docs/a b.md', 'docs/a(b).md', 'docs/ref.md', 'docs/image.svg']);
});

test('root README checks fail on missing/deleted/local escaping targets and do not fetch remote URLs', t => {
  const root = mkdtempSync(resolve(tmpdir(), 'ark-readme-links-'));
  t.after(() => rmSync(root, { recursive: true, force: true }));
  const write = (file, text) => { mkdirSync(dirname(resolve(root, file)), { recursive: true }); writeFileSync(resolve(root, file), text); };
  write('docs/a b.md', '# Present\n');
  write('README.md', '[local](docs/a%20b.md#section) [remote](https://invalid.example/never-fetched) [mail](mailto:test@example.invalid)');
  write('README_CN.md', '[English](README.md)');
  assert.deepEqual(checkReadmeLinks(root), []);
  write('README.md', '[missing](docs/missing.md) [escape](../outside.md) [bad](docs/%ZZ.md)');
  assert.equal(checkReadmeLinks(root).length, 3);
  rmSync(resolve(root, 'README_CN.md'));
  assert.equal(checkReadmeLinks(root).length, 4);
});

test('a missing dark srcset asset fails while data and HTTP srcset URLs are not local files', t => {
  const root = mkdtempSync(resolve(tmpdir(), 'ark-readme-srcset-'));
  t.after(() => rmSync(root, { recursive: true, force: true }));
  mkdirSync(resolve(root, 'docs/assets'), { recursive: true });
  writeFileSync(resolve(root, 'docs/assets/light.svg'), '<svg/>');
  writeFileSync(resolve(root, 'README_CN.md'), '[English](README.md)');
  writeFileSync(resolve(root, 'README.md'), '<picture><source media="(prefers-color-scheme: dark)" ' +
    'srcset="docs/assets/dark.svg 1x, https://invalid.example/dark.svg 2x">' +
    '<img src="docs/assets/light.svg"></picture>\n' +
    '<img srcset="data:image/svg+xml,%3Csvg%3E 1x, https://invalid.example/image.svg?a=b,c 2x">');
  assert.equal(checkReadmeLinks(root).length, 1);
  assert.match(checkReadmeLinks(root)[0], /dark\.svg/);
  writeFileSync(resolve(root, 'docs/assets/dark.svg'), '<svg/>');
  assert.deepEqual(checkReadmeLinks(root), []);
});
