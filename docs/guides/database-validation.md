# Database service acceptance

The four public service/failure invocations passed on **2026-10-21** using isolated local servers.

| Environment | Value |
| --- | --- |
| Machine | Mac mini · Apple M4 Pro · 14 cores · 48 GiB |
| System | macOS 27.0.1 (26A434) |
| Build | Apple Clang 21.0.0 · C++20 · Boost.Asio · Debug |
| ark | `61ad91ae59e93391e03d685fa01ddf02ba0ae98e` |
| arknet | v0.2.2 · `c5432ca176edf9f4c06db03713f03764184c5cea` |
| Services | Redis 7.4.2 over authenticated TCP; MySQL 8.4.11 with authentication and verified TLS |

| Program mode | Result | Coverage |
| --- | --- | --- |
| Public service · Redis | Passed | Binary pipeline, follow-up requests, input/result ownership, owner-thread resumption and drain |
| Public service · MySQL | Passed | Query/prepared results, commit/rollback, ownership, owner-thread resumption and drain |
| Public failure · Redis | Passed | Wrong password, server-observed blocking-command timeout and explicit-request recovery |
| Public failure · MySQL | Passed | Wrong password, untrusted CA, server-observed slow-query timeout and explicit-request recovery |

Both normal workflows also passed host-stack unwinding while requests were pending. Server processes and data were dedicated to these tests; the processes were stopped after completion. Reproduce using the [public service](../../tests/database/README.md#public-modules-against-real-services) and [failure](../../tests/database/README.md#public-real-service-failure-checks) commands. Results cover these functional scenarios on one machine; cross-machine capacity, request cancellation, server termination and dynamic-library unloading require separate acceptance.
