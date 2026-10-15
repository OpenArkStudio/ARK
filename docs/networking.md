# Network builds and regression tests

English | [简体中文](networking_CN.md)

ARK requires C++20 and uses [arknet](https://github.com/OpenArkStudio/arknet)
directly for TCP, WebSocket, HTTP and RDP. RDP uses arknet's RUDP endpoints with
the legacy ARQ wire format, MTU 1400 and FEC disabled. Application message headers
and Update-thread callback dispatch remain the same. No coroutine API migration
is included.

## Dependencies

- CMake 3.21+ and a C++20 compiler: GCC 12+, Clang with C++20 support, or VS 2022.
- Standalone Asio 1.38.0+, or Boost 1.90+ when `ASIO_STANDALONE=OFF`.
- RapidXML 1.13, spdlog 1.17.0 and ARK's pinned readerwriterqueue/args submodules.
- Protobuf, Zstandard and zlib when building applications and plugins.

The vcpkg manifest pins the package baseline. CMake retrieves arknet from its
pinned revision in the independent repository. To validate a development checkout,
set `ARKNET_SOURCE_DIR` to its absolute path.

Use Protobuf, Abseil and utf8_range from the same installation prefix. When
setting `Protobuf_DIR` manually, select matching `absl_DIR` and `utf8_range_DIR`.

## Build and run the network tests

```sh
git submodule update --init 3rdparty/args 3rdparty/spdlog 3rdparty/readerwriterqueue
cmake -S . -B network-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_NETWORK_TESTS=ON \
  -DASIO_STANDALONE=OFF -DCMAKE_BUILD_TYPE=Debug
cmake --build network-build --parallel 2
cmake --build network-build --target check-network
```

Boost is the default dependency. For Standalone Asio, use `-DASIO_STANDALONE=ON`;
add `-DVCPKG_MANIFEST_NO_DEFAULT_FEATURES=ON` to omit the Boost installation. The tests are permanent C++ doctest cases under
`tests/network/`; the binary can also run directly or select cases with
`--test-case=RUDP*`. Python and Ruby are not required.

Coverage includes framing, binary payloads, reconnection, limits and timeouts,
Update-thread dispatch, reentrant callbacks, HTTP status/headers/query data,
shutdown cancellation, owner release inside receive callbacks, malformed input and log formatting/directory creation/rotation.
CI checks both dependency configurations on Linux,
macOS and Windows, plus memory/undefined-behavior and thread sanitizers on Linux.

Local validation on 2026-10-15 used an Apple M4 Pro (14 CPU cores, 48 GiB RAM),
macOS 27.0.1 arm64 and Apple Clang 21.0.0. Local results:

| Configuration | Core cases / assertions | Network cases / assertions |
| --- | ---: | ---: |
| Release, Boost 1.90, 64-bit GUID, Protobuf | 16 / 106 | 32 / 879 |
| Debug, Standalone Asio 1.38.2, 32-bit GUID, ASan/UBSan, Protobuf disabled | 9 / 55 | 32 / 879 |
| Debug, Boost, 64-bit GUID, ASan/UBSan, Protobuf | 16 / 106 | — |

Protobuf cases cover valid messages, truncated input, actor identity, binary messages containing NUL,
existing wire payloads, external JSON field names and the new public descriptor/enum names.

macOS ASan ran without leak detection; Linux CI enables LeakSanitizer. These are
correctness checks, not capacity measurements. The full Release application,
default plugins and examples built; all eight legacy test executables passed.
GCC 15.2 passed C++20 syntax checks for ten application, default-plugin and core-test
translation units; this does not establish GCC link or runtime validation.

See [core regressions](../tests/core/README.md) for independent builds and filtering.
When `ARK_BUILD_CORE_TESTS=ON`, `check-network` also runs the core cases.

## Build applications

Enable the `application` manifest feature to install Protobuf:

```sh
cmake -S . -B app-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES=application
cmake --build app-build --parallel 2
```

## Deployment acceptance

Loopback regression is not a 100,000–200,000 PCU capacity result. Cross-machine
load, endurance and the application's complete business flows require separate
acceptance before production rollout. ARQ/FEC performance measurements are in
[arknet's reports](https://openarkstudio.github.io/arknet/performance/).
