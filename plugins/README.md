# Official plugins

[简体中文](README_CN.md)

Each official plugin has a separate directory, public headers in `include/ark/<name>/`, implementation files in `src/` and an optional CMake target. Business plugins belong in application repositories.

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
| mysql, redis | Legacy database integrations pending arknet clients | Off |

Use `ARK_BUILD_PLUGIN_<NAME>` to select a plugin. Optional integrations require their own dependencies and verification. MySQL and Redis clients will be implemented in arknet before replacing the legacy plugin calls. Lua and Actor integrations remain planned work.

See [project layout and SDK](../docs/project-layout.md) for targets and build commands.
