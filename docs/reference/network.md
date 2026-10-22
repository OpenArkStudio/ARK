# Network plugin

The network plugin integrates the independent arknet library with ark's message framing, session IDs and Update-thread business callbacks.

| Transport | Client | Server |
| --- | --- | --- |
| TCP | `tcp_client` | `tcp_server` |
| WebSocket | `websocket_client` | `websocket_server` |
| RUDP | `rudp_client` | `rudp_server` |
| HTTP | `http_client_module` | `http_server` |

`net` defines start, shutdown, send and Update operations. Application messages retain their existing headers. RUDP uses arknet's compatible ARQ wire format with FEC disabled by default.

See [network builds and tests](../networking.md), [ark::net](https://github.com/OpenArkStudio/ark/blob/main/src/plugins/net/net.hpp) and the [arknet protocol guides](https://openarkstudio.github.io/arknet/features/).

The optional routed [HTTP server plugin](http.md) exposes `http_server_module`; its route handlers run on arknet IO workers.

## API and private construction

All Net source headers and implementations live in `src/plugins/net/`. The
`ark::net` API target maps 18 explicit public headers to `<ark/net/...>` and
propagates their public dependencies. Enable `ARK_BUILD_PROTO=ON` for the service
headers that include protocol messages; `ark::net` then carries `ark::proto`.
Direct endpoint headers remain usable with protocol generation disabled.
API targets do not build or load the runtime DSO.

The private plugin uses `plugin_base`, and a factory returning
`std::unique_ptr<net_service_manager_module>` constructs a cpp-local
`module_base<net_service_manager_module>`. Lookup retains the public interface's
RTTI key. A second private factory returns `std::unique_ptr<http_client_module>`;
its `module_base<http_client_module>` implementation is declared only in
`http_client_module_impl.cpp`. HTTP client registration also uses its public interface key. Both
modules still receive updates; HTTP installs first and uninstalls last. The
library stem `ark_net_plugin` and `DllEntryPlugin`/`DllExitPlugin` signatures stay
unchanged. arknet continues to own IO workers and transport operations.

**Header migration:** remove includes of
`<ark/net/net_service_manager_module_impl.hpp>`, `<ark/net/net_plugin.hpp>` and
`<ark/net/http_client_module_impl.hpp>`.
Load the plugin through the application manifest and obtain the registered
`net_service_manager_module` instead of constructing the manager implementation.
Use CMake's target-provided include roots instead of only `-I <source>/include`.
**Breaking source/ABI change:** `http_client_module_impl` and its old public
header are removed from the source/build/install interfaces. Include
`<ark/net/http_client_module.hpp>` and resolve `http_client_module` from the plugin
manager instead of constructing the concrete client. The factory is internal
to Net and is not an application API. Rebuild Net and consumers together, using
a fresh install prefix so obsolete headers from older installations do not linger.
`net_client_service_impl.hpp`, `net_server_service_impl.hpp`, endpoint classes
and callback templates remain public. Request overloads, timeout defaults,
future results, owner-update callback dispatch and shutdown behavior are unchanged.

## Server and client service examples

The process must load Bus, Utility and Net and provide its Bus configuration.
Start listeners explicitly after startup; loading the plugin does not create
them. These functions use the public manager interface for both roles:

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

Connection requests retain their existing asynchronous behavior. Continue calling
`manager.update(timestamp)` on the owner thread; keep callback receivers alive
until delivery stops. See [Bus](bus.md) for identity and endpoint configuration.

`check-net-plugin`, included in application-enabled `check-core`, loads actual
Bus/Utility/Net DSOs, checks both interface keys, TCP message and HTTP callbacks
on the manager thread, futures completing without manager updates, pending-request
cancellation through `pre_shut()`, normal repeated shutdown and listener release.
DSO-created futures are consumed while the implementation library is still loaded;
this does not establish their validity across arbitrary library unloading. The
independent `tests/consumer/net` project links only `ark::net`, compiles each
public header separately and rejects the removed implementation and private factory/assembly paths. Existing
`check-network` endpoint, framing, reconnect and shutdown cases remain applicable.

## Initial application receive buffer

TCP, WebSocket and RUDP endpoints use a 4 KiB initial application receive buffer and grow as needed. Call `set_initial_receive_buffer_size(bytes)` on the network object before `start_client` or `start_server`; `get_initial_receive_buffer_size()` reads the configured value. Zero or a change while the endpoint is working returns `false`; existing sessions retain their selected capacity.

This is the framing buffer for application messages, configured separately from kernel socket send/receive buffers. It is not a 4 KiB maximum message size. The [local receive-buffer report](../performance/receive-buffers.md) retains the allocation, fragmented-input and large-batch throughput tradeoff. It does not establish network RTT or connection capacity.

## Member callbacks and borrowed arguments

Endpoint constructors and client/server service registration bind the supplied
member function to the same non-owning receiver pointer with C++20
`std::bind_front`. Registration does not invoke the callback or extend the
receiver's lifetime; keep that object alive until its callbacks can no longer run.
Public callback signatures and Update-thread dispatch remain unchanged.

Message and event pointers borrow the original objects for that invocation.
The WebSocket client's `const conv_id_t&` parameter also refers to an invocation
argument; copy its value instead of retaining the reference. The server service's
forward callback retains its existing `int` parameter and conversion from
`conv_id_t`; this change does not expand that parameter's range.

`check-core` with `ARK_BUILD_PROTO=ON` exercises the public service-registration
helpers (their headers already depend on Protobuf). `check-network` exercises
actual endpoint delivery, including the WebSocket reference parameter and owner
Update thread. These checks do not establish capacity or a new ownership model.
