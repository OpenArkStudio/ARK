# Cluster、Router 与 Zone 设计说明

<Aside type="note" title="草案">

本页解释历史拓扑与远端日志提案，不定义已发布的服务发现、路由、高可用或日志交付契约。当前连接与消息 API 见 [Bus](../reference/bus_CN.md)。

</Aside>

## 提议中的进程职责

原笔记将进程分为三个层次：

| 层次 | 设想中的职责 |
| --- | --- |
| `cluster` | 跨区共享服务，由 Master 管理集群拓扑 |
| `router` | 区服与集群进程之间的消息转发中间层 |
| `zone` | 小区进程，由 World 管理区内拓扑 |

![Cluster、Router 与 Zone 的参考职责](../assets/server-topology_CN.svg)

设想中的路由规则要求区内消息在本区处理。发往集群的消息可以经过 Router，由其依据历史包头中的 `dst_bus_id` 目标选择路由。集群内所有进程连接 Master 与 Router，部分区服进程连接 Router。

这些是职责与连接关系的提议，没有由此定义鉴权、路由收敛、重试、拥塞控制、重复交付或状态恢复。[服务器架构](../concepts/server-architecture_CN.md)说明仓库示例角色与部署参考之间的关系。

## 历史远端日志设想

笔记还提出：所有进程先与日志服务建立连接，再开始业务工作；日志客户端配置包含远端地址、端口与日志格式。

这个启动门槛是设想，不是框架已实现的全局要求。首次连接成功，本身也无法保证后续全部日志被交付或保留。交付确认、缓冲限额、背压、鉴权、重连，以及日志服务不可用时的处理，都仍需明确策略。本页不代替这些选型。

## 使用当前框架时的边界

应用可以组合现有角色插件，并配置网络与 Bus 关系。应依据实际配置和 API 使用，不能从进程名称推断行为。部署多个区服不会自动要求使用 Consul 或自研 Registry；Registry 的一致性、租约与故障切换仍是独立设计事项。

此提案不证明跨主机容量、拓扑自动恢复、存储一致性或远端日志无丢失。这些需要明确的设计要求与部署测试，详见[基准测试与验收](../performance/benchmark_CN.md)。

中文[原始笔记](https://github.com/OpenArkStudio/ark/blob/main/docs/site/legacy-source/design/network.md)保留在[源码归档](https://github.com/OpenArkStudio/ark/blob/main/docs/site/legacy-source/README.md)中。英文页是对历史内容的现代说明，不表示存在历史英文原文，也不表示已经接受其中的实现决策。
