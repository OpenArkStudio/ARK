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


## CI dependency archive cache

CI restores vcpkg package archives from the runner temporary directory with
`actions/cache`. A versioned v2 key contains a compatibility prefix and an exact
runner-image suffix. The prefix includes the runner OS and architecture,
`ImageOS`, pinned vcpkg revision, manifest and configuration bytes (including the
baseline), local port overlay, selected/default features, build profile, compiler
environment flags and target/host triplet overrides. Only `ImageVersion` is moved
to the suffix. Within each ref searched by Actions, lookup uses the exact key
first, then that single prefix; there is no OS-only or cross-profile fallback.
Both image fields remain required.

An archive restore only makes candidates available to the vcpkg `files` provider.
It does not install an existing dependency tree. vcpkg computes each requested
package's full ABI hash, including compiler tracking, and retrieves only matching
archives; mismatched packages build from source. The workflow does not disable
compiler tracking, override ABI hashes or rename package archives. See the
[vcpkg ABI mismatch guide](https://learn.microsoft.com/en-us/vcpkg/users/binarycaching-troubleshooting#cause-3-differences-between-local-and-remote-build-environments).
A rolling image can therefore share an archive container without treating its
packages as compatible in advance.

After successful configuration, a miss or prefix restore saves only the exact
key. There is no second save under the compatibility prefix. Image-specific
snapshots can still overlap and use cache capacity. The v1 keys cannot be safely
searched by this prefix and are deliberately excluded: v2 starts cold until an
accessible run has saved a matching family. Actions cache branch/ref visibility
still applies; see the [Actions cache reference](https://docs.github.com/en/actions/reference/workflows-and-actions/dependency-caching#cache-key-matching).
This change does not make caches from another merge-queue ref readable. A compatible v2 archive on the default branch is needed to seed queue
runs that cannot access each other's entries.

Missing identity, unsupported dependency layout or archive paths keep persistence
disabled with `VCPKG_BINARY_SOURCES=clear`. Cache-service failures retain ordinary
source configuration, compilation and regressions. The cache covers archives only,
not installed trees or project object files. Local Node fixtures validate the
lookup boundaries, exports and workflow wiring; hosted cache reuse and any timing
improvement require separate CI evidence.
