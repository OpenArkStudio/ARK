# 构建选项

| 选项 | 默认值 | 用途 |
| --- | --- | --- |
| `ARK_BUILD_APPLICATIONS` | `ON` | 构建应用及选中的运行时插件 |
| `ARK_BUILD_PROTO` | 与 `ARK_BUILD_APPLICATIONS` 一致 | 生成并构建协议消息；构建应用时必须启用 |
| `ARK_BUILD_NETWORK_TESTS` | `OFF` | 构建独立 C++ 网络与日志回归测试 |
| `ARK_BUILD_TESTS` | `ON` | 构建原有测试程序 |
| `ARK_BUILD_CORE_TESTS` | 与 `ARK_BUILD_TESTS` 一致 | 构建 C++20 核心与可选的 Protobuf 回归测试 |
| `ARK_BUILD_EXAMPLES` | `ON` | 构建选中的示例 |
| `ARK_BUILD_BENCHMARKS` | `OFF` | 构建独立 C++ 基准程序 |
| `ARK_USE_GUID_32` | `OFF` | 使用 32 位 GUID 配置；SDK 目标向调用方传递该设置 |
| `ARKNET_SOURCE_DIR` | 空 | 使用独立的本地 arknet 源码 |
| `ARKNET_REVISION` | 仓库固定的 commit | 指定获取 arknet 时使用的版本；已有 CMake 缓存会保留原值 |
| `ASIO_STANDALONE` | `OFF` | `OFF` 选择 Boost，`ON` 选择 Standalone Asio |

项目通过 CMake 要求 C++20，并禁用编译器扩展。应用和网络测试需要 Boost.Asio 与 Boost.Beast。仅构建核心或原生数据库开发测试时可以选择 Standalone Asio，须明确关闭应用和网络测试；不支持的组合会在获取依赖前由 CMake 报错。本地 arknet 路径是开发覆盖项，不会把网络库复制进 ark。

升级 arknet 时，使用新的构建目录，或显式修改缓存中的引用。`ARKNET_SOURCE_DIR` 非空时优先使用本地源码，不按固定 commit 获取依赖；具体操作见[升级 arknet](../get-started/installation_CN.md#升级-arknet)。

官方插件选项采用 `ARK_BUILD_PLUGIN_<NAME>`。默认运行时插件为 `kernel`、`net`、`bus`、`utility` 和 `etcd_client`；可选数据库插件已整理目录，不代表驱动替换和运行验收已经完成。参见[项目结构](directory-structure_CN.md)、[协议生成](protobuf_CN.md)与[网络构建](../networking_CN.md)。

开启 `ARK_BUILD_EXAMPLES` 时，[协程任务示例](../guides/tasks_CN.md)可以在关闭应用的情况下独立构建；其余现有插件示例仍要求开启应用构建。
