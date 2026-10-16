# Network plugin

The network plugin integrates the independent arknet library with ark's message framing, session IDs and Update-thread business callbacks.

| Transport | Client | Server |
| --- | --- | --- |
| TCP | `tcp_client` | `tcp_server` |
| WebSocket | `websocket_client` | `websocket_server` |
| RUDP | `rudp_client` | `rudp_server` |
| HTTP | `http_client_module_impl` | `http_server` |

`net` defines start, shutdown, send and Update operations. Application messages retain their existing headers. RUDP uses arknet's compatible ARQ wire format with FEC disabled by default.

See [network builds and tests](../networking.md), [ark::net](https://github.com/OpenArkStudio/ark/blob/main/include/ark/net/net.hpp) and the [arknet protocol guides](https://openarkstudio.github.io/arknet/features/).

## Initial application receive buffer

TCP, WebSocket and RUDP endpoints use a 4 KiB initial application receive buffer and grow as needed. Call `set_initial_receive_buffer_size(bytes)` on the network object before `start_client` or `start_server`; `get_initial_receive_buffer_size()` reads the configured value. Zero or a change while the endpoint is working returns `false`; existing sessions retain their selected capacity.

This is the framing buffer for application messages, configured separately from kernel socket send/receive buffers. It is not a 4 KiB maximum message size. The [local receive-buffer report](../performance/receive-buffers.md) retains the allocation, fragmented-input and large-batch throughput tradeoff. It does not establish network RTT or connection capacity.
