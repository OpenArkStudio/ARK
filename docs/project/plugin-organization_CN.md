# 官方插件与仓库组织

核心与插件的公开接口统一放在 `include/ark/`，核心构建定义放在 `src/core/`，官方插件实现放在 `src/plugins/<名称>/`。核心与官方插件纳入 `ark`，业务插件放在应用仓库，`arknet` 保持独立。业务热更新的设计与实现暂缓，目录与 snake_case 整理已经落地，独立插件的功能整合仍按插件分别验收。

## 现有实现

| 独立仓库 | ark 内的相关实现 | 建议合入位置 | 需要整理的内容 |
| --- | --- | --- | --- |
| [MySQLPlugin](https://github.com/OpenArkStudio/MySQLPlugin) | `src/plugins/mysql` | `src/plugins/mysql` | [原生协程接口](../reference/mysql_CN.md)、预处理语句、事务和共享业务执行器；部署仍需真实 MySQL 验收 |
| [RedisPlugin](https://github.com/OpenArkStudio/RedisPlugin) | `src/plugins/redis` | `src/plugins/redis` | [原生协程接口](../reference/redis_CN.md)、具名连接池、RESP/TLS 和正常停机；部署仍需真实 Redis 验收 |
| [URLPlugin](https://github.com/OpenArkStudio/URLPlugin) | Net HTTP client 与 HTTP server 插件 | `src/plugins/http` | 对照 libcurl 版本的请求、编码与 actor 调用能力，统一 HTTP 服务接口 |
| [LuaScriptPlugin](https://github.com/OpenArkStudio/LuaScriptPlugin) | 需要按新插件接口接入 | `src/plugins/lua` | 整理 Lua 绑定与脚本生命周期，移除对旧 NFComm 接口的依赖 |
| [ActorPlugin](https://github.com/OpenArkStudio/ActorPlugin) | 需要按新插件接口接入 | `src/plugins/actor` | 明确任务调度、消息投递和业务线程的关系 |

这些仓库仍保留不同代际的接口和构建方式。合入时逐项对照功能、重叠实现与依赖。MySQL、Redis 客户端归入 arknet，其他插件的依赖按各自需求讨论。

## 目录布局

```text
ark/
├── application/         # 应用启动器
├── include/ark/         # 核心与插件公开接口
├── src/
│   ├── core/            # header-only 核心 target 定义
│   ├── plugins/         # 官方插件实现与 CMake 文件
│   │   ├── kernel/
│   │   ├── net/
│   │   ├── bus/
│   │   ├── http/
│   │   ├── mysql/
│   │   └── redis/
│   ├── proto/
│   └── server/
├── examples/            # 示例应用与业务插件
├── tests/               # 核心与跨插件集成测试
├── benchmarks/          # 可独立编译运行的基准程序
├── tools/               # 开发与配置工具
├── cmake/
└── docs/                # 中英文文档与 Nimbus 站点
```

上图列出主要目录。Consul、etcd、Kafka 等现有官方插件也按同一规则组织。Lua、Actor 的接入仍在规划中，上表列出的是它们的建议实现位置。

每个插件采用相同的目录布局，以 MySQL 为例：

```text
include/ark/mysql/      # 使用该插件时需要的公开接口
src/plugins/mysql/
├── CMakeLists.txt
├── detail/             # 私有原生数据库桥接
└── *.cpp               # 插件入口与框架接入
tests/database/         # 数据库集成与协程测试
```

插件使用说明放在 `docs/`，示例应用放在 `examples/`。公共接口按能力定义，数据库客户端类型尽量保留在实现内。

## 核心与插件的边界

| 部分 | 职责 | 构建与发布 |
| --- | --- | --- |
| 核心运行时 | 插件与模块注册、依赖顺序及生命周期 | 与官方插件统一验证接口兼容性 |
| 官方插件 | 网络、存储、脚本、服务发现等可选服务 | 独立 CMake target，按需启用和打包 |
| 业务插件 | 产品规则与业务消息处理 | 由应用仓库独立构建和发布 |
| arknet | IO 执行、网络端点、传输协议及数据库客户端 | 独立版本，由 ark 固定依赖版本 |

启用某个插件时才解析其专属依赖；框架公共头文件避免包含 MySQL、Redis、Lua 等客户端头文件。编译依赖和运行时服务依赖需要明确区分，例如启用 Redis 插件的测试还需要可用的 Redis 服务。

## 数据库客户端的职责

MySQL 和 Redis 客户端在 arknet 中自主实现，不采用 Boost.MySQL 或 Boost.Redis。arknet 负责数据库协议编解码、认证和异步通信；ark 的插件负责框架配置、模块服务和业务回调接入。

arknet 的单节点客户端范围为：MySQL 提供查询、参数化预处理和事务；Redis 提供通用命令、RESP2／RESP3 和流水线请求。两者包含认证、TLS、连接池、超时、取消和队列上限。Redis Cluster、Sentinel 和 Pub/Sub 后续再加。原生客户端已随 [arknet 0.2.0](https://github.com/OpenArkStudio/arknet/releases/tag/v0.2.0) 发布，包含独立服务端互通验证和本机性能报告；这不代表 ark 的 MySQL、Redis 插件已经切换到新客户端。插件接入及其业务回归仍待完成。

## 接下来讨论

1. 保持已统一的 snake_case SDK 和目录结构；公开头文件与 CMake target 见[项目结构](../reference/directory-structure_CN.md)。
2. MySQL、Redis 插件采用 Asio 协程和 arknet 原生拥有型结果。arknet 提供调用方驱动的执行器，ark 在模块线程中派发；完成配置、事务、取消和关闭流程验证后，再启用运行时插件。
3. 分别确认 Lua、Actor 等官方插件的接口和依赖，按插件补齐编译、功能和异常路径验证。

热更新相关的状态持有、调用完成判断和兼容边界，待当前改造完成后继续讨论。
