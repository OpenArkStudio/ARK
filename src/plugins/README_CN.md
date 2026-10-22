# 官方插件

[English](README.md)

核心与其他插件的公开头文件仍位于 `include/ark/`。MySQL、Redis、Utility、Net、Bus 和 Kernel 将公开头、实现与私有装配放在 `src/plugins/<名称>/` 同一目录；CMake 仅映射显式列出的公开头，保留 `<ark/mysql/...>`、`<ark/redis/...>`、`<ark/utility/...>`、`<ark/net/...>`、`<ark/bus/...>` 和 `<ark/kernel/...>` 逻辑包含路径。每个插件仍按独立 CMake target 构建，业务插件放在应用仓库。

| 插件 | 职责 | 默认构建 |
| --- | --- | --- |
| kernel | 实体数据、配置和事件服务 | 开启 |
| net | 通过 arknet 提供 TCP、RUDP、WebSocket 和 HTTP 服务 | 开启 |
| bus | 进程关系和消息路由 | 开启 |
| utility | ID、定时器、调度和压缩 | 开启 |
| etcd_client | etcd 服务访问 | 开启 |
| http | HTTP 服务端模块 | 关闭 |
| consul | Consul 服务发现 | 关闭 |
| kafka | Kafka 消息服务 | 关闭 |
| physic | 物理服务 | 关闭 |
| mysql、redis | arknet 原生数据库连接池与公开协程接口 | 关闭 |

使用 `ARK_BUILD_PLUGIN_<名称大写>` 选择插件。可选插件需要各自的依赖和验证。[MySQL](../../docs/reference/mysql_CN.md) 提供查询、预处理语句和事务；[Redis](../../docs/reference/redis_CN.md) 提供命令和有序流水线。两者均通过具名配置和管理器托管的协程任务使用。构建与运行时选择见[数据库插件启用方式](../../docs/reference/build-options_CN.md#原生数据库插件)。Lua、Actor 的整合仍在规划中。

CMake target 和构建命令见[项目布局与公共接口](../../docs/project-layout_CN.md)。

[Utility 参考](../../docs/reference/utility_CN.md)说明四个模块接口、所属线程驱动的定时器与实现头迁移。

Kernel 的原有 38 个头与 cpp 共置于 `src/plugins/kernel/`，通过 `ark::kernel` 保留全部 `<ark/kernel/...>` 路径，包括 `data_impl.hpp`、`data_list_impl.hpp` 和 `kernel_plugin.hpp`。此次只移动源码，创建宏、注册方式、类型可见性和生命周期均不变。消费检查及 Protobuf 条件见 [Kernel 参考](../../docs/reference/kernel_CN.md)。
