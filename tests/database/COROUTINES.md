# Database coroutine prototype

[简体中文](COROUTINES_CN.md)

This opt-in prototype uses Asio `awaitable`, `async_initiate` and `co_spawn`.
It does not add a coroutine promise implementation or change the published
database modules. The new helpers stay private in `src/plugins/detail/`.

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
