# Net 插件

网络插件将独立的 arknet 网络库接入 ark，处理业务拆包、Session ID 与 Update 线程上的业务回调。

| 传输 | 客户端 | 服务端 |
| --- | --- | --- |
| TCP | `tcp_client` | `tcp_server` |
| WebSocket | `websocket_client` | `websocket_server` |
| RUDP | `rudp_client` | `rudp_server` |
| HTTP | `http_client_module_impl` | `http_server` |

`net` 定义启动、关闭、发送和 Update 操作。业务消息继续使用现有消息头；RUDP 使用 arknet 兼容的 ARQ 报文格式，默认关闭 FEC。

构建命令和验证范围见[网络构建与测试](../networking_CN.md)。接口见 [ark::net](https://github.com/OpenArkStudio/ark/blob/main/src/plugins/net/net.hpp)，协议能力见 [arknet 指南](https://openarkstudio.github.io/arknet/zh/features/)。

可选的[HTTP 服务端插件](http_CN.md)公开 `http_server_module`，其路由处理器在 arknet IO 工作线程上运行。

## API 与私有创建

Net 的全部源头文件与实现位于 `src/plugins/net/`。`ark::net` API 目标通过显式清单，
将 19 个公开头映射到 `<ark/net/...>`，并传递公开依赖。包含协议消息的服务头需要
`ARK_BUILD_PROTO=ON`；此时 `ark::net` 会传递 `ark::proto`。直接端点头仍可在关闭协议生成时使用。
API 目标本身不会构建或加载运行时动态库。

私有插件采用 `plugin_base`，工厂返回 `std::unique_ptr<net_service_manager_module>`，
具体类型在 cpp 中继承 `module_base<net_service_manager_module>`。查找继续使用公开接口的
RTTI 键，HTTP 客户端也按其公开接口键注册。两个模块仍参与更新；HTTP 先安装、最后卸载。
库名 `ark_net_plugin` 与 `DllEntryPlugin`／`DllExitPlugin` 签名保持不变，arknet 继续拥有
IO 工作线程与传输操作。

**头文件迁移：**移除 `<ark/net/net_service_manager_module_impl.hpp>` 与
`<ark/net/net_plugin.hpp>` 的包含。通过应用清单加载插件，再获取已注册的
`net_service_manager_module`，不直接构造管理器实现。改用 CMake 目标传递的包含目录，
不能仅使用 `-I <源码>/include`。没有新增 SDK 安装包。
`http_client_module_impl.hpp`、`net_client_service_impl.hpp`、`net_server_service_impl.hpp`、
端点类与回调模板继续公开，原有使用方无需更换这些类型。

## 服务端与客户端服务示例

进程需加载 Bus、Utility 和 Net，并提供自身的 Bus 配置。启动后显式创建监听；
加载插件不会自动监听。两种角色均通过公开管理器接口使用：

```cpp
#include <ark/net/net_service_manager_module.hpp>
#include <ark/plugin_manager.hpp>

bool start_listeners(ark::plugin_manager& manager) {
    return manager.find_module<ark::net_service_manager_module>()->create_server();
}

bool connect_tcp(ark::plugin_manager& manager, const ark::bus_addr& target,
    const std::string& host, uint16_t port) {
    return manager.find_module<ark::net_service_manager_module>()->create_client_service(
        target, ark::proto_type::tcp, host, port);
}
```

连接请求保持既有异步语义。继续在所属线程调用 `manager.update(timestamp)`，
并在回调停止前保持接收者有效。进程身份与端点配置见 [Bus](bus_CN.md)。

启用应用构建时，`check-core` 包含的 `check-net-plugin` 加载真实 Bus／Utility／Net 动态库，
验证两个接口键、管理器线程上的 TCP 消息与 HTTP 回调、正常重复停服及监听释放。
独立的 `tests/consumer/net` 工程仅链接 `ark::net`，分别编译各公开头，
并拒绝两个私有头路径。既有 `check-network` 继续覆盖端点、拆包、重连与停服。

## 应用层初始收包缓冲

TCP、WebSocket 与 RUDP 端点使用 4 KiB 初始应用层收包缓冲，随后按需增长。在 `start_client` 或 `start_server` 前，通过网络对象的 `set_initial_receive_buffer_size(bytes)` 配置；`get_initial_receive_buffer_size()` 读取配置值。传入零，或端点运行时修改，会返回 `false`；既有 Session 保留创建时的容量。

这是应用消息的拆包缓冲，与内核 socket 收发缓冲分别配置，也不是 4 KiB 的最大消息限制。[本地收包缓冲报告](../performance/receive-buffers_CN.md)保留了内存申请、分片输入与大批量吞吐量之间的取舍，不把这些结果视为网络 RTT 或连接容量。

## 成员回调与借用参数

端点构造函数与客户端／服务端服务的注册接口通过 C++20 `std::bind_front`，
将成员函数绑定到传入的同一个非拥有接收者指针。注册不会调用回调或延长接收者生命周期；
在回调不再可能执行前，接收者必须保持有效。公开回调签名与 Update 线程派发保持不变。

消息与事件指针在本次调用期间借用原对象。WebSocket 客户端的 `const conv_id_t&`
也引用本次调用的参数；需要保留时复制其值，不保留引用。服务端服务的转发回调仍使用
原有 `int` 参数及从 `conv_id_t` 的转换，本次改动不扩大该参数的取值范围。

启用 `ARK_BUILD_PROTO=ON` 的 `check-core` 验证公开服务注册助手，其头文件原本就依赖
Protobuf。`check-network` 验证真实端点派发，包括 WebSocket 的引用参数与所属 Update 线程。
这些检查不构成容量验收，也不改变所有权模型。
