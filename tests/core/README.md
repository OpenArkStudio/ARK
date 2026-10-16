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
