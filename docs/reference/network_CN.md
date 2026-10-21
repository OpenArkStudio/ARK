# Net 插件

网络插件将独立的 arknet 网络库接入 ark，处理业务拆包、Session ID 与 Update 线程上的业务回调。

| 传输 | 客户端 | 服务端 |
| --- | --- | --- |
| TCP | `tcp_client` | `tcp_server` |
| WebSocket | `websocket_client` | `websocket_server` |
| RUDP | `rudp_client` | `rudp_server` |
| HTTP | `http_client_module_impl` | `http_server` |

`net` 定义启动、关闭、发送和 Update 操作。业务消息继续使用现有消息头；RUDP 使用 arknet 兼容的 ARQ 报文格式，默认关闭 FEC。

构建命令和验证范围见[网络构建与测试](../networking_CN.md)。接口见 [ark::net](https://github.com/OpenArkStudio/ark/blob/main/include/ark/net/net.hpp)，协议能力见 [arknet 指南](https://openarkstudio.github.io/arknet/zh/features/)。

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
