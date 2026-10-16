# 服务器架构

参考拓扑将目录与发现、全局服务、路由、小区和存储分开部署。图中展示进程职责，不代表已经测得的生产容量。

![参考进程拓扑](../assets/server-architecture.png)

- 目录服务确定目标环境。
- Master 与共享服务承担全区职责。
- Router 连接小区与共享服务。
- World、Game、Proxy 和 Login 等进程组成小区。
- 存储服务具有独立的扩容与故障边界。

[路由设计提案](../project/network-design_CN.md)说明 cluster、router 和 zone 的划分。具体部署、发现与恢复策略需要结合应用配置和验证。
