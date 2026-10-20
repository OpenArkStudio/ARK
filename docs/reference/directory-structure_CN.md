# 项目结构与 SDK

ark 使用 C++20。核心与官方插件放在本仓库，业务插件放在应用仓库。
网络库继续维护在独立的 [arknet 仓库](https://github.com/OpenArkStudio/arknet)。

## 目录

构建产物归属于各自的 CMake 构建目录：`cmake -S . -B out/release` 将可执行文件、插件与归档输出到 `out/release/bin`。Debug 和 sanitizer 构建使用不同目录，避免产物互相覆盖。`-B build` 仍输出到 `build/bin`；部署时将所选构建的产物复制到运行目录的 `bin/`。

| 路径 | 内容 |
| --- | --- |
| `include/ark/` | 核心与插件公开头文件 |
| `src/core/` | 核心 CMake target 与 header-only 使用要求 |
| `src/app/` | 应用入口 |
| `src/proto/` | 协议定义与编解码实现 |
| `src/server/` | 内置服务端应用 |
| `include/ark/<name>/` | 官方插件公开头文件 |
| `src/plugins/<name>/` | 插件实现 |
| `tests/` | 核心、网络与私有数据库回归测试 |
| `tests/network/` | 网络与日志回归测试 |
| `tests/consumer/` | 独立使用公开 SDK 的示例项目 |
| `examples/` | 应用示例 |
| `build/` | 运行配置、资源、工具与[启动脚本](../../build/README_CN.md) |
| `deployment/` | 容器构建文件与 Kubernetes 示例 |

使用 `<ark/plugin_manager.hpp>`、`<ark/net/tcp_client.hpp>` 等公开路径包含头文件，
不需要把实现目录加入应用的包含路径。Protobuf 生成文件位于构建目录，由 `ark::proto` 提供。

## CMake 目标

| 目标 | 用途 |
| --- | --- |
| `ark::core` | 核心头文件、C++20 要求与公共编译定义 |
| `ark::net`、`ark::kernel`、`ark::utility` 等 | 插件头文件及其公开 API 依赖 |
| `ark::proto` | 生成的协议消息及其编译库，需要 `ARK_BUILD_PROTO=ON` |
| `ark::ark` | 多插件应用使用的聚合 SDK |

核心与插件 API 目标负责传递 CMake 编译要求，不负责加载运行时插件。
设置 `ARK_BUILD_APPLICATIONS=ON` 后，通过 `ARK_BUILD_PLUGIN_<NAME>` 选择要编译的插件动态库。
现有动态库文件名和配置中的插件名称保留；使用新 C++ API 的应用需要重新编译。

```cmake
add_subdirectory(path/to/ark ark-build)
add_executable(my_service main.cpp)
target_link_libraries(my_service PRIVATE ark::core ark::net)
```

默认编译 `kernel`、`net`、`bus`、`utility`、`etcd_client` 插件。
其他现有插件按需启用，并需要对应驱动依赖；目录迁移不代表这些集成已经通过验证。

## 命名与格式

- 命名空间、类型、函数和文件名使用 `snake_case`，例如 `ark::plugin_manager`、
  `find_module()`、`plugin_manager.hpp`。
- 私有和受保护成员以下划线结尾，例如 `module_name_`。
- include 顺序为系统头文件、第三方头文件、ark 头文件。
- 使用 clang-format 18 和仓库内的 `.clang-format`。第三方与生成文件不参与项目格式化。

## 验证 SDK 使用方式

初始化固定版本的子模块，并按[网络构建与回归测试](../networking_CN.md)安装依赖后，
编译独立的 SDK 使用示例：

```sh
cmake -S tests/consumer -B sdk-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DASIO_STANDALONE=OFF
cmake --build sdk-build --target check-sdk
```

该示例关闭应用和 Protobuf 构建，检查公开头文件路径、C++20 编译要求、模块继承和导出的 GUID 配置。
增加 `-DARK_USE_GUID_32=ON` 可检查 32 位 GUID 配置。

## 运行资源与生成数据

源码目录中的 `build/` 还保存运行资源，它与 `app-build/` 这样的独立 CMake 构建目录不同。

| 路径 | 用途 |
| --- | --- |
| `app-build/bin/` | 使用 `cmake -B app-build` 时的构建产物；各构建目录独立保存输出 |
| `build/bin/` | 复制到仓库运行目录的应用与插件二进制文件 |
| `build/binlog/` | 日志配置选用的运行日志目录 |
| `build/conf/` | 应用、插件与进程配置 |
| `build/resource/excel/` | 配置表格 |
| `build/resource/meta/`、`build/resource/server/` | 元数据定义与生成的服务端配置 |
| `build/resource/client/`、`build/resource/mysql/` | 生成的客户端配置与数据库结构资源 |
| `build/resource/code/` | 复制到公开头文件前的元数据生成代码 |
| `build/tools/` | 配置与运行环境准备工具 |

旧文中的 `bin/`、`frame/`、`dep/` 目录不是当前 SDK 布局。生成流程见[配置工具](../guides/tools_CN.md)，进程启动见[应用启动器](../guides/applications_CN.md)。协议公开头文件与兼容边界见 [Protobuf 消息](protobuf_CN.md)。
