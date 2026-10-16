# ark

ark 是用于构建分布式应用的 C++20 框架。应用组合插件，插件注册模块，模块提供业务与基础设施接口。

- **组合方式**：应用、插件与面向接口的模块。
- **网络能力**：通过独立的 [arknet 网络库](https://openarkstudio.github.io/arknet/zh/)提供 TCP、WebSocket、HTTP 和 RDP。
- **开发工具**：CMake 构建、正式 C++ 测试用例和跨平台 CI。

<Aside type="note" title="说明：选择合适的层级">
使用 ark 组织应用服务；需要网络库时可直接使用 arknet。IO 执行由 arknet 负责，ark 通过 `update()` 循环分发业务回调。
</Aside>
