# Bus plugin

Bus gives processes logical addresses and loads application types, process endpoints and connection relationships. `bus_module` exposes configuration queries. `msg_module` sends messages by application type, Bus ID or session, while the Net plugin manages the underlying connections.

## Addresses and responsibilities

A Bus address has four components, `channel.zone.app.instance`, such as `1.1.31.1`. Each occupies 8 bits in the combined `bus_id_t`. It identifies a process, rather than an IP address or listening port.

| Component | Meaning | Example |
| --- | --- | --- |
| `channel` | Application-defined channel | `1` |
| `zone` | Application-defined region | `1` |
| `app` | Numeric `app_type` value | `31` is `ark_app_game` |
| `instance` | Instance number within that process type | `1` |

| Interface | Responsibility |
| --- | --- |
| `bus_module` | Local process information, application names, target relationships and other process configurations |
| `msg_module` | Serialize messages, select an existing connection, fill headers and request sending |
| `net_service_manager_module` | Listening services, client connections and Bus-to-session mappings |
| `net_client_service` / `net_server_service` | Receive callbacks, connection events and connection state handling |

## Configure processes and relationships

`bus_module::init()` reads four sections from `bus.plugin.toml`:

| Section | Contents |
| --- | --- |
| `bus.applications` | Application names mapped to numeric `app_type` values |
| `bus.relations` | Directed targets: which `target` types a `source` should connect to |
| `bus.processes` | Listening endpoints, connection limits and thread settings for each Bus ID |
| `bus.registry` | Registry parameters exposed by `get_reg_center()` |

The following example describes Game and World processes. External and intranet endpoints use different ports to avoid conflicts when both listeners start.

```toml
[bus]
[[bus.applications]]
name = "world"
type = 30
[[bus.applications]]
name = "game"
type = 31
[[bus.relations]]
source = "game"
target = "world"
[[bus.processes]]
bus_id = "1.1.30.1"
server_endpoint = "tcp://127.0.0.1:9101"
intranet_endpoint = "tcp://127.0.0.1:9111"
max_connections = 100
threads = 1
[[bus.processes]]
bus_id = "1.1.31.1"
server_endpoint = "tcp://127.0.0.1:9102"
intranet_endpoint = "tcp://127.0.0.1:9112"
max_connections = 100
threads = 1
[bus.registry]
host = "127.0.0.1"
port = 8500
service_name = "ark-service"
check_interval = "10s"
check_timeout = "3s"
```

This is a Bus configuration example, not a complete two-process launch configuration. Applications must also load Bus, Net and their dependencies, set each process's Bus ID, start listeners and initiate connections. `relations` supplies target types; reading a relationship does not automatically create a connection.

Configuring `bus.registry` does not enable dynamic registration or discovery. The Consul registration and health-based discovery paths in the current network service manager remain commented out. This configuration alone cannot discover target processes.

## Migrate XML runtime settings

Bus runtime settings now require TOML content; renaming the old XML file is insufficient. Replace it with `bus.plugin.toml` and update each application's `ark_bus_plugin` configuration reference. The source asset is `config/bus.plugin.toml`; deployment still places it under the runtime `conf/` prefix. XML application manifests remain supported, and business data XML is unchanged.

| XML location/attribute | TOML location/key |
| --- | --- |
| `applications/application` | `[[bus.applications]]`; `name`, `type` unchanged |
| `relations/relation` | `[[bus.relations]]`; `source`, `target` unchanged |
| `processes/proc` | `[[bus.processes]]` |
| `busid` | `bus_id` |
| `endpoint_server`, `endpoint_intranet` | `server_endpoint`, `intranet_endpoint` |
| `max_connection`, `thread_num` | `max_connections`, `threads` |
| `center` / `ip` | `[bus.registry]` / `host`; other keys unchanged |

All four sections and their fields are required. Arrays may be empty (`applications = []`, `relations = []`, `processes = []` under `[bus]`). Unknown keys, wrong TOML types and invalid values fail initialization with the file, section or array index, and field in the diagnostic. Integer settings use TOML integers: application types are `1..254`, connection limits `0..4294967295`, threads `0..255`, and registry ports `0..65535`. Zero retains its existing configured meaning; the parser does not start services or choose a concurrency policy.

Bus IDs require exactly four decimal components in `0..255`. Nonempty endpoints require a supported protocol and `protocol://host:port` (optional path), with port `0..65535`. Hostnames are retained without DNS lookup. IPv6 literals use brackets, for example `tcp://[::1]:9011`; their numeric syntax is validated locally, and the stored host is `::1` without brackets for arknet. The public endpoint formatter is unchanged. An empty endpoint remains allowed and disables that listener. Both endpoints are validated independently. Registry interval and timeout strings are passed through without interpreting their duration grammar.

The public `app_config`, `proc_config` and `reg_center` fields and query methods remain unchanged. Application maps and other-process maps keep their first entry for a duplicate key; the last matching self-process entry wins, and relations retain input order and duplicates. A missing self-process entry remains unconfigured. These settings do not enable discovery or create network connections. The private parser uses the existing toml++ dependency only when configuration or application builds are enabled.

## Query local and target processes

After module initialization, obtain the Bus interface from the plugin manager. This function collects statically configured processes for the current application's target types.

