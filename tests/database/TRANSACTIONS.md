# Private MySQL transaction scopes

[简体中文](TRANSACTIONS_CN.md)

This development module is a fixed-lease transaction building block. It does not
activate the published MySQL plugin, replace MySQL++, change normal plugin
shutdown, or add a public database API. The existing `mysql_service` is unchanged.

## Build and run

Use the [database development configuration](README.md) with SSL enabled and the
existing local arknet checkout. The dependency revision remains unchanged.

```sh
cmake --build database-build --target ark_database_mysql_transaction_state_tests \
  ark_database_mysql_transaction_tests
./database-build/tests/database/ark_database_mysql_transaction_state_tests
./database-build/tests/database/ark_database_mysql_transaction_tests
```

The state executable needs no database and is included in `check-database` when
`ARKNET_ENABLE_SSL=ON`. The real-service executable requires the same
`ARK_DATABASE_MYSQL_HOST`, `PORT`, `USER`, `PASSWORD`, `DATABASE` and `CA`
environment variables as [the single-request test](MYSQL.md). Missing variables
or an invalid port are errors. This fault-relay program requires
`ARK_DATABASE_MYSQL_HOST=127.0.0.1`; other hosts are rejected before connecting.
It always verifies TLS; it creates and removes a
unique `ark_transaction_<number>` InnoDB table. Use a disposable test database
with permission to create/drop that table and inspect the test account's running
statements. Do not use an application database.

The real-service executable also binds random IPv4 loopback ports for transparent
TCP fault relays. They forward TLS without decrypting it, then deliberately drop
server responses after earlier requests have completed. They never belong in
the library or deployment configuration.
Prepared-statement leak checks observe the server's global `Prepared_stmt_count`;
use an otherwise idle disposable MySQL instance so other clients cannot change
that counter during a check.

## Ownership and execution

`mysql_transaction_service` receives an **existing**
`std::shared_ptr<arknet::mysql::pool>` and explicit
`mysql_transaction_limits{max_transactions, max_input_bytes}`. It creates no
connection pool or worker. Transaction admission has its own explicit limits;
the native pool retains its connection/waiter/result limits. This owner is not
automatically attached to `mysql_service::update()` in this first module.

```cpp
ark::detail::mysql_transaction_service transactions(existing_pool, {16, 65536});
transactions.start({std::chrono::seconds(5), std::chrono::seconds(1)},
    [](ark::detail::mysql_transaction_reply begun) {
        // Move begun.transaction into the module's owning state, then submit
        // queries or prepared execution, then explicitly commit or roll back.
    });
// Run on the module thread during normal updates:
transactions.update();
```

`mysql_transaction` is move-only. One native lease covers acquisition, BEGIN,
every query/prepared execution and the terminal COMMIT/ROLLBACK. At most one user step is in
flight per scope; concurrent steps are rejected without queuing or changing the
transaction. Scope slots remain reserved during acquisition, idle ownership,
cleanup and undispatched completion/capture destruction. SQL string capacity,
parameter-vector capacity and owned string-parameter capacities are charged
until delivery releases its record. The fixed BEGIN and cleanup command
storage is created before BEGIN admission, and cleanup does not need another
business completion record.

Accepted native callbacks only move owning results into pre-reserved state;
`update()` advances the scope and dispatches user callbacks on the constructing
thread. Rejected requests return ID zero and may call back inline. Request IDs
are logical and never reused; stale cancellation cannot target a later step.
Each update advances, dispatches and reaps at most `maximum` records in each
bounded phase, using round-robin state inspection. `update(0)` does no work.

The existing `database_awaitable<mysql_transaction_reply>` and `module_task`
can wrap these callbacks without a new coroutine promise. The formal consumer
uses the library-provided manual runtime and the usual update sequence:

```cpp
transactions.update();
bridge.dispatch();
tasks.poll();
```

## Results, deadlines and cleanup

- `reply.result` is the native owning query result; `initiation_exception` keeps
  a local initiating exception. `bool(reply)` checks only this business result
  and exception; it does not certify cleanup or commitment. BEGIN additionally
  returns an owning `transaction` only when it is active.
- `start_execute(sql, parameters, completion)` prepares, executes and explicitly
  closes one statement on the held lease. Parameters and results own their data,
  including embedded NUL bytes. `mysql_transaction_execute()` supplies the same
  lazy Asio coroutine factory as the existing transaction operations.
