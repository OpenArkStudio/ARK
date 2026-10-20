# CI policy tests

Run the native Node tests without installing npm packages (CMake 3.21 or newer
is required for the portable Windows runtime export fixtures):

```sh
node --test tests/ci/*.test.mjs
```

The tests use temporary local Git repositories. No GitHub API token, network service,
C++ rebuild or database is required. They verify complete rename-aware path decisions,
event-specific module selection, documentation availability and strict job results.

Source PRs run the related native modules on Linux with Boost, reusing the existing
configure/build/regression steps. Core/application tests select Applications;
network tests select Network; database tests select Database. Network/HTTP/Bus
plugin changes also select Applications, and MySQL/Redis/shared database support
changes select Applications and Database. Public core headers, shared build or
dependency inputs, CI changes and unknown paths select all three source modules.
Missing, invalid or empty enumeration falls back to the full 12-variant matrix.
Native changes run
the fatal clang-format 18 check. Documentation and README link checks retain their
existing path rules; source CI success reports module validation, not full matrix
acceptance.

Merge-group candidates compare the event's `base_sha` directly with `head_sha`,
including changes from earlier queued PRs. A candidate containing code, build,
dependency, CI or unknown paths runs all 12 native variants: Network on Linux,
macOS and Windows plus Linux AddressSanitizer（内存错误检测） with
UndefinedBehaviorSanitizer（未定义行为检测） and ThreadSanitizer（线程数据竞争检测）;
Applications on Linux, macOS and Windows; Database on Linux Boost, Linux
Standalone, macOS Boost and Windows Boost.
Manual runs retain the same full matrix. Only a known, nonempty documentation-only
range with an available site can skip native jobs. Queue candidates retain site
validation when the site is present, and README changes require their link check.

Documentation, root README links and C++ formatting share one `validation` job,
one checkout and one Node setup. Nimbus uses the npm package cache when its site
and lockfile are available; a required unavailable site still fails its preflight.
clang-format 18 is installed only when its command is absent. Each check keeps its
existing scope flag and exports its actual step `outcome`; later selected checks
still run after an earlier check fails. The scope job remains separate so native
matrices and lightweight validation can start together after classification.
This reduces the workflow from eight job definitions to six, or from 17 to 15
runner jobs when all 12 native variants and all lightweight checks are selected.
Actual waiting time depends on runner availability and check durations.

The required gate validates the actual workflow event and the declared validation
stage. Selected native jobs and the combined validation job must succeed;
unnecessary native jobs must be explicitly skipped. Each selected validation
step must report `success`, and each unnecessary step must report `skipped`.
A successful combined job cannot substitute for a missing or skipped required
step. Failed, cancelled, unknown or missing results fail; a source module result
cannot substitute for a queue result. Full acceptance still
requires the combined queue candidate. Protected `main` accepts only candidates
that passed their required checks; its resulting push does not repeat CI for the
same accepted tree. The separate documentation workflow still publishes on main
pushes. README badges display the merge-group workflow status.

Each existing PR/ref concurrency group permits one active run and only the most
recently queued pending run. A new run waits for the active run to finish instead
of automatically cancelling it. Individual running jobs retain their configured
timeouts; those limits do not include pending time. Merge-group candidates use
their own refs. The required gate still rejects failed, cancelled or missing
results. Manual cancellation can still prevent the `always()` summary from
completing. See [GitHub concurrency rules](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax#concurrency).

Root README link checks run only when a root README changes. They validate local file
targets, including images, HTML `srcset` and reference links. It handles the forms
used by these READMEs; it is not a complete Markdown parser. Remote URLs and heading
fragments are not validated. The Nimbus check separately validates links and anchors
within the generated documentation site, not root README fragments.

Windows Applications CI disables vcpkg's per-target app-local dependency copying
with `VCPKG_APPLOCAL_DEPS=OFF`. The pinned toolchain otherwise adds post-build
copy commands to shared libraries and executables, many of which use the same
output directory. After configuration, `windows-runtime.cmake` reads the actual
installed root, target triplet and build configuration from `CMakeCache.txt`.
It requires the matching dependency `bin` directory and exports only that directory
through `GITHUB_PATH`, so subsequent parallel builds, regression targets and their
application subprocesses inherit it. Debug uses `debug/bin`; release-family
configurations use `bin`, without a cross-configuration fallback. Invalid cache
inputs, a missing runtime directory or an unusable output channel fail the job.
Network, Database and ordinary developer builds retain their existing behavior.
The tests exercise real CMake cache loading, directory selection, subprocess PATH
inheritance and failure propagation. This removes shared app-local copy writes;
it does not identify the owner of an observed Windows file lock. Windows CI must
still validate actual DLL loading and all application regressions.

The vcpkg cache tests cover Windows/Unix archive paths, dependency and runner-image
key changes, provider exports, unavailable storage and explicit cold-build fallbacks.
The C++ CI jobs restore only local package archives through Actions cache; vcpkg
still validates each package ABI. A cache miss/service failure runs the ordinary
dependency build and tests. Archives are saved after successful CMake configuration,
before compiling and testing ark; later build or test failures still fail the job.
Workflow checks cover this order and retain fatal configure/build/test steps.
Missing runner-image identity or unsupported archive paths disable persistence and
export `VCPKG_BINARY_SOURCES=clear`, which prevents run-vcpkg's removed `x-gha`
default from being selected. Missing GitHub file-command channels fail explicitly.
No project object files, installed trees, credentials or compiler caches are persisted.
