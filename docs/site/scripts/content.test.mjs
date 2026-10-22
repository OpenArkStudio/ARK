import assert from "node:assert/strict";
import { test } from "node:test";
import { canonicalFile, convertPage, pageExtension, pageRoute } from "./content.mjs";

test("bilingual routes and home aliases", () => {
  assert.equal(pageRoute("README_CN.md"), "/zh/");
  assert.equal(pageRoute("udp/performance.md"), "/udp/performance");
  assert.equal(pageRoute("THIRD_PARTY_NOTICES_CN.md"), "/zh/third-party-notices");
  assert.equal(pageRoute("features/udp/index_CN.md"), "/zh/features/udp");
  assert.equal(pageRoute("features/index.md"), "/features");
  assert.equal(pageRoute("code-style.md"), "/project/code-style");
  assert.equal(pageRoute("project-layout_CN.md"), "/zh/reference/directory-structure");
  assert.equal(canonicalFile("service-manifests.md"), "guides/service-configuration/application-manifests.md");
  assert.equal(canonicalFile("service-manifests_CN.md"), "guides/service-configuration/application-manifests_CN.md");
  assert.equal(pageRoute("service-manifests.md"), "/guides/service-configuration/application-manifests");
  assert.equal(pageRoute("service-manifests_CN.md"), "/zh/guides/service-configuration/application-manifests");
});

test("relocated manifest sources resolve links from their original Markdown location", () => {
  const files = new Set(["service-manifests.md", "service-manifests_CN.md", "networking.md", "networking_CN.md"]);
  for (const chinese of [false, true]) {
    const suffix = chinese ? "_CN" : "";
    const prefix = chinese ? "/zh" : "";
    const source = `# Manifest\n\n[build](networking${suffix}.md)\n\n[manifest](service-manifests${suffix}.md)\n\n[sample](../config/timers.app.toml)\n\n\`\`\`toml\n[application]\nplugin_config_dir = "conf/"\n\`\`\`\n`;
    const result = convertPage(source, `service-manifests${suffix}.md`, files);
    assert.ok(result.includes(`[build](${prefix}/networking)`));
    assert.ok(result.includes(`[manifest](${prefix}/guides/service-configuration/application-manifests)`));
    assert.ok(result.includes("[sample](https://github.com/OpenArkStudio/ark/blob/main/config/timers.app.toml)"));
    assert.ok(result.includes('```toml\n[application]\nplugin_config_dir = "conf/"\n```'));
  }
});

test("convert parsed links while preserving code and labels", () => {
  const source = '# UDP\n\n[用法](usage_CN.md#示例)\n\n![吞吐](../performance/assets/udp.svg)\n\n```cpp\n// [use](usage_CN.md)\n```\n';
  const result = convertPage(source, "udp/overview_CN.md", new Set(["udp/usage_CN.md"]));
  assert.match(result, /\[用法\]\(\/zh\/udp\/usage#%E7%A4%BA%E4%BE%8B\)/);
  assert.match(result, /!\[吞吐\]\(\/ark\/performance\/assets\/udp.svg\)/);
  assert.match(result, /\/\/ \[use\]\(usage_CN.md\)/);
  assert.match(result, /title: "UDP"/);
  assert.doesNotMatch(result, /# UDP/);
});

test("source references point at GitHub; missing documents fail", () => {
  const result = convertPage("# Page\n\n[source](../include/ark.hpp)", "page.md", new Set());
  assert.match(result, /https:\/\/github.com\/OpenArkStudio\/ark\/blob\/main\/include\/ark.hpp/);
  assert.throws(() => convertPage("# Page\n\n[missing](missing.md)", "page.md", new Set()), /missing document/);
});

test("native Nimbus components use MDX and retain nested Markdown links", () => {
  const source = '# Guide\n\n<Steps>\n\n<Step title="Install">\n\n[Setup](installation.md)\n\n</Step>\n\n</Steps>\n\n<Aside type="caution" title="Warning">\n\nRead the [setup](installation.md).\n\n</Aside>\n';
  const result = convertPage(source, "guide.md", new Set(["installation.md"]));
  assert.equal(pageExtension(source), ".mdx");
  assert.match(result, /import \{ Aside \} from "@\/components\/ui\/aside"/);
  assert.match(result, /import \{ Step, Steps \} from "@\/components\/ui\/steps"/);
  assert.match(result, /\[Setup\]\(\/installation\)/);
  assert.match(result, /\[setup\]\(\/installation\)/);
});

test("component examples inside code blocks and inline code stay plain Markdown", () => {
  const source = '# Authoring\n\nUse `<Steps>`.\n\n```md\n<Aside type="note">\n\nExample\n\n</Aside>\n```\n';
  const result = convertPage(source, "authoring.md", new Set());
  assert.equal(pageExtension(source), ".md");
  assert.doesNotMatch(result, /import \{/);
  assert.ok(result.includes('```md\n<Aside type="note">'));
});
