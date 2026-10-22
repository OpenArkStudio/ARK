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

The public header checks include `<ark/error_code.hpp>` as the first include and
include it twice. Compile-time checks preserve its unsigned 32-bit underlying type
and every declared error value.

With `ARK_BUILD_PROTO=ON`, additional cases verify Protobuf parse failures, actor
identity, binary messages, legacy wire payloads and external JSON field names.
They also exercise the new `ark::protobuf` descriptor and enum names.
Initialize Protobuf as described for application builds.

The C++20 language regressions check the exact six built-in integer classifications, qualifier and reference handling, constant values, exception contracts and fixed UID allocation outputs. The standalone array-map header check also rejects reintroduced legacy language macros.

With `ARK_BUILD_CONFIG=ON` (the default for core tests),
`check-application-manifest` checks TOML/XML equivalence, defaults, strict fields,
invalid input, diagnostics and owning results. It is also part of `check-core`.
See [application manifests](../../docs/service-manifests.md).

When the application is also built, `check-app-manifest-failures` runs the real
launcher against missing, malformed and mistyped manifests. It requires normal
exit code 1 with a manifest diagnostic, including on Windows, and is included in
`check-core`.

`check-app-startup-failures` uses the real launcher and a test DSO for all four startup phases, each returning false or throwing, with both TOML and XML manifests. It also checks a non-standard exception, a throwing plugin entrypoint, a missing library and a loadable DSO with no `DllEntryPlugin`. Before each controlled module failure or entrypoint exception, the registered test module starts and retains a real owned loopback TCP listener on an ephemeral port. The missing-entry fixture confirms native library initialization occurred before failure. All 24 children must report one exact phase/module-or-plugin diagnostic, preserve available exception details, and exit normally with code 1 without later startup calls, update, normal shutdown or uninstall. Core tests check retained failure context, unchanged exception propagation, first failure across multiple modules and owner-thread access. The listener makes ordinary static cleanup block if failed-start module ownership is retained, so the exit check covers a real active IO pool. These tests establish failure reporting and process termination, not partial-start resource cleanup or same-process recovery.

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

## Public task interface

`check-task-sdk` is included in `check-core`. It compiles `<ark/task.hpp>`
without an earlier include and runs an ordinary `ark::core` consumer. Cases
cover typed owning values, stopped admission, explicit cancellation and owner
thread checks. The controlled child checks require exit 77 for pending-task and
wrong-thread destruction; they do not abandon suspended frames in the test owner.
Private database task tests retain forwarding aliases to this same public class.
See [Coroutine tasks](../../docs/guides/tasks.md) for the independently runnable
`ark_task_example` and explicit lifetime requirements.

## Shared business runtime

`check-business-runtime`, included in `check-core`, compiles a standalone
`<ark/plugin_manager.hpp>` consumer and checks one shared executor, the total
handler budget, module completion publication before polling, sequential awaits
after admission stops, late results, owner/reentrancy checks, pending-module
unregister, stable group references and failure diagnostics. The real unregister
macro must throw before deleting a pending task's module. Four controlled child
processes cover pending, wrong-thread, update-stack and final-completion-stack
destruction. These tests require no database service.

```sh
cmake --build core-build --target check-business-runtime
```

The manager tests establish managed business-task drain. The launcher fixture below also checks one controlled native worker retirement barrier; neither establishes generic dynamic-library unload safety. See [module task integration](../../docs/guides/tasks.md#shared-business-executor-for-modules).

When applications and configuration are built, `check-app-business-runtime` loads a test-only native plugin in the real launcher. POSIX cases request SIGTERM; Windows supplies `exit` to the real command thread. The accepted task receives two controlled private-bridge results during draining, rejects a new root task with spare group capacity, and finishes its callback before owner-frame stop, native worker exit/join and shutdown/uninstall. Default unlimited and positive-deadline successful drains both exit 0. Separate false/throwing-hook, standard/non-standard throwing-update, stalled-task and delayed task/native-worker children must diagnose failure once, retain their module and remain alive until the owned test observation timeout. Throwing-update traces require an observed throw followed by a resumed owner update and accepted completion; Windows must not route these handled C++ exceptions into the native crash dialog. Late completion after the configured application deadline must not resume unloading. Malformed, negative and overflowing CLI deadlines are rejected before manifest loading. The application deadline is cooperative and cannot interrupt a blocking user update/readiness/shutdown call. The owned test observation timeout terminates only the test child and is not a successful shutdown or a production policy. No real database is involved.

With applications, examples, Kernel and Utility enabled, `check-typed-examples` loads the actual Utility DSO and verifies all four interfaces, interface RTTI keys, GUID creation/parsing, zlib/Zstandard binary round trips, a timer advanced by manager timestamps, and normal/idempotent shutdown. `check-app-presets` also loads the shipped Timers runtime; both are dependencies of `check-core`. The task/scheduler include-order probes link `ark::utility` for the mapped scheduler header, while the separate public task-only checks retain `ark::core` alone.

With Net, Bus, Utility and Protobuf enabled, `check-net-plugin` loads actual DSOs and checks both Net interfaces and RTTI keys, manager-thread TCP/HTTP callbacks, repeated shutdown and listener release. It is also a `check-core` dependency and requires no external service. The independent `tests/consumer/net` project links only `ark::net` and compiles every retained public header separately.

With Bus, Net, Utility and Protobuf enabled, `check-bus-plugin` loads actual DSOs and checks Bus/message interface keys, configured queries, routed Protobuf headers/body, transport failure propagation and repeated normal shutdown. `check-core` includes this target. The independent `tests/consumer/bus` project links only `ark::bus` and compiles both public module headers separately. Existing parser/configuration cases use an interface factory while retaining their assertions, and still support Protobuf-disabled builds.
