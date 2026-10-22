# 依赖管理

[English](dependencies.md)

[vcpkg 清单](../vcpkg.json)固定统一的包注册表基线。CMake 通过导入 target 使用已安装的包，
仓库不再内置 `3rdparty/` 依赖源码，也不需要初始化依赖子模块。

| 构建范围 | vcpkg feature | 包与 CMake target |
| --- | --- | --- |
| 核心 | 始终安装 | `spdlog` → `spdlog::spdlog_header_only`；`readerwriterqueue` → `readerwriterqueue::readerwriterqueue`；`rapidxml` 头文件 |
| Boost 网络 | `boost`（默认） | `boost-asio`、`boost-beast` |
| 应用清单读取 | `config`（默认） | `tomlplusplus` → `tomlplusplus::tomlplusplus` |
| 应用 | `application` | `args` → `taywee::args`、Protobuf、zlib、Zstandard |
| etcd 客户端插件 | `etcd-client` | `nlohmann-json` → `nlohmann_json::nlohmann_json` |
| 原生数据库开发 | `database` | OpenSSL |

spdlog 的导入 target 传递外部 fmt 依赖与编译定义。`ark::core` 的使用方会继承这两个包，
无需手工添加头文件路径或定义。只有启用应用或 etcd 插件时，CMake 才分别查找 args 和 JSON；
JSON 是 etcd 插件的私有依赖。

应用构建默认启用 etcd 插件，因此使用
`-DVCPKG_MANIFEST_FEATURES="application;boost;etcd-client"`。不构建 etcd 时，设置
`-DARK_BUILD_PLUGIN_ETCD_CLIENT=OFF`，并省略 `etcd-client`。
清单 feature 负责选择依赖包，`ARK_BUILD_*` 选项负责选择 CMake 构建目标，两者不会自动互相启用。

只构建不读取 TOML 配置的核心时，可以通过 `-DVCPKG_MANIFEST_NO_DEFAULT_FEATURES=ON`
关闭默认 feature，设置 `-DARK_BUILD_CONFIG=OFF`，再按需选择网络 feature。
使用 Standalone Asio 且保留配置读取时，在 `-DASIO_STANDALONE=ON` 的同时显式设置
`-DVCPKG_MANIFEST_FEATURES=config`。

清单读取器优先使用已安装的 toml++ 3.4+ 包。未安装该包时，保留原有固定提交
`30172438cee64926dc41fdd9c11fb3ba5b2ba9de` 的获取路径；仅在找不到包时使用该路径，
正常的 vcpkg 配置构建不会安装第二份 toml++。
arknet 仍由根 CMake 文件固定提交；本地开发可以通过 `ARKNET_SOURCE_DIR` 提供源码。

## 可选的旧版集成

physic 插件与 physics_scene 依赖 PhysX 3，当前构建没有已验证的 SDK 集成。
选择它们时会明确报错，不会静默替换成现代 PhysX 包。
旧版 simulate 服务器也缺少已验证的 PhysX 3/MessagePack 集成，继续保持禁用。
已从默认禁用的 http_server 删除未使用的 BeastHttp 头文件目录；这不代表该示例已经通过运行验收。

原 cpp-bredis 子模块没有活动引用，已移除；原生 Redis 客户端由 arknet 提供。
已移除内置源码中的许可声明保留在[第三方许可声明](reference/third-party-notices_CN.md)中，
安装的包也会提供各自的版权文件。

各平台命令见[安装指南](get-started/installation_CN.md)，回归入口见[网络构建](networking_CN.md)。
