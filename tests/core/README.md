# Core regressions

[简体中文](README_CN.md)

The doctest executable covers string growth and equality, hash keys, object ownership,
constant iteration, conversion templates, typed module lookup and member callbacks.
Initialize the pinned dependencies as described in [network builds](../../docs/networking.md).

```sh
cmake -S . -B core-build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_TESTS=OFF -DARK_BUILD_CORE_TESTS=ON
cmake --build core-build --target check-core
```

Run `core-build/tests/core/ark_core_tests --help` for doctest filters. `check-network`
also runs these tests when `ARK_BUILD_CORE_TESTS=ON`, including in the existing CI
matrix. No database service is required.

With `ARK_BUILD_PROTO=ON`, additional cases verify Protobuf parse failures, actor
identity, binary messages, legacy wire payloads and external JSON field names.
They also exercise the new `ark::protobuf` descriptor and enum names.
Initialize Protobuf as described for application builds.

With `ARK_BUILD_CONFIG=ON` (the default for core tests),
`check-application-manifest` checks TOML/XML equivalence, defaults, strict fields,
invalid input, diagnostics and owning results. It is also part of `check-core`.
See [application manifests](../../docs/service-manifests.md).

When the application is also built, `check-app-manifest-failures` runs the real
launcher against missing, malformed and mistyped manifests. It requires normal
exit code 1 with a manifest diagnostic, including on Windows, and is included in
`check-core`.

`check-kernel-initialization` loads real metadata and static data from
`resource/meta/`. It checks four kernel/config initialization orders with both
resource-relative paths and legacy paths beginning with `/`, for eight combinations.

`check-logging` runs the independent C++ logging path regressions and is included
in `check-core`. It checks relative and nested filenames, extensions and hidden
files, dotted directory names, both hourly sink factories, and the configured
logger directory. An empty `output_path` uses the working directory. Hourly logs
retain the `directory/YYYYMMDD/basename_hh.ext` layout; only the filename's final
extension moves after the hour suffix. Files without an extension and names ending
in a dot keep that basename. All test writes use temporary directories.

```sh
cmake --build core-build --target check-logging
```

## Public task SDK

`check-task-sdk` is included in `check-core`. It compiles `<ark/task.hpp>`
without an earlier include and runs an ordinary `ark::core` consumer. Cases
cover typed owning values, stopped admission, explicit cancellation and owner
thread checks. The controlled child checks require exit 77 for pending-task and
wrong-thread destruction; they do not abandon suspended frames in the test owner.
Private database task tests retain forwarding aliases to this same public class.
See [Coroutine tasks](../../docs/guides/tasks.md) for the independently runnable
`ark_task_example` and explicit lifetime requirements.
