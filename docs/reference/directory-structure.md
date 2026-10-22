# Project layout and public plugin interfaces

ark uses C++20. Core headers and official plugins live in this repository;
application-specific business plugins live with their application. The network
library remains in the independent [arknet repository](https://github.com/OpenArkStudio/arknet).

## Directories

Build artifacts belong to each CMake build directory: `cmake -S . -B out/release` produces executables, plugins and archives under `out/release/bin`. Use another directory for Debug or sanitizer builds to keep their artifacts separate. A build with `-B build` continues to use `build/bin`. Deployment copies the selected build's artifacts into the runtime's `bin/` directory.

| Path | Contents |
| --- | --- |
| `include/ark/` | Public core and other plugin headers |
| `src/core/` | Core CMake target and header-only usage requirements |
| `application/` | Application entry point |
| `src/proto/` | Protocol schemas and codec implementation |
| `src/server/` | Built-in server applications |
| `include/ark/<name>/` | Public headers for plugins outside the co-located set |
| `src/plugins/<name>/` | Implementation; MySQL/Redis/Utility/Net/Bus/HTTP/Kernel also keep public headers and private assembly here |
| `tests/` | Core, network and private database regressions |
| `tests/network/` | Network and logging regressions |
| `tests/consumer/` | An independent project consuming the public plugin interfaces |
| `examples/` | [Capability examples](../guides/examples.md); hpp/cpp files stay adjacent |
| `config/` | Application manifest, plugin and logger configuration samples |
| `resources/` | Business data, workbooks and legacy resources |
| `tools/` | Offline configuration tools and retained legacy tools |
| `deployment/scripts/` | Single-application launch and explicit-PID stop helpers |
| `deployment/` | Container recipes and Kubernetes examples |

Include public headers as `<ark/plugin_manager.hpp>` or `<ark/net/tcp_client.hpp>`.
Do not add implementation directories to an application's include path. Generated
Protobuf headers are placed under the build directory, exposed through `ark::proto`.

The MySQL/Redis interfaces have one source each: `src/plugins/mysql/mysql_module.hpp` and `src/plugins/redis/redis_module.hpp`. Utility public interfaces and retained scheduler helpers live in `src/plugins/utility/`; Net public interfaces, endpoint templates and transport helpers live in `src/plugins/net/`. Net excludes only private plugin assembly and the concrete service-manager definition from its public mapping. Bus keeps its two public module headers in `src/plugins/bus/`; shared `<ark/bus.hpp>` core value types remain under `include/ark/`. HTTP maps its public `src/plugins/http/http_server_module.hpp`, excluding the private plugin and concrete class. At configuration time CMake copies ordinary files from explicit lists into each ark build tree's `src/plugins/sdk-include/ark/<name>/`, removing obsolete mappings only within those controlled directories. Other migrated plugins exclude private assembly and factories; Kernel retains all 38 existing headers, including `kernel_plugin.hpp`, without changing concrete-type or assembly visibility. Consume `ark::mysql`, `ark::redis`, `ark::utility`, `ark::net`, `ark::bus`, `ark::http` and `ark::kernel` to retain the logical include paths, including when runtime plugins are disabled. Commands using only handwritten `-I <source>/include` must adopt the target-provided include roots. The mapped copies are generated build artifacts; edit the co-located source headers.

## CMake targets

| Target | Usage |
| --- | --- |
| `ark::core` | Core headers, C++20 requirements and shared compile definitions |
| `ark::net`, `ark::kernel`, `ark::utility`, etc. | Headers and transitive public API dependencies of a plugin |
| `ark::proto` | Generated protocol messages and their compiled library; requires `ARK_BUILD_PROTO=ON` |
| `ark::ark` | Aggregate public interface target for applications using multiple official plugins |

The core and plugin API targets carry CMake usage requirements. They do not load
runtime plugins. Shared plugin libraries are built with `ARK_BUILD_APPLICATIONS=ON`
and selected with `ARK_BUILD_PLUGIN_<NAME>`. Existing shared-library filenames and
plugin configuration names are retained; applications must be rebuilt for the new C++ API.

```cmake
add_subdirectory(path/to/ark ark-build)
add_executable(my_service main.cpp)
target_link_libraries(my_service PRIVATE ark::core ark::net)
```

The default runtime plugin selection is `kernel`, `net`, `bus`, `utility` and
`etcd_client`. Other existing plugins are opt-in and require their driver dependencies;
the directory migration alone does not validate those integrations.

## Naming and formatting

- Namespaces, types, functions and filenames use `snake_case`: `ark::plugin_manager`,
  `find_module()`, `plugin_manager.hpp`.
- Private and protected members end in `_`, for example `module_name_`.
- Include system headers first, followed by third-party headers, then ark headers.
- Use clang-format 18 and the checked-in `.clang-format`. Third-party and generated
  files are excluded from project formatting.

See [Code style](../project/code-style.md) for complete naming conventions, local checks and protocol migration details.

## Verify public-header consumption

Install the dependencies described in
[Network builds and regression tests](../networking.md), then build the independent consumer:

```sh
cmake -S tests/consumer -B sdk-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DASIO_STANDALONE=OFF -DARKNET_ENABLE_SSL=ON \
  -DVCPKG_MANIFEST_FEATURES=database
cmake --build sdk-build --target check-sdk
```

The consumer builds with applications and Protobuf disabled. It verifies public
include paths, inherited C++20 settings, module overrides and exported GUID configuration.
Use `-DARK_USE_GUID_32=ON` to check the 32-bit GUID configuration.

The consumer also checks MySQL/Redis/Utility public-header mapping and rejects visible implementation or private assembly headers. The command enables SSL and the vcpkg `database` feature's OpenSSL dependency to check both interfaces; with SSL disabled, only Redis is compiled. This checks public-header consumption from the selected build tree.

The Net-only consumer enables Protobuf while keeping runtime plugins disabled.
It compiles all 19 Net public headers independently and excludes the private
manager/plugin headers:

```sh
cmake -S tests/consumer/net -B net-sdk-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES=application -DASIO_STANDALONE=OFF
cmake --build net-sdk-build --target check-sdk --parallel 2
```

The Bus-only consumer also enables Protobuf and compiles the two mapped public
module headers independently, without exposing private implementation, assembly
or parser headers. Shared `<ark/bus.hpp>` core value types keep their source path.

```sh
cmake -S tests/consumer/bus -B bus-sdk-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES=application -DASIO_STANDALONE=OFF
cmake --build bus-sdk-build --target check-sdk --parallel 2
```

The HTTP-only consumer needs no Protobuf or runtime plugin. It compiles the mapped public module and router API while excluding private assembly/parser headers.

```sh
cmake -S tests/consumer/http -B http-sdk-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DASIO_STANDALONE=OFF
cmake --build http-sdk-build --target check-sdk --parallel 2
```

## Runtime resources and generated data

Source assets and staged runtimes have separate layouts. `build/` is only an optional CMake build directory; it no longer contains tracked runtime files.

| Source path | Purpose and runtime destination |
| --- | --- |
| `config/` | Configuration samples; copy the selected files into the runtime's `conf/` |
| `resources/excel/` | Workbooks edited offline |
| `resources/meta/`, `resources/server/` | Generated metadata and business data; stage under the runtime's `resource/` |
| `resources/client/`, `resources/mysql/` | Client-output placeholders and database schema resources |
| `resources/code/` | Generated metadata source before copying into the public headers |
| `tools/` | Configuration exporter and retained legacy tools; see the tools guide for status |
| `deployment/scripts/` | Launch/stop helpers; copy into the runtime or pass its directory explicitly |

Runtime packages still use `bin/`, `conf/` and optional `resource/`; logging configuration writes to the runtime's `binlog/`. Source moves do not change business XML, resource formats or runtime path resolution. See [helper migration](../guides/applications.md#migrate-old-helper-commands).

The historical `bin/`, `frame/` and `dep/` tree is not the current source layout. See [configuration tools](../guides/tools.md) for generation and [application launcher](../guides/applications.md) for process startup. Public protocol headers and compatibility boundaries are documented in [Protobuf messages](protobuf.md).

Kernel co-locates its 38 existing headers with its cpp files in `src/plugins/kernel/`. `ark::kernel` preserves every `<ark/kernel/...>` path, including `data_impl.hpp`, `data_list_impl.hpp` and `kernel_plugin.hpp`. This source-only move keeps creation macros, registration, type visibility and lifecycle unchanged. See [Kernel reference](kernel.md) for consumption checks and Protobuf requirements.
