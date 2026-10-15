# MySQL service development test

[English](MYSQL.md) | [简体中文](MYSQL_CN.md)

This opt-in executable exercises `plugins/mysql/src/detail/mysql_service.hpp`
against a real MySQL server. The service directly uses the native arknet pool;
it does not change the published MySQL module API, register a runtime module or
replace the legacy driver yet. The internal result type is not a plugin SDK
commitment.

## Build

Use a local arknet checkout containing the native MySQL connection/pool and
`manual_runtime` headers. OpenSSL is required for MySQL authentication and TLS. The development
switch defaults to `OFF`; the ordinary application and plugin selection is
unchanged. Normal ark dependency discovery still applies; use the project's
usual toolchain or dependency prefix when configuring.

```sh
cmake -S . -B build-database \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_PROTO=OFF \
  -DARK_BUILD_TESTS=OFF -DARK_BUILD_CORE_TESTS=OFF -DARK_BUILD_EXAMPLES=OFF \
  -DARK_BUILD_DATABASE_TESTS=ON -DARKNET_ENABLE_SSL=ON \
  -DARKNET_SOURCE_DIR=/path/to/arknet
cmake --build build-database --target ark_database_mysql_tests --parallel 1
```

Boost.Asio is the default. To select Standalone Asio, configure a separate build
directory with `-DASIO_STANDALONE=ON` and, if needed,
`-DARKNET_ASIO_INCLUDE_DIR=/path/to/asio/include`.

## Run

Provide a dedicated test database and account with permission to create, query,
insert into, delete from and drop its tables. The program uses verified TLS;
`HOST` must match the server certificate. It does not start or stop a server.
The following values are examples, not production credentials:

```sh
export ARK_DATABASE_MYSQL_HOST=127.0.0.1
export ARK_DATABASE_MYSQL_PORT=3306
export ARK_DATABASE_MYSQL_USER=ark_test
export ARK_DATABASE_MYSQL_PASSWORD=test-password
export ARK_DATABASE_MYSQL_DATABASE=ark_tests
export ARK_DATABASE_MYSQL_CA=/path/to/test-ca.pem
./build-database/tests/database/ark_database_mysql_tests
```

All six variables are required; only the password may be empty. Missing or
invalid configuration exits with an error instead of reporting a skipped pass.
The program creates a unique `ark_private_db_<clock>` table and drops it on
success. On failure it attempts `DROP TABLE IF EXISTS` for that same table and
reports any cleanup failure. It does not edit other tables or server settings.

## What it verifies

- Text queries and one prepare → execute → close sequence on the same exclusive
  lease. There is no SQL interpolation, cross-lease statement cache or write
  replay.
- Owning results retain signed/unsigned integers, floating-point values, binary
  and UTF-8 strings, SQL NULL, datetime/time values and duplicate column names.
  Saved results remain valid after later commands.
- Native server codes, SQLSTATE, prepare/parameter errors and `outcome_unknown`
  remain available. Statement cleanup is reported separately and cannot change
  a successful execution into a failed write.
- Count and retained-input limits apply through owner-thread dispatch. Native
  result limits are forwarded for both text and prepared queries.
- One deadline covers acquiring a connection and each subsequent command. An
  expired waiter does not execute its write; a sent timeout retains uncertainty.
  Unrepresentable durations are rejected before admission.
- Business completions run only in `update()` on the constructing thread. Tests
  cover destroying the service from a completion and from a capture destructor
  during `stop()`.
- Logical request IDs distinguish waiting acquisition from native command IDs.
  A second connection observes a uniquely tagged server-side sleep before sent
  cancellation is tested. Waiting cancellation does not send its write; sent
  cancellation preserves `outcome_unknown` and separate statement cleanup.
- Cancellation after a terminal result is queued preserves that result, and its
  admission reservation remains charged until owner dispatch. Admission stop
  plus cancellation still delivers accepted terminal continuations.

## Private asynchronous contract

| Method | Admission and completion |
| --- | --- |
| `submit_query()` / `submit_execute()` | Existing Boolean interface: `false` means no admission and no callback. |
| `start_query()` / `start_execute()` | Return a logical request ID. Rejected work returns 0 and still completes with an explicit native status, possibly inline. Accepted work completes through `update()`. |
| `cancel(id)` | Requests native cancellation; `true` means the logical operation was active, not that SQL did not execute. |
| `is_pending(id)` | Observes logical terminal completion. A queued result can return `false` before dispatch or native cleanup. |

All service methods belong to the constructing/module thread. Inputs, outputs
and completion records own their storage. `diagnostics()` retains at most the
last cancellation-submission and explicit lease-return exceptions separately;
neither replaces a known execution result or triggers a write retry.

## Shutdown boundary

`stop()` requests native pool close and removes business callbacks that have not
started. It is **not an IO drain or join barrier**. Native handlers and transport
cleanup may continue after the service is destroyed. A callback already running
continues to return normally, and a sent command may already have executed.

Coroutine shutdown uses `stop_accepting()` followed by `cancel_all()` while owner
dispatch continues. These methods retain terminal delivery for accepted requests;
legacy `stop()` must not be used to discard a suspended continuation. See the
[coroutine prototype](COROUTINES.md) for the manual-executor drain sequence.
Logical completion is distinct from transport cleanup and worker exit. Exceptions
in later native pool-return callbacks remain a separate library boundary.

The private service must not be connected to automatic plugin unload until the
library completion barrier and ark shutdown ordering are defined. Transactions,
public result types and the legacy synchronous API remain outside this
development test. Functional checks do not establish production
capacity or cross-platform acceptance.
