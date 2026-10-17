# Database coroutine prototype

[简体中文](COROUTINES_CN.md)

This opt-in prototype uses Asio `awaitable`, `async_initiate` and `co_spawn`.
It does not add a coroutine promise implementation or change the published
database modules. Task ownership is available through the public `<ark/task.hpp>`, with the former private names forwarding to that same implementation. The database operation helpers and bridge remain private under `src/plugins/`.

## Execution and ownership

`module_task<T>` is an alias for Asio's `awaitable<T>`. A
`module_task_group` owns every task accepted by `start()` until its final
completion. A returned task ID is an observation/cancellation ID: dropping it
does not destroy a suspended coroutine. Dropping an unstarted, lazy awaitable
performs no native operation.

Construct the shared arknet `manual_runtime`, group, bridge and service on the
module thread. The library owns the execution context; this runtime has no IO
runner. Native database pools keep their library-owned IO threads. All group
operations, delivery scheduling and task completions belong to the module
thread. Other Asio platform helpers are independent of this owner-poll contract.

One module update performs, in order:

```cpp
service.update();  // Move native terminal results into reserved bridge records.
bridge.dispatch(); // Schedule one bounded delivery batch on the manual executor.
tasks.poll();      // Run ready Asio handlers and coroutine continuations.
```

The bridge requires its task's executor to equal that manual runtime's executor;
a different executor is rejected before native submission. Invoking a service
callback on the module thread alone does not satisfy this executor contract.

## Private operation functions

`mysql/detail/mysql_awaitable.hpp` provides `mysql_query(service, bridge, sql, timeout)` and `mysql_execute(service, bridge, sql, parameters, timeout)`. Both return `module_task<mysql_service_reply>` and preserve the native query result, optional statement-close result and independent statement-close exception. MySQL requires the SSL-enabled development build, as the service does.

`redis/detail/redis_awaitable.hpp` provides `redis_command(service, bridge, command, timeout)` and `redis_pipeline(service, bridge, commands, timeout)`. Both return `module_task<arknet::redis::result>` and accept the existing plain or TLS service. TLS requires an SSL-enabled build. Pipeline reply order, binary data, server error values and `command_may_have_run` remain unchanged.

These are private function templates for the existing service callback contract, not runtime-polymorphic interfaces or published plugin APIs. SQL, parameter vectors and commands are passed by value and moved into the lazy wait during the factory call. They survive changes to or destruction of the caller's original input before the task starts. Dropping the unstarted task issues no request. Keep the referenced service and bridge alive until the returned task completes or is discarded unstarted; the functions do not own them or drain them on destruction.

```cpp
auto operation = ark::detail::redis_command(service, bridge, {"ECHO", payload}, 3s);
auto result = co_await std::move(operation);
```

The caller supplies the timeout explicitly. It starts when the lazy operation initiates the service request and retains the existing acquire-plus-command deadline rather than granting each stage a fresh budget. Cancellation forwards only a nonzero accepted service request ID and waits for its terminal callback. An inline refusal with ID zero still delivers through the bridge and has nothing to cancel. A native error or uncertain write is returned unchanged and is never replayed; a local C++ failure is rethrown through the existing bridge. These functions add no executor, worker thread or normal-stop policy.

`ark_database_operations_tests` runs without a database server. Controlled service callbacks verify ownership and result forwarding, and real plain/TLS service instances verify local invalid-deadline refusal without connecting. These checks do not replace separately configured real-service tests; `ark_database_service_awaitable_tests` now uses the same operation functions for those tests.

## Private transaction functions

`mysql/detail/mysql_transaction_awaitable.hpp` provides four lazy operation factories, available in SSL-enabled development builds:

| Function | Inputs |
| --- | --- |
| `mysql_transaction_begin(service, bridge, options)` | Transaction total timeout and independent cleanup timeout, owned by value |
| `mysql_transaction_query(transaction, bridge, sql)` | SQL owned by value; the existing transaction deadline applies |
| `mysql_transaction_commit(transaction, bridge)` | The same transaction handle and its remaining deadline |
| `mysql_transaction_rollback(transaction, bridge)` | Explicit rollback through the same transaction handle |

Each ordinary factory immediately captures its inputs and returns `module_task<mysql_transaction_reply>` through the existing bridge. It performs no native request until polled. Dropping an unstarted task performs no request. Keep the referenced service, bridge and transaction handle alive and unmoved until the task completes or is discarded unstarted; moving the handle after creating a lazy step invalidates that step's intended target. Move the handle out of a successful BEGIN reply before creating subsequent steps. Only BEGIN accepts options; subsequent steps do not reset the transaction's total deadline or independent cleanup budget.

The move-only reply is forwarded whole: the owning handle, query result, optional rollback result and transaction snapshot remain intact. In particular, a reply's `initiation_exception` stays in that reply with any cleanup, cancellation or lease-return diagnostics; it is not rethrown while discarding the other fields. A C++ exception thrown by initiation before acceptance still follows the bridge's exception path. Unknown outcomes remain unknown, with no retry or invented successful commit. Cancellation forwards the accepted logical request ID and waits for the actual callback; an inline ID-zero rejection still resumes on the manual executor.

`ark_database_mysql_transaction_awaitable_tests` requires no database. Controlled callbacks verify ownership, result forwarding, cancellation and quota-one sequential steps; actual service/empty-handle instances verify local refusal only. These checks do not establish real lease or database behavior. The separately configured `ark_database_mysql_transaction_tests` uses these same functions for BEGIN/query/COMMIT and an explicit BEGIN/ROLLBACK sequence. Run its real-service checks using [the transaction instructions](TRANSACTIONS.md); it does not start a database.

