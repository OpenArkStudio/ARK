# Official plugins

[简体中文](README_CN.md)

All official plugins co-locate their headers, implementation and assembly under `src/plugins/<name>/`. CMake uses explicit public-header mappings to preserve logical `<ark/<name>/...>` paths, even with runtime plugins disabled. Kernel, Consul, Etcd Client, Kafka, OSS SDK and Physic retain every existing public header, including implementation and assembly headers; the earlier MySQL/Redis/Utility/Net/Bus/HTTP mappings keep private assembly and factories excluded. Runtime plugins retain independent optional targets. Business plugins belong in application repositories.

| Plugin | Responsibility | Default build |
| --- | --- | --- |
| kernel | Entity data, configuration and event services | On |
| net | TCP, RUDP, WebSocket and HTTP integration with arknet | On |
| bus | Process relationships and message routing | On |
| utility | IDs, timers, scheduling and compression | On |
| etcd_client | etcd service access | On |
| [http](../../docs/reference/http.md) | Routed HTTP server module | Off |
| consul | Consul service discovery | Off |
| kafka | Kafka messaging | Off |
| osssdk | OSS SDK scaffold | Off |
| physic | Physics services | Off |
| mysql, redis | Native arknet database pools and public coroutine APIs | Off |

Use `ARK_BUILD_PLUGIN_<NAME>` to select a plugin. Optional integrations require their own dependencies and verification. [MySQL](../../docs/reference/mysql.md) provides queries, prepared statements and transactions; [Redis](../../docs/reference/redis.md) provides commands and ordered pipelines. Both use named configuration and manager-owned coroutine tasks. See [database plugin activation](../../docs/reference/build-options.md#native-database-plugins) for build and runtime selection. Lua and Actor integrations remain planned work.

See [project layout and public plugin interfaces](../../docs/project-layout.md) for targets and build commands.

[Utility](../../docs/reference/utility.md) documents the four module interfaces, owner-driven timers and the implementation-header migration.

Kernel co-locates its 38 existing headers with its cpp files in `src/plugins/kernel/`. `ark::kernel` preserves every `<ark/kernel/...>` path, including `data_impl.hpp`, `data_list_impl.hpp` and `kernel_plugin.hpp`. This source-only move keeps creation macros, registration, type visibility and lifecycle unchanged. See [Kernel reference](../../docs/reference/kernel.md) for consumption checks and Protobuf requirements.

The five remaining plugin families retain 22 byte-identical headers. Mapping checks do not validate Consul, Kafka or OSS SDK runtime integrations. Physic retains the unsupported PhysX 3 configuration failure; see [optional legacy dependencies](../../docs/dependencies.md#optional-legacy-integrations).
