# Private database integration tests

[简体中文](README_CN.md)

These development tests exercise internal native database services. They do not
replace the published `mysql_module` / `redis_module` interfaces, select the new
clients for runtime plugins, remove old drivers or change plugin registration.
`ARK_BUILD_DATABASE_TESTS` is `OFF` by default.

## Build

Initialize the existing dependencies described in [network builds](../../docs/networking.md).
Use an explicit local arknet checkout containing the native Redis/MySQL clients
and `arknet/base/manual_runtime.hpp`;
these tests never change the production dependency pin or fetch a feature branch.

```sh
cmake -S . -B database-build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  "-DVCPKG_MANIFEST_FEATURES=boost;database" \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_PROTO=OFF \
  -DARK_BUILD_TESTS=OFF -DARK_BUILD_CORE_TESTS=OFF \
  -DARK_BUILD_DATABASE_TESTS=ON -DARKNET_SOURCE_DIR=/absolute/path/to/arknet \
  -DASIO_STANDALONE=OFF -DARKNET_ENABLE_SSL=ON
cmake --build database-build --target ark_database_queue_tests \
  ark_database_task_tests ark_database_task_contract_tests \
  ark_database_redis_stop_tests ark_database_redis_tls_tests \
  ark_database_redis_tests ark_database_mysql_tests \
  ark_database_service_awaitable_tests --parallel 2
```

For Standalone Asio, set `ASIO_STANDALONE=ON`,
`VCPKG_MANIFEST_NO_DEFAULT_FEATURES=ON` and `VCPKG_MANIFEST_FEATURES=database`;
provide `ARKNET_ASIO_INCLUDE_DIR` when needed. MySQL is built only when
`ARKNET_ENABLE_SSL=ON`: OpenSSL is required for authentication even with an
explicit unencrypted MySQL connection. Redis's plain connection tests can be built
without this option. No new test framework is downloaded: queue tests use the
existing `tests/network/doctest.h`, as core/network tests do; real-service checks
are standalone C++ executables.

## Run local regressions

```sh
cmake --build database-build --target check-database
# Queue, stop and coroutine regressions; programs can also run directly.
bash scripts/test_databases.sh database-build local
./database-build/tests/database/ark_database_queue_tests --list-test-cases
```

The queue cases cover count/retained-input admission, completed-but-undispatched
operations, concurrent producers, owner-thread dispatch, callback exceptions,
reentrancy and stop-capture release. The Redis stop regression verifies that a
capture destructor can destroy its service during `stop()`; it requires no
running database and uses only IPv4 loopback port 1.
The state regression also verifies failed-acquire error/reply mapping using explicit
input states; it does not reproduce or claim a native connection-pool fault.

The real-service targets are not run by `check-database`. Missing endpoint
configuration is an error, not a silently skipped successful check.

With SSL enabled, local checks also include the private transaction state tests.
The separate [MySQL transaction test](TRANSACTIONS.md) requires a disposable real
service and exercises one native lease across multiple statements and completion.

## Redis TLS regression

With `ARKNET_ENABLE_SSL=ON`, `check-database` also runs the native TLS regression.
It can be built and run separately:

```sh
cmake --build database-build --target check-database-redis-tls
# Or: ./database-build/tests/database/ark_database_redis_tls_tests
```

The private `redis_service` alias keeps its plain connection API. The private
`tls_redis_service` uses the same request, cancellation and owner-thread delivery
implementation with arknet's TLS pool. It verifies the certificate chain and the
configured connection host; an unrelated CA or wrong hostname must fail before
any RESP command, including `HELLO`, reaches the test endpoint. A successful
binary pipeline retains owning results after service destruction. A sent-request
cancellation returns its terminal result through `update()` and the module
coroutine executor, preserving `command_may_have_run`.

The executable binds a random IPv4 loopback port and needs no running Redis
service. Its bounded RESP-over-TLS protocol test endpoint checks only these
contracts; it is not a real Redis compatibility or capacity test. It reads the
public test-only certificates and key directly from
`ARKNET_SOURCE_DIR/tests/certs` (arknet, BSL-1.0; see that checkout's `LICENSE` and
certificate README). The fixtures remain in that dependency checkout and are
never copied into the application. They must never be used in deployment.
`ARKNET_ENABLE_SSL=OFF` omits this target and leaves the plain service available.

## Redis service check

Provide an isolated Redis-compatible service. The check creates a unique
`ark-private-service-...` key, verifies binary SET/GET and deletes that key. It also
checks owning results, module-thread dispatch, admission recovery, stop suppression
and service destruction from a business callback.

For example, start a disposable local service in another terminal:

```sh
redis-server --bind 127.0.0.1 --port 26791 --protected-mode yes \
  --save '' --appendonly no
```

Then run:

```sh
export ARK_DATABASE_REDIS_HOST=127.0.0.1
export ARK_DATABASE_REDIS_PORT=26791
# Set ARK_DATABASE_REDIS_USER / ARK_DATABASE_REDIS_PASSWORD when required.
bash scripts/test_databases.sh database-build redis
# Or: ./database-build/tests/database/ark_database_redis_tests
```

Stop the disposable server after testing. The runner never starts/stops services,
installs tools or selects a database endpoint for the real-service checks. Do not
point write-enabled checks at application data. MySQL settings and workload are
specified in [MYSQL.md](MYSQL.md). `all` runs the local, Redis and enabled MySQL
executables and requires both real-service configurations.

## Internal contract and lifetime

The library owns network IO. Native callbacks only move owning results into the
module's reserved completion records; business callbacks run from `update()` on
the constructing thread. The service must be submitted, updated, stopped and
destroyed on that thread. Input container/string capacities are charged while
waiting for a connection; reservations remain occupied until business callback
execution and capture destruction finish. Native result-size limits still apply;
arbitrary user callback captures are outside the input-byte quota.

The existing `submit*()` methods return false for an unaccepted request with no
callback. New private `start*()` methods return a logical request ID and report
rejection through a terminal callback even when returning ID zero. Their
`cancel(id)` requests cancellation; `stop_accepting()` closes admission while
preserving accepted terminal delivery. Accepted operations are
completed once unless `stop()` removes their not-yet-started business callbacks.
One checked deadline covers acquisition and execution. Native server/transport
errors, owning results and uncertain write outcomes are preserved; no write is
replayed. The planned public API uses native arknet owning results and favors
asynchronous/coroutine calls. The separate [coroutine prototype](COROUTINES.md)
documents its owner executor, bounded delivery, logical cancellation and explicit
close/drain tests; it does not change the published modules or plugin lifecycle.

`stop()` suppresses business callbacks and requests pool closure. It **does not
wait** for native callbacks, transports, TLS resources or IO threads to be destroyed.
The fix for reentrant capture destruction pins the queue and closes the native pool
before clearing captures; it does not add an IO-drain barrier. These helpers are
not registered with runtime plugins or approved for dynamic-library unload.
