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