```cpp
#include <vector>

#include "ark/bus/bus_module.hpp"
#include "ark/plugin_manager.hpp"

std::vector<ark::proc_config> configured_targets(ark::plugin_manager& manager) {
    auto* bus = manager.find_module<ark::bus_module>();
    if (bus == nullptr)
    {
        return {};
    }

    std::vector<ark::app_type> types;
    if (!bus->get_target_bus_relations(types))
    {
        return {};
    }

    std::vector<ark::proc_config> result;
    for (auto type : types)
    {
        std::vector<const ark::proc_config*> processes;
        bus->get_other_proc(type, processes);
        for (const auto* process : processes)
        {
            result.push_back(*process);
        }
    }
    return result;
}
```

`get_other_proc(type, list)` appends pointers to its output list and returns the resulting total list length. The example starts with an empty list each time and copies the records, avoiding externally retained pointers into module state. This query filters by application type without also restricting channel or zone; applications can apply those filters when needed.

| Query | Result and limits |
| --- | --- |
| `get_self_bus_id()` / `get_self_bus_name()` | Current process integer ID / four-component address |
| `get_self_proc()` | Local `proc_config`; static configuration does not mean a listener is running |
| `get_app_type(name)` | Returns `ark_app_default` when absent |
| `get_app_name(type)` | Returns an empty string when absent |
| `get_other_proc(id, proc)` | Copies the configuration and returns `true` when found; otherwise returns `false` |
| `combine_bus_id(type, instance)` | Combines the local channel and zone with the supplied type and instance |

## Establish a TCP connection

The target process first starts its listeners with `net_service_manager_module::create_server()`. The following function reads the target's configured intranet endpoint and queues a TCP connection through the network service manager.

```cpp
#include "ark/bus/bus_module.hpp"
#include "ark/net/net_service_manager_module.hpp"
#include "ark/plugin_manager.hpp"

bool queue_tcp_connection(ark::plugin_manager& manager, ark::bus_id_t target_id) {
    auto* bus = manager.find_module<ark::bus_module>();
    auto* network = manager.find_module<ark::net_service_manager_module>();
    if (bus == nullptr || network == nullptr)
    {
        return false;
    }

    ark::proc_config target;
    if (!bus->get_other_proc(target_id, target))
    {
        return false;
    }

    const auto& endpoint = target.intranet_ep;
    if (endpoint.proto() != ark::proto_type::tcp || endpoint.get_ip().empty() || endpoint.get_port() == 0)
    {
        return false;
    }

    return network->create_client_service(
        ark::bus_addr(target_id), ark::proto_type::tcp, endpoint.get_ip(), endpoint.get_port());
}
```

`true` means the connection request was queued in the client service and will be processed by subsequent `update()` calls. It does not mean the connection has succeeded. Observe connection events with the client service's `reg_net_event_callback()` and register message handlers with `reg_msg_callback()`. The receiver registers handlers on the corresponding server service.

The current client service handles reconnection through its connection state machine, with a 30-second retry interval in the implementation. This is not a configurable backoff policy in the XML file. Reconnection does not replay application messages whose earlier send failed.

## Send and receive messages

Once a connection is available, send application-defined Protobuf messages through `msg_module`. The following function accepts the caller's message ID and message type, without defining an application protocol.

```cpp
#include "ark/bus/msg_module.hpp"

bool send_to_process(ark::msg_module& messages, ark::bus_id_t target_id, ark::msg_id_t message_id,
    const google::protobuf::Message& body) {
    return messages.send_msg_by_bus_id(target_id, message_id, body);
}
```

| Send method | Connection selection |
| --- | --- |
| `send_msg_by_bus_id()` | Uses an existing connection for the source and target Bus IDs, with an inter-process message header |
| `send_def_ss_msg_by_app_type()` | Uses consistent hashing within the target type's client service; the current key is the local process Bus ID |
| `send_def_cs_msg_by_app_type()` | Uses the same selection method with a client message header |
| `send_to_client()` | Sends to a session through the external listening service |
| `send_to_intra()` | Sends to a session through the intranet listening service |

Inter-process headers carry the message ID, source / target Bus IDs and Actor ID. Sending returns `false` on failure, for example when a service or connection is unavailable or serialization fails. `true` does not mean the remote application has processed the message. Request/response handling, timeouts and idempotency belong to the application protocol.

The receive flow is: the network service extracts a `net_msg` → invokes the handler registered for its message ID → the handler parses its application message with `msg_module::decode_protobuf()`. The parser returns `false` on a Protobuf parse failure. `read_message_body()` copies the raw body and retrieves the Actor ID. A callback's `net_msg*` is borrowed data; copy the required contents before deferring work.

## Coverage and planned work

These examples cover configuration queries, an explicit TCP connection request and sending through an existing connection. They are not an accepted multi-process deployment example, and do not imply that every protocol is integrated with the service manager. See the [Net plugin](network.md) for transport integration.

TODO: provide a standalone two-process example and tests covering connection setup, message round trips, process restarts, reconnection and cross-host routing. Implement and validate dynamic registration / discovery separately.

## API sources

- [Bus addresses and process configuration](https://github.com/OpenArkStudio/ark/blob/main/include/ark/bus.hpp)
- [Bus interface](https://github.com/OpenArkStudio/ark/blob/main/include/ark/bus/bus_module.hpp)
- [Message interface](https://github.com/OpenArkStudio/ark/blob/main/include/ark/bus/msg_module.hpp)
- [Network service management interface](https://github.com/OpenArkStudio/ark/blob/main/include/ark/net/net_service_manager_module.hpp)
- [Network build and tests](../networking.md)
