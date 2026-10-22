# 构建选项

| 选项 | 默认值 | 用途 |
| --- | --- | --- |
| `ARK_BUILD_APPLICATIONS` | `ON` | 构建应用及选中的运行时插件 |
| `ARK_BUILD_PROTO` | 与 `ARK_BUILD_APPLICATIONS` 一致 | 生成并构建协议消息；构建应用时必须启用 |
| `ARK_BUILD_NETWORK_TESTS` | `OFF` | 构建独立 C++ 网络与日志回归测试 |
| `ARK_BUILD_TESTS` | `ON` | 构建原有测试程序 |
| `ARK_BUILD_CORE_TESTS` | 与 `ARK_BUILD_TESTS` 一致 | 构建 C++20 核心与可选的 Protobuf 回归测试 |
| `ARK_BUILD_EXAMPLES` | `ON` | 构建选中的示例 |
| `ARK_BUILD_PLUGIN_MYSQL` | `OFF` | 构建原生 MySQL 协程插件，要求 `ARKNET_ENABLE_SSL=ON` |
| `ARK_BUILD_PLUGIN_REDIS` | `OFF` | 构建原生 Redis 协程插件，TLS 连接池需要开启 SSL |
| `ARK_BUILD_DATABASE_TESTS` | `OFF` | 构建原生数据库及公开插件回归，要求显式设置 `ARKNET_SOURCE_DIR` |
| `ARK_BUILD_BENCHMARKS` | `OFF` | 构建独立 C++ 基准程序 |
| `ARK_USE_GUID_32` | `OFF` | 使用 32 位 GUID 配置；公共接口目标向调用方传递该设置 |
| `ARKNET_SOURCE_DIR` | 空 | 使用独立的本地 arknet 源码 |
| `ARKNET_REVISION` | 仓库固定的 commit | 指定获取 arknet 时使用的版本；已有 CMake 缓存会保留原值 |
| `ASIO_STANDALONE` | `OFF` | `OFF` 选择 Boost，`ON` 选择 Standalone Asio |

项目通过 CMake 要求 C++20，并禁用编译器扩展。应用和网络测试需要 Boost.Asio 与 Boost.Beast。仅构建核心或原生数据库开发测试时可以选择 Standalone Asio，须明确关闭应用和网络测试；不支持的组合会在获取依赖前由 CMake 报错。本地 arknet 路径是开发覆盖项，不会把网络库复制进 ark。

升级 arknet 时，使用新的构建目录，或显式修改缓存中的引用。`ARKNET_SOURCE_DIR` 非空时优先使用本地源码，不按固定 commit 获取依赖；具体操作见[升级 arknet](../get-started/installation_CN.md#升级-arknet)。

官方插件选项采用 `ARK_BUILD_PLUGIN_<NAME>`。默认运行时插件为 `kernel`、`net`、`bus`、`utility` 和 `etcd_client`；启用 MySQL 或 Redis 构建选项可使用原生协程插件；本机协议测试与插件加载检查不能替代真实数据库部署验收。参见[项目结构](directory-structure_CN.md)、[协议生成](protobuf_CN.md)与[网络构建](../networking_CN.md)。

开启 `ARK_BUILD_EXAMPLES` 时，[协程任务示例](../guides/tasks_CN.md)可以在关闭应用的情况下独立构建；其余现有插件示例仍要求开启应用构建。

数据库插件的配置与接口见 [MySQL](mysql_CN.md) 和 [Redis](redis_CN.md)。

## 原生数据库插件

按安装指南准备依赖后，可同时构建两个可选插件：

```sh
cmake -S . -B build -DARK_BUILD_APPLICATIONS=ON \
  -DARK_BUILD_PLUGIN_MYSQL=ON -DARK_BUILD_PLUGIN_REDIS=ON -DARKNET_ENABLE_SSL=ON
cmake --build build --target ark_mysql_plugin ark_redis_plugin
```

MySQL、Redis、Utility、Net、Bus、Kernel 的公开头通过显式清单在配置阶段复制到各构建树的 `src/plugins/sdk-include/ark/<名称>/`。`ark::mysql`、`ark::redis`、`ark::utility`、`ark::net`、`ark::bus`、`ark::kernel` 传递该目录，插件关闭时仍可使用；其他已迁移插件的私有装配头和工厂不进入映射；Kernel 保留包括 `kernel_plugin.hpp` 在内的原有 38 个头，注册方式和类型可见性不变。源码目录与构建目录不能互相替代。

业务插件链接 `ark::mysql` 和/或 `ark::redis`，以获取公开头文件和编译要求；公共接口目标不会加载运行时动态库。将所需动态库与启动器一起部署，并在应用清单中填写库名称：

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

只保留应用所需的插件。这里的 `config` 为空：业务模块通过公开接口配置具名连接，不再使用旧版数据库 XML 文件。平台扩展名、Debug 后缀和公共接口兼容要求见[运行时选择动态库](../guides/plugins_CN.md#在运行时选择动态库)。在业务模块初始化时查找数据库模块，再通过[托管任务组](../guides/tasks_CN.md)启动操作。
