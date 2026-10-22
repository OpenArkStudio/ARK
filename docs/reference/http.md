# HTTP server plugin

The optional HTTP plugin exposes `http_server_module` and arknet's public
`http_router` type. It starts an HTTP/1 listener explicitly and routes requests on
arknet IO workers. The Net plugin's HTTP client remains a separate interface.

## Build and configure

Enable `ARK_BUILD_APPLICATIONS=ON` and `ARK_BUILD_PLUGIN_HTTP=ON`; applications use
Boost.Asio and Boost.Beast. Deploy `ark_http_plugin` with the usual platform suffix
and Debug `_d`. Select the plugin and its configuration in the application manifest:

```toml
[[plugins]]
name = "ark_http_plugin"
config = "http.plugin.toml"
```

The configuration path resolves against the selected plugin configuration directory.
The shipped `config/http.plugin.toml` contains:

```toml
[http]
host = "0.0.0.0"
port = 9090
threads = 4
```

All three fields are required. `host` must be a nonempty string without NUL;
`port` is a TOML integer in `0..65535`, and `threads` an integer in `1..255`.
Unknown keys, missing/wrong types and out-of-range values fail `init()` with the
file and field in the diagnostic; syntax errors include line and column.
Renaming an old XML file does not convert its contents. See
[plugin configuration](../service-manifests.md#http-and-consul-plugin-settings).

`start_server()` uses those configured values. The explicit overload retains its
public defaults `port = 8080` and `thread_num = 1`; these are not TOML defaults.
A zero port asks the OS for an available port. The public module does not expose
the selected port, so choose a known configured port when clients must discover it.
Initialization parses configuration without starting a listener.

## Route requests

After startup, obtain the required registered interface and register routes before
starting the listener:

```cpp
#include <ark/http/http_server_module.hpp>
#include <ark/plugin_manager.hpp>

bool listen(ark::plugin_manager& manager) {
    auto* server = manager.find_module<ark::http_server_module>();
    server->get_router().add(arknet::http::verb::get, "/users/:id",
        [](const auto& context, auto& response) {
            response.set(arknet::http::field::content_type, "text/plain");
            response.body() = "user:" + std::string(context.param("id"));
        });
    return server->start_server();
}
```

The router reference is borrowed from the module; `get_router()` returns the same
owned router each time. Route context/response arguments belong to that invocation.
Keep handler state alive and synchronize shared business state accessed by IO
workers. Routing does not wait for `manager.update()` or move handlers to the
manager's thread. `make_200<response_body_type>()` retains the public response
helper, including content type, length and keep-alive behavior.

An already running module rejects another start. Two module instances cannot
reserve the same nonzero endpoint within the library. `pre_shut()` stops and
releases the server and reservation; the endpoint can then be started again.
The owner thread drives normal manager shutdown. This is existing HTTP listener
shutdown, not generic plugin hot replacement or HTTP/2 support. arknet continues
to own IO workers, request parsing and transport operations.

## Public interfaces and header mapping

The public header lives in `src/plugins/http/http_server_module.hpp`. An explicit
CMake mapping through `ark::http` retains `<ark/http/http_server_module.hpp>`,
including when runtime plugins and Protobuf are disabled. The concrete class
lives in cpp, inherits `module_base<http_server_module>` and is constructed by a
private interface-returning factory. Private plugin assembly uses `plugin_base`
and explicit `DllEntryPlugin` / `DllExitPlugin` functions. The public virtual and
router types, RTTI key, library stem and entry signatures are unchanged. The
module does not register an update callback, matching its inherited default.

**Header migration:** replace `<ark/http/http_server_module_impl.hpp>` and
`<ark/http/http_plugin.hpp>` includes with the public module header and obtain the
loaded interface from `plugin_manager`. Use the `ark::http` target's include roots
instead of only `-I <source>/include`. Private assembly/parser headers are not
mapped. The generated mapping is an include view of the co-located source header.

## Validation

With the HTTP target enabled, `check-http-plugin` (also in `check-core`) loads the
actual HTTP DSO and sends real loopback HTTP requests. It checks the interface
key, router identity, path/query routing, binary POST bodies, response status and
headers, IO-worker dispatch without manager updates, retirement of an active
handler during owner-thread stop, repeated stop and endpoint reuse. The in-flight
shutdown case permits the existing connection-close outcome; it does not require
a response after shutdown begins. No substitute server or transport is used.

The existing 37-case `check-network` suite retains its HTTP duplicate binding,
restart, public response helper and invalid-TOML assertions through the private
factory. `tests/consumer/http` links only `ark::http`, compiles the public header
separately and rejects private header paths with apps/Protobuf disabled. See
[directory structure](directory-structure.md) for the public-header consumer command, and
[arknet HTTP](https://openarkstudio.github.io/arknet/features/) for routing APIs.
