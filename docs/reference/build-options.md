# Build options

| Option | Default | Purpose |
| --- | --- | --- |
| `ARK_BUILD_APPLICATIONS` | `ON` | Build applications and the selected runtime plugins |
| `ARK_BUILD_PROTO` | Value of `ARK_BUILD_APPLICATIONS` | Generate and build protocol messages; applications require it |
| `ARK_BUILD_NETWORK_TESTS` | `OFF` | Build focused C++ network/logging regressions |
| `ARK_BUILD_TESTS` | `ON` | Build legacy test programs |
| `ARK_BUILD_CORE_TESTS` | Value of `ARK_BUILD_TESTS` | Build C++20 core and optional Protobuf regressions |
| `ARK_BUILD_EXAMPLES` | `ON` | Build enabled examples |
| `ARK_BUILD_BENCHMARKS` | `OFF` | Build standalone C++ benchmark programs |
| `ARK_USE_GUID_32` | `OFF` | Select the 32-bit GUID configuration; consumers inherit the setting through SDK targets |
| `ARKNET_SOURCE_DIR` | Empty | Use an independent local arknet checkout |
| `ASIO_STANDALONE` | `OFF` | `OFF` selects Boost; `ON` selects Standalone Asio |

The project requires C++20 through CMake and disables compiler extensions. Applications and network tests require Boost.Asio and Boost.Beast. Standalone Asio is available for core-only and native database development builds; explicitly disable applications and network tests when selecting it. CMake rejects unsupported combinations before fetching dependencies. A local arknet checkout is a development override, not a copy inside ark.

Official plugin options use `ARK_BUILD_PLUGIN_<NAME>`. The default runtime selection is `kernel`, `net`, `bus`, `utility` and `etcd_client`; optional database-plugin directories do not establish driver replacement or runtime acceptance. See [project layout](directory-structure.md), [protocol generation](protobuf.md) and [network builds](../networking.md).

With `ARK_BUILD_EXAMPLES` enabled, the [coroutine task example](../guides/tasks.md) also builds with applications disabled. Existing plugin examples still require application builds.
