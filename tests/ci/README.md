# CI policy tests

Run the native Node tests without installing npm packages:

```sh
node --test tests/ci/*.test.mjs
```

The tests use temporary local Git repositories. No GitHub API token, network service,
C++ rebuild or database is required. They verify complete rename-aware path decisions,
safe fallback to the full C++ matrix, documentation availability and strict job results.
Main pushes and manual runs always retain the full C++ matrix.

Root README link checks run only when a root README changes. They validate local file
targets, including images, HTML `srcset` and reference links. It handles the forms
used by these READMEs; it is not a complete Markdown parser. Remote URLs and heading
fragments are not validated. The Nimbus check separately validates links and anchors
within the generated documentation site, not root README fragments.

The vcpkg cache tests cover Windows/Unix archive paths, dependency and runner-image
key changes, provider exports, unavailable storage and explicit cold-build fallbacks.
The C++ CI jobs restore only local package archives through Actions cache; vcpkg
still validates each package ABI. A cache miss/service failure runs the ordinary
dependency build and tests. Archives are saved only after the job's tests succeed.
Missing runner-image identity or unsupported archive paths disable persistence and
export `VCPKG_BINARY_SOURCES=clear`, which prevents run-vcpkg's removed `x-gha`
default from being selected. Missing GitHub file-command channels fail explicitly.
No project object files, installed trees, credentials or compiler caches are persisted.
