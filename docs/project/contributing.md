# Contributing and documentation

Changes target the protected main branch through pull requests. Run the relevant checks and squash merge with an English commit subject. See the [contribution guide](https://github.com/OpenArkStudio/ark/blob/main/.github/CONTRIBUTING.md).

## Develop the documentation

The site uses Cloudflare Nimbus with Astro. Markdown in `docs/` is the content source; each page has an English file and a `_CN.md` translation. Generated site content and output are ignored.

```sh
npm ci --prefix docs/site
npm run dev --prefix docs/site -- --host 127.0.0.1
npm run check --prefix docs/site
```

The site provides language switching, search, code highlighting and plain Markdown endpoints for AI readers. Pull requests run the documentation checks without publishing. After a change is merged into `main`, the documentation workflow builds and publishes GitHub Pages; maintainers can also trigger it manually. Node.js 22.12+ is needed for the documentation build; it is not needed to compile ark or its C++ tests.
