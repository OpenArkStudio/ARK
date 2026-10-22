# Build options

| Option | Default | Purpose |
| --- | --- | --- |
| `ARK_BUILD_APPLICATIONS` | `ON` | Build applications and the selected runtime plugins |
| `ARK_BUILD_PROTO` | Value of `ARK_BUILD_APPLICATIONS` | Generate and build protocol messages; applications require it |
| `ARK_BUILD_NETWORK_TESTS` | `OFF` | Build focused C++ network/logging regressions |
| `ARK_BUILD_TESTS` | `ON` | Build legacy test programs |
| `ARK_BUILD_CORE_TESTS` | Value of `ARK_BUILD_TESTS` | Build C++20 core and optional Protobuf regressions |
| `ARK_BUILD_EXAMPLES` | `ON` | Build enabled examples |
| `ARK_BUILD_PLUGIN_MYSQL` | `OFF` | Build the native MySQL coroutine plugin; requires `ARKNET_ENABLE_SSL=ON` |
| `ARK_BUILD_PLUGIN_REDIS` | `OFF` | Build the native Redis coroutine plugin; enable SSL for TLS pools |
| `ARK_BUILD_DATABASE_TESTS` | `OFF` | Build native database and public plugin regressions; requires an explicit `ARKNET_SOURCE_DIR` |
| `ARK_BUILD_BENCHMARKS` | `OFF` | Build standalone C++ benchmark programs |
| `ARK_USE_GUID_32` | `OFF` | Select the 32-bit GUID configuration; consumers inherit the setting through interface targets |
| `ARKNET_SOURCE_DIR` | Empty | Use an independent local arknet checkout |
| `ARKNET_REVISION` | Repository-pinned commit | Select the fetched arknet revision; an existing CMake cache retains its value |
| `ASIO_STANDALONE` | `OFF` | `OFF` selects Boost; `ON` selects Standalone Asio |

The project requires C++20 through CMake and disables compiler extensions. Applications and network tests require Boost.Asio and Boost.Beast. Standalone Asio is available for core-only and native database development builds; explicitly disable applications and network tests when selecting it. CMake rejects unsupported combinations before fetching dependencies. A local arknet checkout is a development override, not a copy inside ark.

When upgrading arknet, use a new build directory or explicitly change the cached revision. A nonempty `ARKNET_SOURCE_DIR` takes precedence over fetching the pin; see [Upgrade arknet](../get-started/installation.md#upgrade-arknet).

Official plugin options use `ARK_BUILD_PLUGIN_<NAME>`. The default runtime selection is `kernel`, `net`, `bus`, `utility` and `etcd_client`; enable the optional MySQL/Redis targets to build native coroutine plugins. Protocol and plugin-load tests do not replace real-server deployment acceptance. See [project layout](directory-structure.md), [protocol generation](protobuf.md) and [network builds](../networking.md).

With `ARK_BUILD_EXAMPLES` enabled, the [coroutine task example](../guides/tasks.md) also builds with applications disabled. Existing plugin examples still require application builds.

See [MySQL](mysql.md) and [Redis](redis.md) for plugin configuration and coroutine APIs.

## Native database plugins

With the dependencies from the installation guide available, build both optional runtime plugins:

```sh
cmake -S . -B build -DARK_BUILD_APPLICATIONS=ON \
  -DARK_BUILD_PLUGIN_MYSQL=ON -DARK_BUILD_PLUGIN_REDIS=ON -DARKNET_ENABLE_SSL=ON
cmake --build build --target ark_mysql_plugin ark_redis_plugin
```

The MySQL/Redis/Utility/Net/Bus public headers are copied from explicit lists at configuration time into each ark build tree's `src/plugins/sdk-include/ark/<name>/`. `ark::mysql`, `ark::redis`, `ark::utility`, `ark::net` and `ark::bus` carry that include root even with runtime plugins disabled; private assembly headers and factories are excluded. The source and generated include roots are distinct.

Link a consumer to `ark::mysql` and/or `ark::redis` for the public headers and usage requirements. These interface targets do not load a runtime library. Deploy the selected libraries with the launcher and add their stems to the application's manifest:

```toml
[application]
plugin_library_dir = "bin/"
plugin_config_dir = "conf/"

[[plugins]]
name = "ark_mysql_plugin"
config = ""

[[plugins]]
name = "ark_redis_plugin"
config = ""
```

Include only the providers the application needs. The empty `config` is intentional: consumers configure named connections through the public modules, rather than legacy database XML files. Follow [runtime library selection](../guides/plugins.md#select-libraries-at-runtime) for platform extensions, Debug suffixes and compatible interface settings. Resolve the module during consumer initialization and start its operations through a [managed task group](../guides/tasks.md).
