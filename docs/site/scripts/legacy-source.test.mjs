import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { readFile, readdir } from "node:fs/promises";
import { test } from "node:test";
import { articles, sourceCommit } from "./migration-source.mjs";

const archive = new URL("../legacy-source/", import.meta.url);

async function files(directory, prefix = "") {
  const result = [];
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    const name = prefix + entry.name;
    if (entry.isDirectory()) result.push(...await files(new URL(entry.name + "/", directory), name + "/"));
    else result.push(name);
  }
  return result;
}

test("historical article archive preserves its snapshot, source mapping and exact bytes", async () => {
  const readme = await readFile(new URL("README.md", archive), "utf8");
  assert.match(readme, new RegExp("Snapshot: `" + sourceCommit + "`"));
  const attributes = await readFile(new URL(".gitattributes", archive), "utf8");
  assert.match(attributes, /^\*\.md -text$/m);
  const checksums = new Map();
  for (const row of readme.matchAll(/^\| `([^`]+)` \| `([^`]+)` \| `([0-9a-f]{64})` \|$/gm)) {
    assert.equal(checksums.has(row[1]), false, `duplicate provenance row: ${row[1]}`);
    checksums.set(row[1], { target: row[2], digest: row[3] });
  }
  const expected = Object.keys(articles).sort();
  assert.equal(expected.length, 40);
  assert.deepEqual([...checksums.keys()].sort(), expected);
  assert.deepEqual((await files(archive)).filter(file => file !== "README.md" && file !== ".gitattributes").sort(), expected);
  for (const file of expected) {
    assert.doesNotMatch(file, /(?:^\/|\.\.|\\)/, `source path is relative: ${file}`);
    const record = checksums.get(file);
    assert.equal(record.target, articles[file], `mapping for ${file}`);
    const buffer = await readFile(new URL(file, archive));
    const digest = createHash("sha256").update(buffer).digest("hex");
    assert.equal(digest, record.digest, `original bytes changed: ${file}`);
  }
});
