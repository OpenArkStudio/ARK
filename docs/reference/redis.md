# Redis coroutine plugin

`redis_module` uses arknet's native Redis pool. This replaces the former bredis
synchronous and callback APIs; migrate `connect`, `set`, `get` and command-specific
wrappers to named configuration and `co_await command` / `pipeline`. There is no
blocking compatibility facade. Applications must rebuild against this interface.

```cpp
// During the consumer module's init(), after module registration:
auto* redis = find_module<ark::redis_module>();
auto& tasks = get_plugin_manager()->create_task_group(*this);
ark::redis_options options;
options.pool.connection.host = "127.0.0.1";
options.pool.connection.port = "6379";
options.pool.max_connections = 8;
options.max_operations = 64;
options.max_input_bytes = 1024 * 1024;
redis->configure("cache", std::move(options));

// A member coroutine, started through tasks.start(workflow()):
ark::task<> workflow() {
    auto result = co_await redis_->command("cache", {"GET", "key"});
    if (!result) {
        // Inspect result.error, transport_error and command_may_have_run.
        co_return;
    }
    // result.replies owns its strings, arrays and binary data.
}
```

Configuration owns its strings, credentials, pool, protocol, response limits and
TLS settings. `configure` returns false for a duplicate name or sealed admission;
empty names and invalid limits throw. It may run before the Redis module's own
`init`: the installed manager already supplies its task runtime. It does not use
the obsolete `redis.plugin.conf` file. Configuration is owner-thread only and is
not dynamically replaced. Set `options.tls = true` and configure certificate
verification through `options.pool` when building with `ARKNET_ENABLE_SSL`;
requesting TLS in a build without SSL throws.

Command factories take owning values and preserve inputs before the lazy task
starts. The returned task also pins its connection state, including when the
facade is destroyed before an unstarted task is used. Run operations on the exact
manager task executor. A foreign executor throws before native submission.
Unknown connection names return native `not_connected` (`closed` after sealing).
Local bridge admission failures throw; invalid deadlines return native
`invalid_command`. Native transport/database failures remain `arknet::redis::result`.
Redis error replies remain values in the ordered `replies` vector.

`pipeline(name, commands, timeout)` returns one native result containing ordered
replies. Its timeout covers pool acquisition and execution. `max_operations`
bounds bridge records and service operations retained until owner delivery;
`max_input_bytes` bounds service-retained command storage, including allocated
capacity. Input rejection reports native `queue_full`. Configure native pool,
waiter and response bounds separately in `options.pool`. These are per-connection
limits, not a whole-process memory limit.

Cancellation requests flow from the manager's managed task group into the native
service. A cancelled waiting acquire is known unsent; cancellation after sending
can have `command_may_have_run == true`. No write replay or synthetic success is
introduced. The native result, including partial replies, stays owned by its
caller after pool retirement.

The Redis plugin runs `service.update()` and then bridge dispatch during manager
updates. The manager polls its shared task runtime centrally. arknet owns native
IO and worker threads. Application code must keep updating the manager while
stopping it: `stop()` can return false while accepted workflows finish, including
later commands in the same workflow. Readiness only observes logical activity;
it does not seal the service early. After all managed tasks and delivery stacks
have returned, `pre_shut` seals admission and calls native `retire_native()` from
the host lifecycle frame. A false return or exception preserves resources and
prevents `shut` from releasing them. `service.stop()` is not the retirement
barrier. Calling lifecycle functions recursively from a running task is not a
supported teardown path; manager shutdown defers that work to its host frame.

`tests/database/public_redis_module.cpp` covers the public facade against a
bounded loopback RESP protocol peer: owning input/results, ordered pipeline
replies, foreign executor rejection, quotas, cancellation, verified TLS and graceful
manager stop. This peer is not a Redis implementation. Private TLS regressions
cover additional underlying-service failure cases. These local checks do not establish real Redis
deployment acceptance, production capacity or a general leak-free guarantee.

`ark_database_public_redis_plugin_tests` additionally builds and loads the actual
plugin library, exercising macro registration, coroutine delivery and host
shutdown. This does not certify OS library unloading, hot update or real Redis
server acceptance.
