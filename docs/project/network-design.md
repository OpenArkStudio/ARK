# Cluster, router and zone design notes

<Aside type="note" title="Draft">

This is an explanation of a historical topology and remote-logging proposal. It does not define a released discovery, routing, high-availability or log-delivery contract. Current connection/message APIs are documented in [Bus](../reference/bus.md).

</Aside>

## Proposed process responsibilities

The note divided processes into three levels:

| Level | Intended role |
| --- | --- |
| `cluster` | Services shared across zones; Master manages the cluster topology |
| `router` | A message-forwarding intermediary between zone and cluster processes |
| `zone` | Per-zone processes; World manages the zone topology |

![Reference roles for cluster, router and zones](../assets/server-topology.svg)

The proposed routing rules kept zone-internal traffic within its zone. Messages destined for the cluster could go through Router, which would select a route using the header's historical `dst_bus_id` destination. All cluster processes would connect to Master and Router, while selected zone processes would connect to Router.

These are proposed responsibilities and connection rules. They do not themselves define authentication, route convergence, retry behavior, congestion control, duplicate delivery or state recovery. The [server architecture](../concepts/server-architecture.md) explains how the repository's example roles relate to a deployment reference.

## Historical remote-logging idea

The note also proposed that every process establish a connection to a log server before beginning business work. A log-client configuration would contain the remote address/port and log format.

That startup gate is an idea, not an implemented framework-wide requirement. A successful initial connection would not by itself guarantee that all subsequent logs are delivered or retained. Delivery acknowledgement, buffering limits, backpressure, authentication, reconnect behavior and what to do when logging is unavailable still require a defined policy. This page does not choose one.

## Use the current framework without assuming the proposal

Applications can compose the existing role plugins and configure their network/Bus relationships. Use their actual configuration and APIs rather than inferring behavior from a process name. Neither Consul nor a custom Registry becomes mandatory merely because a deployment uses multiple zones. Registry consistency, leases and failover remain separate design work.

This proposal does not establish cross-host capacity, automatic topology recovery, storage consistency or lossless remote logs. Preserve those as explicit deployment tests and design requirements; see [benchmarks and acceptance](../performance/benchmark.md).

The Chinese [original note](https://github.com/OpenArkStudio/ark/blob/main/docs/site/legacy-source/design/network.md) is retained in the [source archive](https://github.com/OpenArkStudio/ark/blob/main/docs/site/legacy-source/README.md). The English page is a current explanation of that historical text, not evidence of an English original or an accepted implementation decision.
