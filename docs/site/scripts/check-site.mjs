import assert from "node:assert/strict";
import { readFile, readdir, stat } from "node:fs/promises";
import { join, resolve } from "node:path";
import { parse } from "parse5";

const root = resolve("dist");
const pages = [];
async function collect(directory) {
  for (const item of await readdir(directory, { withFileTypes: true })) {
    const file = join(directory, item.name);
    if (item.isDirectory()) await collect(file);
    else if (item.name.endsWith(".html")) pages.push(file);
  }
}
await collect(root);
const references = new Set();
const documents = new Map();
const failures = [];
const routes = JSON.parse(await readFile(new URL("../src/legacy-routes.json", import.meta.url), "utf8"));
const redirects = new Set(Object.keys(routes).map(route => join(root, route, "index.html")));
const navigation = new Set();
for (const page of pages) {
  const document = parse(await readFile(page, "utf8"));
  const ids = new Set();
  let headings = 0;
  function walk(node) {
    if (node.tagName === "h1") headings++;
    if (node.tagName === "a" && node.attrs?.some(attr => attr.name === "data-nb-sidebar-link")) {
      const href = node.attrs.find(attr => attr.name === "href")?.value;
      if (href) navigation.add(href.replace(/\/$/, ""));
    }
    for (const { name, value } of node.attrs ?? []) {
      if (name === "id") ids.add(value);
      if ((name === "href" || name === "src" || (name === "value" && node.tagName === "option")) && value.startsWith("/") && !value.startsWith("//")) {
        if (value !== "/ark" && !value.startsWith("/ark/")) failures.push(`${page}: reference escapes project base: ${value}`);
        else references.add(value);
      }
    }
    for (const child of node.childNodes ?? []) walk(child);
  }
  walk(document);
  if (!page.endsWith("404.html")) assert.equal(headings, 1, `${page}: one page heading required`);
  documents.set(page, ids);
}
for (const [page] of documents) {
  if (page.endsWith("404.html") || redirects.has(page)) continue;
  const route = "/ark/" + page.slice(root.length + 1).replace(/(?:^|\/)index\.html$/, "").replace(/\/$/, "");
  if (!navigation.has(route.replace(/\/$/, ""))) failures.push(`${page}: missing navigation entry`);
}
for (const [from, redirect] of Object.entries(routes)) {
  const target = join(root, redirect.to, "index.html");
  if (!documents.has(target)) failures.push(`${from}: missing redirect destination ${redirect.to}`);
  for (const [anchor, destination] of Object.entries(redirect.anchors)) {
    if (!documents.get(join(root, destination, "index.html"))?.has(anchor)) failures.push(`${from}#${anchor}: missing redirected anchor in ${destination}`);
  }
}
for (const reference of references) {
  const url = new URL(reference, "https://openarkstudio.github.io");
  let target = join(root, decodeURIComponent(url.pathname.slice("/ark".length)));
  try {
    if ((await stat(target)).isDirectory()) target = join(target, "index.html");
    await stat(target);
    const anchor = decodeURIComponent(url.hash.slice(1));
    if (anchor && documents.has(target) && !documents.get(target).has(anchor)) failures.push(`${reference}: missing anchor`);
  } catch {
    failures.push(`${reference}: missing static file`);
  }
}
assert.deepEqual(failures, [], "Broken site references:\n" + failures.join("\n"));
for (const file of ["llms.txt", "llms-full.txt", "zh/llms.txt", "networking/index.md", "zh/networking/index.md", "sitemap-index.xml", "robots.txt"]) {
  assert.ok((await stat(join(root, file))).size > 0, `missing AI/SEO output: ${file}`);
}
for (const prefix of ["", "zh/"]) {
  const home = await readFile(join(root, prefix, "index.html"), "utf8");
  assert.match(home, /data-home-links/);
  assert.match(home, /data-language-switch/);
  const install = await readFile(join(root, prefix, "get-started/installation/index.html"), "utf8");
  assert.match(install, /data-steps/);
  assert.match(install, /data-step/);
  const markdown = await readFile(join(root, prefix, "get-started/installation/index.md"), "utf8");
  assert.match(markdown, /\n1\. /);
  assert.doesNotMatch(markdown, /<(?:Aside|Steps|Step)[\s>]/);
  const plugin = await readFile(join(root, prefix, "guides/plugins/index.html"), "utf8");
  assert.match(plugin, /role="note"/);
  assert.match(plugin, /class="astro-code/);
}
console.log(`Checked ${pages.length} static HTML pages and ${references.size} internal references, language switching, cards, callouts, highlighting and AI Markdown.`);