- `reply.statement_close` and `statement_close_exception` preserve statement
  cleanup independently from the business result and automatic rollback. MySQL's
  [COM_STMT_CLOSE](https://dev.mysql.com/doc/dev/mysql-server/8.4.10/page_protocol_com_stmt_close.html)
  has no server response; success means the native close operation completed,
  not that a server acknowledgement or transaction commit was received.
- `reply.rollback` is a separate native result for automatic cleanup. An explicit
  rollback returns its operation result in `reply.result`. Neither replaces an
  earlier SQL error or a successful write result.
- `transaction_status` and `snapshot()` distinguish `not_started`, `active`,
  `committed`, `rolled_back` and `unconfirmed`. They retain cleanup/cancellation/
  lease-return exceptions, rollback code/uncertainty and whether discard was
  requested. A successful query is not proof of transaction commitment.
- One absolute deadline begins before acquiring the lease and covers BEGIN,
  all steps and COMMIT. Each native call receives only the remaining duration.
  In-flight calls use the native timeout rather than a competing owner-side
  cancellation. Idle expiry and abandoned handles are observed when the owner
  is updated. Prepare, execute and statement close all share that same business
  deadline. Expired/failed statement close preserves the business result and
  discards the lease with an unconfirmed transaction outcome.
- Automatic rollback gets one separately configured grace deadline, shared by that cleanup
  attempt. Repeated cancellation cannot cancel the internal rollback or give it
  another grace interval. A lost reply/closed connection is discarded with an
  unconfirmed outcome, without replaying a write or COMMIT.
- Cancellation after prepare has published its native result, before the owner
  consumes it, prevents execution. The owner closes the prepared statement and
  attempts rollback without cancelling an already completed native request.
  A sent operation's unknown result remains unknown and causes discard instead
  of another execute, statement-close attempt or fictitious rollback.

All normal methods and snapshot reads use the module thread. Handle destruction
or `abort()` only records an abandonment request and may run on another thread;
the owner must continue updating to observe rollback. Destruction is not an
asynchronous RAII rollback guarantee. Destroying the transaction service suppresses
its undispatched callbacks and requests discard of its held leases; it does not
close the caller's shared pool or establish an IO/DSO drain barrier. The test
owner explicitly calls the native pool's `close_and_wait()` after its work ends.

The scope sends `COMMIT AND NO CHAIN NO RELEASE` and the corresponding rollback,
so server `completion_type` cannot silently start another transaction or disconnect
the scope. MySQL defines those completion clauses in the [official transaction
syntax](https://dev.mysql.com/doc/refman/8.4/en/commit.html).

Use transactional DML/query statements on an appropriate engine such as InnoDB.
DDL, nontransactional tables, transaction-control SQL supplied as a query and
stored programs with internal commits are outside the atomicity guarantee. The
module does not attempt a SQL parser. An unexpected end-of-transaction status
stops the scope with an unconfirmed outcome; it cannot undo an earlier implicit
commit. See [implicit commits](https://dev.mysql.com/doc/refman/8.4/en/implicit-commit.html)
and [InnoDB error handling](https://dev.mysql.com/doc/refman/8.4/en/innodb-error-handling.html).
Any accepted SQL error ends the scope and attempts cleanup rather than assuming
that MySQL rolled back the whole transaction.

## Validation scope

The formal native programs cover fixed connection identity, isolation before
commit, explicit/automatic rollback, completion modes, binary owning results,
bounded admission, idle/acquire deadlines, sent cancellation, stale IDs,
abandoned/throwing deliveries, cross-thread move/abandonment, and coroutine
ownership on the module thread.
Prepared execution additionally covers binary/typed parameters, commit/rollback,
prepare/execute errors, parameter-count rejection, statement deallocation,
deadline exhaustion between preparation and execution, sent prepare/execute
cancellation and cancellation at the native-publication/owner-poll boundary.
The fault relay confirms an actual committed write while the client loses its
COMMIT reply, and separately exercises a lost cleanup reply. These are functional
and lifetime checks, not throughput or deployment-capacity measurements.

Attachment to the old service entry points, public plugin configuration,
savepoints and normal application shutdown remain
separate modules. No Windows/Linux, arbitrary allocator-fault or public-plugin
acceptance is implied by the local test programs.
