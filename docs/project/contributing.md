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
A rolling image can therefore share an archive cache without treating its
packages as compatible in advance.

After successful configuration, a miss or prefix restore saves only the exact
key. There is no second save under the compatibility prefix. Image-specific
snapshots can still overlap and use cache capacity. The v1 keys cannot be safely
searched by this prefix and are deliberately excluded: v2 starts cold until an
accessible run has saved a v2 cache for the same dependency configuration.
Actions cache branch/ref visibility still applies; see the [Actions cache reference](https://docs.github.com/en/actions/reference/workflows-and-actions/dependency-caching#cache-key-matching).
This change does not make caches from another merge-queue ref readable. A compatible v2 archive on the default branch is needed to seed queue
runs that cannot access each other's entries.

Missing identity, unsupported dependency layout or archive paths keep persistence
disabled with `VCPKG_BINARY_SOURCES=clear`. Cache-service failures retain ordinary
source configuration, compilation and regressions. The cache covers archives only,
not installed trees or project object files. Local Node tests validate the
lookup boundaries, exports and workflow wiring; hosted cache reuse and any timing
improvement require separate CI evidence.


### Windows ABI diagnostics

After dependency configuration and the existing archive-save step, Windows
Applications logs a bounded JSON report for Asio, rapidxml and Protobuf. The
helper reads only `ci-build/vcpkg_installed/x64-windows/share/<port>/vcpkg_abi_info.txt`,
without searching directories or invoking build tools. It rejects symbolic links,
non-regular files, malformed records, unexpected triplets and files over 64 KiB.
Absent or rejected metadata is explicitly `unavailable`; missing optional fields
also remain `unavailable` rather than being treated as equal.

`packageAbi` is the SHA256 of the exact ABI metadata bytes. Field values are
reported only as SHA256 fingerprints under fixed names: `triplet`, `triplet_abi`,
`features`, `cmake`, `powershell`, `compiler_hash`, `portfile.cmake`, `ports.cmake`,
`vcpkg.json`, `post_build_checks`, `sbom_info`, `vcpkg-cmake`, `vcpkg-cmake-config`,
`abseil` and `utf8-range`. Unknown field names and all raw values are omitted;
no full environment, absolute paths or credentials are logged. Metadata files
and artifacts are not uploaded.
A separate `compiler_hash` record may be absent; the report does not infer one
from a compiler version string or split the opaque `triplet_abi` value.

The diagnostic also runs after an earlier step fails, unless the job is cancelled.
Its availability status describes metadata only. It cannot turn failed
configuration, compilation or tests into success, and diagnostic errors do not
become a new native acceptance requirement. Cache keys/providers, compiler
tracking and build settings remain unchanged. Compare the fingerprints across
runs to narrow an ABI difference; equal reported fields do not establish equal
unreported inputs. This report is neither a cache-hit verdict nor a performance
measurement. In the investigated Windows pair, all 74 package ABIs differed;
the particular input causing that difference remains unproven.
