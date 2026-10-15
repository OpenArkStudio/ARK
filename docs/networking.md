# Network builds and regression tests

English | [简体中文](networking_CN.md)

ARK requires C++20 and uses [arknet v0.1.0](https://github.com/OpenArkStudio/arknet)
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
release tag in the independent repository. To validate a development checkout,
set `ARKNET_SOURCE_DIR` to its absolute path.

## Build and run the network tests

```sh
git submodule update --init 3rdparty/args 3rdparty/spdlog 3rdparty/readerwriterqueue
cmake -S . -B network-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_NETWORK_TESTS=ON \
  -DASIO_STANDALONE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build network-build --parallel 2
cmake --build network-build --target check-network
```

For the Boost backend, add `-DVCPKG_MANIFEST_FEATURES=boost` and use
`-DASIO_STANDALONE=OFF`. The tests are permanent C++ doctest cases under
`test/network/`; the binary can also run directly or select cases with
`--test-case=RUDP*`. Python and Ruby are not required.

Coverage includes framing, binary payloads, reconnection, limits and timeouts,
Update-thread dispatch, reentrant callbacks, HTTP status/headers/query data,
shutdown cancellation, malformed input and log formatting/directory creation/rotation.
CI checks both backends on Linux,
macOS and Windows, plus memory/undefined-behavior and thread sanitizers on Linux.

Local validation on 2026-10-13 used an Apple M4 Pro (14 CPU cores, 48 GiB RAM),
macOS 27.0.1 arm64 and Apple Clang 21.0.0. Standalone Asio 1.38.2, Boost 1.90,
ASan/UBSan and TSan each passed 31 network/logging cases with 843 assertions. macOS
ASan ran without leak detection; Linux CI enables LeakSanitizer. These are
correctness checks, not capacity measurements.
The default C++20 application build, examples and all eight legacy test
executables also passed locally.

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

## Endpoint teardown

ARK's TCP, WebSocket and RUDP clients and servers may retain their wrapper while
an IO callback is running. When that callback releases the final owner, the owned
arknet endpoint is deleted by a shared cleanup executor with one thread. This
keeps worker joins outside the pool being stopped. Destruction from an external
thread remains synchronous; the cleanup executor drains its queued deletions at
shutdown. Application message and connection callbacks still run through `Update()`.
