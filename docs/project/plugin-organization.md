# Official plugins and repository organization

Public core and plugin headers live in `include/ark/`; core build definitions live in `src/core/` and official plugin implementations in `src/plugins/<name>/`. Core and official plugins belong in `ark`; business plugins belong in application repositories; `arknet` remains independent. Hot-update design and implementation are deferred while individual plugin integrations receive their own functional acceptance. The directory and snake_case migration is complete.

## Existing implementations

| Repository | Related code in ark | Proposed destination | Integration work |
| --- | --- | --- | --- |
| [MySQLPlugin](https://github.com/OpenArkStudio/MySQLPlugin) | `src/plugins/mysql` | `src/plugins/mysql` | [Native coroutine API](../reference/mysql.md), prepared statements, transactions and shared business execution; validate deployment against a real MySQL server |
| [RedisPlugin](https://github.com/OpenArkStudio/RedisPlugin) | `src/plugins/redis` | `src/plugins/redis` | Implement the Redis client in arknet, then replace aredis and bredis calls |
| [URLPlugin](https://github.com/OpenArkStudio/URLPlugin) | Net HTTP client and HTTP server plugin | `src/plugins/http` | Compare libcurl requests, encoding and actor integration; unify the HTTP service interface |
| [LuaScriptPlugin](https://github.com/OpenArkStudio/LuaScriptPlugin) | Integration with the new plugin interface required | `src/plugins/lua` | Define bindings and script lifecycle; replace old NFComm interface dependencies |
| [ActorPlugin](https://github.com/OpenArkStudio/ActorPlugin) | Integration with the new plugin interface required | `src/plugins/actor` | Define task scheduling, message delivery and business-thread responsibilities |

These repositories use different generations of interfaces and build systems. Compare features, overlapping implementations and dependencies during integration. MySQL and Redis clients belong in arknet; discuss other plugin dependencies separately.

## Directory layout

```text
ark/
├── include/ark/         # Public core and plugin interfaces
├── src/
│   ├── core/            # Header-only core target definitions
│   ├── plugins/         # Official plugin implementations and CMake files
│   │   ├── kernel/
│   │   ├── net/
│   │   ├── bus/
│   │   ├── http/
│   │   ├── mysql/
│   │   └── redis/
│   ├── app/
│   ├── proto/
│   └── server/
├── examples/            # Example applications and business plugins
├── tests/               # Core and cross-plugin integration tests
├── benchmarks/          # Standalone C++ benchmark programs
├── tools/               # Development and configuration tools
├── cmake/
└── docs/                # English/Chinese documentation and Nimbus site
```

This shows the main directories. Existing Consul, etcd, Kafka and other official plugins follow the same convention. Lua and Actor integration remains planned; the table above identifies their proposed implementation directories.

Each plugin follows the same package layout, for example:

```text
include/ark/mysql/      # Public interfaces for plugin consumers
src/plugins/mysql/
├── CMakeLists.txt
├── detail/             # Private native database bridge
└── *.cpp               # Plugin entry point and framework integration
tests/database/         # Database integration and coroutine tests
```

Usage documentation belongs in `docs/`, and example applications in `examples/`. Define public interfaces around capabilities; keep database client types within implementation code where possible.

## Core and plugin boundaries

| Component | Responsibility | Build and release |
| --- | --- | --- |
| Core runtime | Plugin/module registration, dependency ordering and lifecycle | Validate interface compatibility together with official plugins |
| Official plugins | Optional networking, storage, scripting and discovery services | Independent CMake targets, enabled and packaged as needed |
| Business plugins | Product rules and business message handlers | Built and released in application repositories |
| arknet | IO execution, network endpoints, transport protocols and database clients | Independently versioned; ark pins the dependency |

Resolve plugin-specific dependencies when the plugin is enabled. Core public headers should avoid including MySQL, Redis or Lua client headers. Distinguish build dependencies from runtime service requirements: Redis integration tests also need an available Redis service.

## Database client responsibilities

Implement MySQL and Redis clients within arknet rather than adopting Boost.MySQL or Boost.Redis. arknet owns database protocol encoding/decoding, authentication and asynchronous communication. The ark plugins integrate framework configuration, module services and business callbacks.

The arknet single-node scope includes MySQL queries, parameterized prepared statements and transactions; Redis generic commands, RESP2/RESP3 and pipelining; and authentication, TLS, pools, timeouts, cancellation and queue limits for both. Redis Cluster, Sentinel and Pub/Sub follow later. The native clients are available in [arknet 0.2.0](https://github.com/OpenArkStudio/arknet/releases/tag/v0.2.0), with independent-server correctness checks and local performance reports. This does not mean ark's MySQL and Redis plugins have switched to the new clients; plugin integration and application regression remain pending.

## Decisions to discuss next

1. Keep the unified snake_case SDK and directory layout; see [Project layout](../reference/directory-structure.md) for public headers and CMake targets.
2. Integrate MySQL and Redis plugins using Asio coroutines and native arknet owning results. arknet provides the caller-driven executor; ark pumps it on the module thread. Validate configuration, transactions, cancellation and complete shutdown before enabling runtime plugins.
3. Confirm interfaces and dependencies for Lua, Actor and other official plugins; add compilation, functional and failure-path verification per plugin.

Resume hot-update discussions about state ownership, in-flight calls and compatibility after the current modernization.
