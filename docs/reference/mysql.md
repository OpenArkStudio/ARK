# MySQL coroutine plugin

`mysql_module` provides named native arknet pools, queries, parameterized prepared statements and transactions. It replaces mysql++, the driver-manager modules and the former synchronous SQL-builder interface. Rebuild plugins and migrate consumers to managed coroutine tasks.

## Configure and query

```cpp
// Consumer module init(), after plugin registration:
auto* database = find_module<ark::mysql_module>();
ark::mysql_options options;
options.pool.connection.host = "db.example.com";
options.pool.connection.username = "app";
options.pool.connection.database = "app";
options.pool.connection.password = load_secret();
options.pool.connection.ca_file = "/etc/app/mysql-ca.pem";
options.pool.max_connections = 8;
options.max_operations = 64;
options.max_input_bytes = 1024 * 1024;
if (!database->configure("primary", std::move(options))) {
    throw std::runtime_error("Duplicate or unavailable MySQL configuration");
}

// A member coroutine started by manager.create_task_group(*this).start(run()):
ark::task<> run() {
    auto reply = co_await database_->query("primary", "SELECT id FROM players LIMIT 10");
    if (!reply) {
        // Inspect reply.result.status.code, message, transport_error and outcome_unknown.
        co_return;
    }
    // Native rows and columns own their storage after the await completes.
    std::vector<arknet::mysql::field_value> parameters{std::int64_t{42}};
    auto prepared = co_await database_->execute("primary", "SELECT ?", std::move(parameters));
    // Check the execution result and statement_close / statement_close_exception separately.
}
```

Configuration is explicit and owner-thread only. It can precede the database module's `init()` because registration already installs its plugin manager. Duplicates and configuration during shutdown return false; empty names or zero limits throw. Connections cannot be reconfigured in place. TLS certificate verification is enabled by default; configure the CA and server name as needed. The optional MySQL target requires `ARKNET_ENABLE_SSL=ON` and OpenSSL. No mysql++ driver is linked and no legacy MySQL XML configuration is read.

## Transactions

```cpp
ark::task<> update_player() {
    auto begun = co_await database_->begin("primary", {std::chrono::seconds(5), std::chrono::seconds(1)});
    if (!begun) { co_return; }
    const auto id = begun.transaction;
    std::vector<arknet::mysql::field_value> values{std::int64_t{42}};
    auto changed = co_await database_->execute(id, "UPDATE players SET score=?", std::move(values));
    if (!changed) {
        auto rolled_back = co_await database_->rollback(id);
        // Inspect rolled_back.transaction_status, including cleanup failures.
        co_return;
    }
    auto committed = co_await database_->commit(id);
    // A transport failure can make the commit outcome unknown. Never blindly replay it.
}
```

Transactions retain one native lease across all steps. Their monotonic identifiers are valid only for the issuing module instance; they own no plugin implementation object. Use one operation at a time per transaction. Unknown/expired identifiers and overlapping steps throw. The original timeout covers acquire, BEGIN, every step and commit; it is never renewed by another step. An unused transaction rolls back at its deadline. `cleanup_timeout` bounds the separate rollback attempt. A failed or uncertain cleanup discards the connection. Automatic retry or cross-connection replay is not provided.

`mysql_reply` retains the owning `arknet::mysql::query_result` and separate statement-close diagnostics. `mysql_transaction_reply` additionally retains rollback results, initiation failures and the final transaction status. Successful execution alone does not prove cleanup or commit succeeded. Native database/transport failures are returned; local C++ and bridge admission failures propagate as exceptions. Cancellation requests reach the native request and preserve `outcome_unknown` for a potentially executed statement.

## Execution, limits and shutdown

Run tasks on the manager's shared business executor. A foreign executor is rejected before native submission. The plugin publishes service completions and bridge deliveries in `update()`; the manager polls its single manual runtime. arknet owns socket IO and worker threads. Inputs are owned before lazy tasks start; returned results remain usable after native retirement.

`max_operations` bounds coroutine delivery records, `max_input_bytes` bounds retained request capacity, and `max_transactions` bounds active transactions per named pool. Configure native pool connections/waiters, query row/column/result-byte limits and connection queue bounds separately. These are per-pool limits, not a process-wide memory budget.

Keep calling manager updates while `stop()` returns false. Accepted workflows can finish subsequent database steps; idle transactions complete deadline cleanup. Readiness waits for both query and transaction coordinators and their delivery stacks. Only then does the host teardown phase seal admission and retire the owning pool through its native join barrier. A retirement failure retains resources and fails shutdown. This is not a hot-update or arbitrary external coroutine lifetime guarantee.

## Validation

`tests/database/public_mysql_module.cpp` runs public query, prepare/execute/close, TLS, transaction commit/rollback, idle deadline cleanup, cancellation and executor checks against a bounded local MySQL protocol peer. Build with `ARK_BUILD_DATABASE_TESTS=ON` and run `ark_database_public_mysql_tests`, or `check-database` for the development regression suite. Existing `ark_database_mysql_tests` and `ark_database_mysql_transaction_tests` require a real MySQL endpoint and explicit environment configuration; they are not replaced by the local peer. Local protocol regressions do not establish production capacity or real-server deployment acceptance.

`ark_database_public_mysql_plugin_tests` additionally builds and loads the real plugin library to verify macro registration, coroutine delivery and host shutdown. It does not certify OS library unloading or hot update.
