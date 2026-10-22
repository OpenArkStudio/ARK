# 项目结构与公共插件接口

ark 使用 C++20。核心与官方插件放在本仓库，业务插件放在应用仓库。
网络库继续维护在独立的 [arknet 仓库](https://github.com/OpenArkStudio/arknet)。

## 目录

构建产物归属于各自的 CMake 构建目录：`cmake -S . -B out/release` 将可执行文件、插件与归档输出到 `out/release/bin`。Debug 和 sanitizer 构建使用不同目录，避免产物互相覆盖。`-B build` 仍输出到 `build/bin`；部署时将所选构建的产物复制到运行目录的 `bin/`。

| 路径 | 内容 |
| --- | --- |
| `include/ark/` | 核心与其他插件的公开头文件 |
| `src/core/` | 核心 CMake target 与 header-only 使用要求 |
| `application/` | 应用入口 |
| `src/proto/` | 协议定义与编解码实现 |
| `src/server/` | 内置服务端应用 |
| `include/ark/<name>/` | 尚未迁移的官方插件公开头文件 |
| `src/plugins/<name>/` | 插件实现；MySQL/Redis/Utility/Net/Bus 的公开头和私有装配也位于此处 |
| `tests/` | 核心、网络与私有数据库回归测试 |
| `tests/network/` | 网络与日志回归测试 |
| `tests/consumer/` | 独立使用公共插件接口的示例项目 |
| `examples/` | 应用示例 |
| `config/` | 应用清单、插件和日志配置样例 |
| `resources/` | 业务数据、工作表与历史资源 |
| `tools/` | 离线配置工具与保留的历史工具 |
| `deployment/scripts/` | 单应用启动与显式 PID 停止脚本 |
| `deployment/` | 容器构建文件与 Kubernetes 示例 |

使用 `<ark/plugin_manager.hpp>`、`<ark/net/tcp_client.hpp>` 等公开路径包含头文件，
不需要把实现目录加入应用的包含路径。Protobuf 生成文件位于构建目录，由 `ark::proto` 提供。

MySQL、Redis 的唯一公开接口源码分别是 `src/plugins/mysql/mysql_module.hpp`、`src/plugins/redis/redis_module.hpp`。Utility 的公开接口与保留的调度辅助头位于 `src/plugins/utility/`；Net 的公开接口、端点模板与传输辅助类型位于 `src/plugins/net/`。Net 仅将私有插件装配与具体服务管理器定义排除在公开映射之外。Bus 的两个公开模块头位于 `src/plugins/bus/`；共享核心值类型 `<ark/bus.hpp>` 仍位于 `include/ark/`。CMake 在配置阶段按显式清单复制普通文件到各构建树的 `src/plugins/sdk-include/ark/<名称>/`；清理该受控目录中已移除的映射，不复制私有插件头或工厂。通过 `ark::mysql`、`ark::redis`、`ark::utility`、`ark::net`、`ark::bus` 消费，逻辑包含路径保持不变，插件关闭时同样有效。仅手写 `-I <源码>/include` 的调用需要改用目标传递的包含目录。

## CMake 目标

| 目标 | 用途 |
| --- | --- |
| `ark::core` | 核心头文件、C++20 要求与公共编译定义 |
| `ark::net`、`ark::kernel`、`ark::utility` 等 | 插件头文件及其公开 API 依赖 |
| `ark::proto` | 生成的协议消息及其编译库，需要 `ARK_BUILD_PROTO=ON` |
| `ark::ark` | 多插件应用使用的聚合接口目标 |

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

## 验证公共头文件使用方式

按[网络构建与回归测试](../networking_CN.md)安装依赖后，
编译独立的公共接口使用示例：

```sh
cmake -S tests/consumer -B sdk-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DASIO_STANDALONE=OFF -DARKNET_ENABLE_SSL=ON \
  -DVCPKG_MANIFEST_FEATURES=database
cmake --build sdk-build --target check-sdk
```

该示例关闭应用和 Protobuf 构建，检查公开头文件路径、C++20 编译要求、模块继承和导出的 GUID 配置。
增加 `-DARK_USE_GUID_32=ON` 可检查 32 位 GUID 配置。

消费者还验证 MySQL/Redis/Utility 的公开头映射，并拒绝可见的实现或私有装配头。上述命令启用 SSL 和 vcpkg `database` feature 所需的 OpenSSL，同时检查两种接口；关闭 SSL 时仅编译 Redis 接口。这是构建树消费检查，不是安装包验收。

仅使用 Net 的消费者启用 Protobuf，保持运行时插件关闭；分别编译 19 个 Net 公开头，
并确认私有管理器与插件头不可见：

```sh
cmake -S tests/consumer/net -B net-sdk-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES=application -DASIO_STANDALONE=OFF
cmake --build net-sdk-build --target check-sdk --parallel 2
```

仅使用 Bus 的消费者同样启用 Protobuf，分别编译两个映射的公开模块头，
确认私有实现、装配与解析头不可见。共享核心值类型 `<ark/bus.hpp>` 保持原有源码路径。

```sh
cmake -S tests/consumer/bus -B bus-sdk-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES=application -DASIO_STANDALONE=OFF
cmake --build bus-sdk-build --target check-sdk --parallel 2
```

## 运行资源与生成数据

源码资产与运行包分别组织。`build/` 只作为可选的 CMake 构建目录，不再保存受版本控制的运行文件。

| 源码路径 | 用途与部署位置 |
| --- | --- |
| `config/` | 配置样例；将所需文件复制到运行目录的 `conf/` |
| `resources/excel/` | 离线编辑的配置工作表 |
| `resources/meta/`、`resources/server/` | 生成的元数据与业务数据；部署到运行目录的 `resource/` |
| `resources/client/`、`resources/mysql/` | 客户端输出目录占位和数据库结构资源 |
| `resources/code/` | 复制到公开头文件前的元数据生成代码 |
| `tools/` | 配置导出器和保留的历史工具；状态见工具指南 |
| `deployment/scripts/` | 启动和停止脚本；可复制到运行目录，或显式传入运行目录 |

运行包继续使用 `bin/`、`conf/` 和按需提供的 `resource/`；日志由配置写入运行目录的 `binlog/`。源码目录改名不改变业务 XML、资源格式或运行时查找规则。见[运行脚本迁移](../guides/applications_CN.md#迁移旧命令)。

旧文中的 `bin/`、`frame/`、`dep/` 目录不是当前源码布局。生成流程见[配置工具](../guides/tools_CN.md)，进程启动见[应用启动器](../guides/applications_CN.md)。协议公开头文件与兼容边界见 [Protobuf 消息](protobuf_CN.md)。
