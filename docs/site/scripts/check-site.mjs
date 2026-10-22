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
  const manifestSlug = "guides/service-configuration/application-manifests";
  const manifestRoute = "/ark/" + prefix + manifestSlug;
  const manifest = parse(await readFile(join(root, prefix, manifestSlug, "index.html"), "utf8"));
  const groups = new Set();
  const labels = new Set();
  const languages = new Set();
  const text = node => node.value ?? (node.childNodes ?? []).map(text).join("");
  function inspect(node) {
    const attrs = new Map((node.attrs ?? []).map(attr => [attr.name, attr.value]));
    if (attrs.has("data-nb-sidebar-group-label")) groups.add(text(node).trim());
    if (attrs.has("data-nb-sidebar-link") && attrs.get("href")?.replace(/\/$/, "") === manifestRoute) labels.add(text(node).trim());
    if (node.tagName === "option") languages.add(attrs.get("value")?.replace(/\/$/, ""));
    for (const child of node.childNodes ?? []) inspect(child);
  }
  inspect(manifest);
  assert.ok(groups.has(prefix ? "服务配置" : "Service configuration"), "missing localized service configuration group");
  assert.ok(labels.has(prefix ? "应用插件清单" : "Application plugin manifests"), "missing localized manifest navigation");
  assert.ok(languages.has("/ark/" + manifestSlug), "missing English manifest language target");
  assert.ok(languages.has("/ark/zh/" + manifestSlug), "missing Chinese manifest language target");
  const manifestMarkdown = await readFile(join(root, prefix, manifestSlug, "index.md"), "utf8");
  assert.match(manifestMarkdown, /```toml\n\[application\]/);
  assert.match(manifestMarkdown, /load_application_manifest/);
  assert.ok(manifestMarkdown.includes(`](/ark/${prefix}networking)`), "manifest Markdown lost its build-guide link");
  const codeStylePath = join(root, prefix, "project/code-style/index.html");
  const codeStyle = parse(await readFile(codeStylePath, "utf8"));
  let codeStyleArticle;
  const codeStyleLanguages = new Set();
  function inspectCodeStyle(node) {
    if (node.tagName === "article") codeStyleArticle = node;
    if (node.tagName === "option") {
      codeStyleLanguages.add(node.attrs?.find(attr => attr.name === "value")?.value?.replace(/\/$/, ""));
    }
    for (const child of node.childNodes ?? []) inspectCodeStyle(child);
  }
  inspectCodeStyle(codeStyle);
  assert.ok(codeStyleArticle, "missing canonical code-style article");
  const codeStyleText = text(codeStyleArticle);
  const codeStyleMarkdown = await readFile(join(root, prefix, "project/code-style/index.md"), "utf8");
  // The skipped aliases still contain accepted compatibility notes. Check their
  // API literals in actual output so adding a migration only to an alias fails.
  const styleAlias = await readFile(new URL(`../../code-style${prefix ? "_CN" : ""}.md`, import.meta.url), "utf8");
  const migrationStart = styleAlias.indexOf(prefix ? "## C++20 兼容层迁移" : "## C++20 compatibility migration");
  assert.ok(migrationStart >= 0, "missing accepted code-style migration boundary");
  const migrationTokens = [...styleAlias.slice(migrationStart).matchAll(/`([^`\n]+)`/g)].map(match => match[1]);
  for (const token of new Set([...migrationTokens, "CMAKE_EXPORT_COMPILE_COMMANDS=ON", "src/application/main.cpp",
    "meta_define.hpp", "session_counter", "RAII"])) {
    assert.ok(codeStyleText.includes(token), `${prefix}canonical code-style HTML lost ${token}`);
    assert.ok(codeStyleMarkdown.includes(token), `${prefix}canonical code-style Markdown lost ${token}`);
  }
  for (const anchor of prefix ? ["c20-兼容层迁移", "标准工具迁移"] : ["c20-compatibility-migration", "standard-utility-migration"]) {
    assert.ok(documents.get(codeStylePath).has(anchor), `${prefix}canonical code-style lost #${anchor}`);
  }
  for (const language of ["/ark/project/code-style", "/ark/zh/project/code-style"]) {
    assert.ok(codeStyleLanguages.has(language), `canonical code-style lost language target ${language}`);
  }

}
console.log(`Checked ${pages.length} static HTML pages and ${references.size} internal references, language switching, cards, callouts, highlighting and AI Markdown.`);
