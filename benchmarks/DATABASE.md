# Private database callback/coroutine benchmark

[简体中文](DATABASE_CN.md)

This optional C++ program compares the private `redis_service` / `mysql_service` callback path with `database_awaitable`, Asio `awaitable`, `manual_runtime` and `module_task_group`. It does not activate database plugins or change published interfaces or module shutdown policy.

## Build

Use an explicit local arknet checkout with native pools and `manual_runtime`, following the [database development prerequisites](../tests/database/README.md). The target is off by default and independent of `ARK_BUILD_BENCHMARKS`.

```sh
cmake -S . -B database-bench-build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_PROTO=OFF \
  -DARK_BUILD_TESTS=OFF -DARK_BUILD_CORE_TESTS=OFF \
  -DARK_BUILD_DATABASE_BENCHMARKS=ON -DARKNET_ENABLE_SSL=ON \
  -DARKNET_SOURCE_DIR=/absolute/path/to/arknet
cmake --build database-bench-build --target ark_database_benchmark
cmake --build database-bench-build --target check-database-benchmark
```

The local self-test requires no server. `ASIO_STANDALONE=ON` selects Standalone Asio; the default is Boost.Asio. MySQL requires `ARKNET_ENABLE_SSL=ON`; a NoTLS build supports Redis only. The executable runs directly, without Python, Ruby or a launcher. It neither starts nor stops database servers.

## Connections and functional checks

Provide endpoints and credentials only through the existing database-test environment names. The program accepts no credential options and does not print connection configuration.

| Protocol | Required environment | Optional environment |
| --- | --- | --- |
| Redis | `ARK_DATABASE_REDIS_HOST`, `ARK_DATABASE_REDIS_PORT` | `ARK_DATABASE_REDIS_USER`, `ARK_DATABASE_REDIS_PASSWORD` |
| MySQL | `ARK_DATABASE_MYSQL_HOST`, `ARK_DATABASE_MYSQL_PORT`, `ARK_DATABASE_MYSQL_USER`, `ARK_DATABASE_MYSQL_PASSWORD`, `ARK_DATABASE_MYSQL_DATABASE`, `ARK_DATABASE_MYSQL_CA` | None |

MySQL password must be present but may be empty. MySQL uses verified TLS with the required CA and the host's certificate identity; there is no insecure TLS switch. Redis uses the private plain TCP Redis service. Use an explicitly provisioned test endpoint and read-only database permissions. The workload sends only Redis `ECHO` or MySQL `SELECT X'...'`; it creates no keys, tables or transactions. Pool authentication and normal connection reset/reinitialization remain part of the native service behavior.

After supplying the environment, run a few operations to verify functionality before considering a performance run:

```sh
for protocol in redis mysql; do
  for mode in callback coroutine; do
    ./database-bench-build/benchmarks/ark_database_benchmark \
      --protocol "$protocol" --mode "$mode" --concurrency 1 --window 1 \
      --payload 513 --warmup-operations 2 --operations 4 --update-us 1000
  done
done
```

Each successful process emits one JSON record, has `valid: true`, zero errors and exits 0. A native failure, local exception, refusal, invalid result or phase deadline makes the record invalid and exits 1. Pending accepted work is cancelled and pumped to its real terminal completion. If that drain itself fails or exceeds the operation timeout plus ten seconds, the program reports failure and exits 2 without destroying suspended coroutine frames. Startup/environment failures exit 1 with an error message. There are no retries, detached tasks or synthesized successful completions.

## Matched workload and bounds

Run callback and coroutine modes in separate processes with identical arguments and server configuration. Each window slot holds at most one accepted logical operation. Callback replacements are submitted after `service.update()` returns, when its completion reservation has retired. Coroutine replacements resume through the bridge on the owner-polled manual executor. Both modes share payload construction, native operation submission, per-operation timestamping, result validation and reporting. No additional benchmark request queue is used; the native pool waiter limit, service quota, bridge quota and task limit are bounded by the declared window.

`--concurrency` is the native pool connection limit, not an application worker count. There is one owner thread; database pools use their normal native IO threads. `--window` is the maximum number of outstanding logical requests, including those waiting for a pool lease or owner delivery. Redis performs one ECHO per request; MySQL performs one text SELECT per request, not a prepared-statement workload or protocol pipeline.

The payload is a deterministic byte sequence `00 01 ... ff 00 ...`. Every native owning result is checked against the complete binary string, including embedded NULs. Each slot retains the previous result and checks it again after the next operation, so the check covers ownership across native operations. Latencies use a vector reserved for the exact configured operation count before warmup; neither mode allocates a sample record per request. All results are sampled with the same clock calls.

| Option | Default | Accepted range / meaning |
| --- | --- | --- |
| `--protocol` | `redis` | `redis` or `mysql` |
| `--mode` | `callback` | `callback` or `coroutine` |
| `--concurrency` | 4 | 1–64 native pool connections |
| `--window` | 16 | 1–256 logical operations |
| `--payload` | 64 | 1–65536 binary bytes returned per operation |
| `--warmup-operations` | 1000 | 1–100000 completed unmeasured operations |
| `--operations` | 10000 | 1–1000000 measured operations |
| `--update-us` | 1000 | 0–1000000 microseconds between owner updates; 0 continuously polls |
| `--update-budget` | 64 | 1–4096 service deliveries / bridge deliveries / manual executor handlers per update, separately |
| `--timeout-ms` | 3000 | 1–30000 milliseconds covering native acquisition and execution |
| `--deadline-seconds` | 60 | 1–300 seconds per phase; failure cancels and drains pending operations |

Require `concurrency <= window <= operations` and `window <= warmup-operations`. Cadence sleeps do not catch up missed ticks; processing time can make actual updates farther apart. `--help` prints the supported options. Vary one dimension at a time and retain all arguments, source revisions, compiler/options, server version/configuration and raw JSON records with results. Do not infer saturation capacity from a closed-loop workload.

## Measurement scope

Pool/service/runtime construction and warmup requests are outside the measured interval. Warmup finishes with all its logical native results delivered and all bridge/task records retired. Background pool lease reset is not observable through the private service API; the first measured acquisition may wait for the last warmup reset, as later acquisitions wait for prior resets. This benchmark does not claim an IO-quiescence or plugin-unload barrier.

The measured interval starts before the first measured submission and ends after every measured result and task completion. It includes pool acquisition/reset waits, network and database time, owner update cadence, command/result ownership, validation and closed-loop refill. Coroutine task launch/completion and bridge work are included. Latency specifically measures owner submission to owner result delivery, before content validation, in both modes. P50 and P99 use nearest-rank percentiles over every delivered measured request; failed runs are invalid regardless of their metrics.

Process CPU percentage uses all client process threads; 100% means one logical core. External database CPU is excluded, even if the server shares the machine. Peak RSS is the process lifetime resident-memory high-water mark, including startup, warmup and preallocated latency storage; it is not interval-only memory, allocation traffic or database memory. The JSON record states both scopes. Throughput is successful logical operations divided by measured wall seconds. Error counters distinguish native outcomes, C++ exceptions, content/ownership failures, admission refusals, task failures and phase deadlines; categories may overlap for a single failed request.

Callback mode also constructs an empty bridge, manual runtime and task group, so RSS does not represent the minimum callback API footprint or isolate coroutine frame allocation costs.

For comparable performance evidence, use a quiet machine, fixed server placement/configuration and repeated separate processes, alternating callback/coroutine order. Sanitizer and bounded functional runs verify correctness and must not be reported as performance measurements. No performance results are included with this development target.
