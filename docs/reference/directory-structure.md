# Project layout and SDK

ark uses C++20. Core headers and official plugins live in this repository;
application-specific business plugins live with their application. The network
library remains in the independent [arknet repository](https://github.com/OpenArkStudio/arknet).

## Directories

Build artifacts belong to each CMake build directory: `cmake -S . -B out/release` produces executables, plugins and archives under `out/release/bin`. Use another directory for Debug or sanitizer builds to keep their artifacts separate. A build with `-B build` continues to use `build/bin`. Deployment copies the selected build's artifacts into the runtime's `bin/` directory.

| Path | Contents |
| --- | --- |
| `include/ark/` | Public core and plugin headers |
| `src/core/` | Core CMake target and header-only usage requirements |
| `src/app/` | Application entry point |
| `src/proto/` | Protocol schemas and codec implementation |
| `src/server/` | Built-in server applications |
| `include/ark/<name>/` | Public headers for an official plugin |
| `src/plugins/<name>/` | Plugin implementation |
| `tests/` | Core, network and private database regressions |
| `tests/network/` | Network and logging regressions |
| `tests/consumer/` | An independent project consuming the public SDK |
| `examples/` | Application examples |
| `build/` | Runtime configuration, resources, tools and [launch helpers](../../build/README.md) |
| `deployment/` | Container recipes and Kubernetes examples |

Include public headers as `<ark/plugin_manager.hpp>` or `<ark/net/tcp_client.hpp>`.
Do not add implementation directories to an application's include path. Generated
Protobuf headers are placed under the build directory, exposed through `ark::proto`.

## CMake targets

| Target | Usage |
| --- | --- |
| `ark::core` | Core headers, C++20 requirements and shared compile definitions |
| `ark::net`, `ark::kernel`, `ark::utility`, etc. | Headers and transitive public API dependencies of a plugin |
| `ark::proto` | Generated protocol messages and their compiled library; requires `ARK_BUILD_PROTO=ON` |
| `ark::ark` | Aggregate SDK for applications using multiple official plugins |

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

## Verify SDK consumption

Install the dependencies described in
[Network builds and regression tests](../networking.md), then build the independent consumer:

```sh
cmake -S tests/consumer -B sdk-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DASIO_STANDALONE=OFF
cmake --build sdk-build --target check-sdk
```

The consumer builds with applications and Protobuf disabled. It verifies public
include paths, inherited C++20 settings, module overrides and exported GUID configuration.
Use `-DARK_USE_GUID_32=ON` to check the 32-bit GUID configuration.

## Runtime resources and generated data

The source checkout's `build/` directory also contains runtime material; it is separate from an out-of-tree CMake directory such as `app-build/`.

| Path | Purpose |
| --- | --- |
| `app-build/bin/` | Build artifacts when configuring with `cmake -B app-build`; each build directory owns its output |
| `build/bin/` | Application and plugin binaries copied into the checked-in runtime layout |
| `build/binlog/` | Runtime log output, when selected by logging configuration |
| `build/conf/` | Application, plugin and process configuration |
| `build/resource/excel/` | Configuration spreadsheets |
| `build/resource/meta/`, `build/resource/server/` | Metadata definitions and generated server configuration |
| `build/resource/client/`, `build/resource/mysql/` | Generated client configuration and database schema resources |
| `build/resource/code/` | Generated metadata source before it is copied into the public headers |
| `build/tools/` | Configuration and runtime preparation tools |

The historical `bin/`, `frame/` and `dep/` tree is not the current SDK layout. See [configuration tools](../guides/tools.md) for generation and [application launcher](../guides/applications.md) for process startup. Public protocol headers and compatibility boundaries are documented in [Protobuf messages](protobuf.md).
