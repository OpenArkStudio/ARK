# CI policy tests

Run the native Node tests without installing npm packages:

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
macOS and Windows plus Linux ASan/UBSan and TSan; Applications on Linux, macOS and
Windows; Database on Linux Boost, Linux Standalone, macOS Boost and Windows Boost.
Manual runs retain the same full matrix. Only a known, nonempty documentation-only
range with an available site can skip native jobs. Queue candidates retain site
validation when the site is present, and README changes require their link check.

The required gate validates the actual workflow event and the declared validation
stage, then requires success from each selected job and an explicit skipped result
from each unnecessary job. Failed, cancelled, unknown or missing results fail;
a source module result cannot substitute for a queue result. Full acceptance still
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
