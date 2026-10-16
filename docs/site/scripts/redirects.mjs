import { readFile, mkdir, writeFile } from "node:fs/promises";
import { join } from "node:path";
const routes = JSON.parse(await readFile(new URL("../src/legacy-routes.json", import.meta.url), "utf8"));
const escape = value => value.replaceAll("&", "&amp;").replaceAll('"', "&quot;").replaceAll("<", "&lt;");
for (const [from, redirect] of Object.entries(routes)) {
  const target = "/ark" + redirect.to;
  const directory = join("dist", from);
  await mkdir(directory, { recursive: true });
  await writeFile(join(directory, "index.html"), `<!doctype html><html lang="${from.startsWith('/zh/') ? 'zh-CN' : 'en'}"><head><meta charset="utf-8"><meta name="robots" content="noindex"><link rel="canonical" href="${escape(target)}"><title>Moved | ark</title></head><body><h1>Moved</h1><a href="${escape(target)}">Continue</a><script>const route=${JSON.stringify(redirect).replaceAll('<', '\\u003c')};const anchor=decodeURIComponent(location.hash.slice(1));location.replace('/ark'+(route.anchors[anchor]??route.to)+location.search+location.hash);</script></body></html>`);
}
console.log(`Wrote ${Object.keys(routes).length} legacy route redirects.`);
