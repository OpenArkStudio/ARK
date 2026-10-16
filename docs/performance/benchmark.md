# Benchmarks and capacity acceptance

Correctness, component cost and deployment capacity answer different questions.

| Measurement | Scope |
| --- | --- |
| C++ regression tests | Message correctness, reconnect, shutdown and callback behavior |
| Local component benchmarks | Receive-buffer allocation, framing cost and private database delivery |
| Cross-machine load and endurance | Throughput, latency, CPU, memory and failure recovery at the target load |

The planned deployment target is 100,000–200,000 simultaneous connections across multiple processes. It is not a single-process capacity claim. Define active message frequency, body sizes, process count, connection distribution and failure conditions before measuring it.

Run [network regressions](../networking.md) and consult [arknet's protocol reports](https://openarkstudio.github.io/arknet/performance/) for transport measurements. TODO: publish a complete cross-machine deployment acceptance report.

## Application receive-buffer measurements

The [receive-buffer report](receive-buffers.md) contains the completed local component measurements, machine details, medians/ranges and retained throughput regression. It compares 4, 64 and 512 KiB initial capacities using a compiled C++ benchmark. This complements the deployment matrix above; it does not replace cross-machine load acceptance.

## Database callbacks and coroutines

The [database report](database-coroutines.md) compares callbacks with Asio coroutines through ark's private database service, manual executor and task group: 56 configurations, 336 fresh processes, two payloads, connection and in-flight limits, and continuous or 1 ms owner updates. SVG charts show throughput and CPU; response time, process memory and complete tables are expandable. These short local measurements do not establish deployment capacity.