## Results, capacity and cancellation

`database_awaitable<Result>` reserves one record per accepted operation, including
completed results waiting for delivery. The count is configurable. Its batch
releases a record immediately before invoking the standard Asio completion
handler, after the source service callback has returned and retired its own
reservation. Therefore `co_await A; co_await B` works with both limits set to one.
Native limits still bound result sizes; arbitrary coroutine locals are not part
of an input-byte quota.

The completion signature is `void(std::exception_ptr, Result)`. Native network,
server, cancellation and timeout results keep their arknet fields, including
uncertain write outcomes. The private MySQL envelope keeps statement cleanup
separate from execution. C++ exceptions remain exceptions; none becomes a
successful native result. A full bridge throws `std::length_error`, a closed
bridge throws `std::logic_error`, and neither starts a native operation or adds a
hidden waiting queue. An inline native refusal still uses a reserved record and
is delivered by the executor batch.

If owner-side batch scheduling throws, ready records remain owned and reserved;
the owner may retry `dispatch()`. Duplicate and late native completions cannot
deliver the task twice. A record-allocation failure before native acceptance is
reported directly through Asio on the already-active manual executor.

Cancellation forwards the service's logical request ID, including cancellation
requested before the native ID is published. It does not manufacture a terminal
result, rewrite an already-ready result, or replay a command. The accepted
operation remains reserved until the service supplies its terminal response.
One service deadline covers connection acquisition and all command phases.
If submitting the native cancellation throws, `cancellation_exception()` retains
the first local diagnostic. The operation still waits for its real terminal;
that diagnostic cannot replace a successful or uncertain native outcome.

## Stop task admission

On the constructing module thread, `tasks.stop_accepting()` makes later
`start()` calls return zero. Repeated calls are harmless; a call from another
thread throws `std::logic_error`, even after admission has stopped. It neither
cancels nor polls accepted tasks. Those tasks can continue through multiple
`co_await` operations and deliver their completion on the owner thread.

Keep the services and bridges accepting while an accepted task may submit its
next operation; their own `stop_accepting()` methods reject those later requests.
`tasks.closed()` becomes true when task admission is stopped and no accepted
tasks remain. Calling `tasks.close()` later still requests cancellation of any
remaining tasks and requires explicit polling to finish them.

This private capability does not select a process-shutdown or plugin-unload
policy, add a deadline, or change the destruction/drain requirements below.

## Explicit shutdown

Begin shutdown on the module thread:

```cpp
service.stop_accepting();
bridge.stop_accepting();
tasks.close(); // Stop task admission and request cancellation of accepted tasks.
```

Keep executing the normal update sequence until `tasks.closed()` and
`bridge.pending() == 0`. Only then destroy the group, bridge, service and the
business objects referenced by their tasks. The final task completion removes
its group membership before user completion code, so that final callback may
destroy the now-empty group facade. Runtime state remains pinned until the
active poll/delivery returns.

Group and bridge destructors require their constructing thread and no pending
operations. Violation calls `std::terminate`; they never recursively poll, block,
detach work or manually destroy a suspended Asio frame. A task which ignores
cancellation still needs its actual terminal completion. There is no automatic
module destructor, plugin unload or arbitrary task-abandonment guarantee here.
The existing service `stop()` still suppresses callbacks and is not a drain
barrier; calling it before coroutine shutdown finishes would break that sequence.

## Build and run

Use an explicit local arknet source containing
`arknet/base/manual_runtime.hpp`, following the [development build](README.md).
The local suite requires no database:

```sh
cmake --build database-build --target ark_database_task_tests \
  ark_database_task_contract_tests ark_database_queue_tests
./database-build/tests/database/ark_database_task_tests
cmake -DPROGRAM="$PWD/database-build/tests/database/ark_database_task_contract_tests" \
  -P tests/database/task_group_contract.cmake
```

The contract program's explicit child modes install a controlled terminate
handler and exit with code 77 for pending/wrong-thread destruction; the CMake
check verifies that code without creating a crash dump. The local suite covers
owner execution, cancellation before ID publication, ready results followed by
cancellation, duplicate completion, scheduling failure/retry and quota-one
sequential operations.

Build the independent real-service executable and provide the same required
environment as the [Redis](README.md#redis-service-check) and [MySQL](MYSQL.md)
tests. It never starts, stops or chooses a database server:

```sh
cmake --build database-build --target ark_database_service_awaitable_tests
./database-build/tests/database/ark_database_service_awaitable_tests redis
./database-build/tests/database/ark_database_service_awaitable_tests mysql
# Equivalent shell entries:
bash scripts/test_databases.sh database-build redis-awaitable
bash scripts/test_databases.sh database-build mysql-awaitable
```

Redis verifies owning binary ECHO/PING replies, native refusal and cancelled
blocking reads. MySQL verifies text/prepared typed results, separate statement
cleanup, native refusal and cancellation of `SELECT SLEEP`. Both use quota one,
verified executor delivery and explicit close/drain. MySQL requires
`ARKNET_ENABLE_SSL=ON` and a matching trusted server certificate. These are local
functional checks, not production capacity measurements or cross-platform
acceptance.

See [Coroutine tasks](../../docs/guides/tasks.md) for public `ark::task<T>` and `ark::task_group` usage. The private `module_task<T>` / `module_task_group` names forward to that same implementation and create no second scheduler.
