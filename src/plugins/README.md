# Official plugins

[简体中文](README_CN.md)

Core and other plugin public headers remain under `include/ark/`. The MySQL/Redis pilot co-locates public interface headers, implementation and private assembly under `src/plugins/<name>/`; CMake maps only explicitly listed interface headers, preserving `<ark/mysql/mysql_module.hpp>` and `<ark/redis/redis_module.hpp>`. Runtime plugins retain independent optional targets. Business plugins belong in application repositories.

| Plugin | Responsibility | Default build |
| --- | --- | --- |
| kernel | Entity data, configuration and event services | On |
| net | TCP, RUDP, WebSocket and HTTP integration with arknet | On |
| bus | Process relationships and message routing | On |
| utility | IDs, timers, scheduling and compression | On |
| etcd_client | etcd service access | On |
| http | HTTP server module | Off |
| consul | Consul service discovery | Off |
| kafka | Kafka messaging | Off |
| physic | Physics services | Off |
| mysql, redis | Native arknet database pools and public coroutine APIs | Off |

Use `ARK_BUILD_PLUGIN_<NAME>` to select a plugin. Optional integrations require their own dependencies and verification. [MySQL](../../docs/reference/mysql.md) provides queries, prepared statements and transactions; [Redis](../../docs/reference/redis.md) provides commands and ordered pipelines. Both use named configuration and manager-owned coroutine tasks. See [database plugin activation](../../docs/reference/build-options.md#native-database-plugins) for build and runtime selection. Lua and Actor integrations remain planned work.

See [project layout and SDK](../../docs/project-layout.md) for targets and build commands.
