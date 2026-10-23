# Utility plugin

`ark_utility_plugin` provides GUID, timer, schedule and compression modules. Link a consumer to `ark::utility` for public headers and usage requirements; select `ark_utility_plugin` in the application manifest to load its implementation. A CMake target dependency alone does not load modules.

## Public interfaces

| Header | Interface and behavior |
| --- | --- |
| `<ark/utility/guid_module.hpp>` | `guid_module::create_guid()` and `parse_uid()`; existing GUID modes and bus-derived worker bits are unchanged |
| `<ark/utility/timer_module.hpp>` | Single/repeating timers driven by manager timestamps; only this Utility module registers for owner-thread updates |
| `<ark/utility/schedule_module.hpp>` | Interval, target-time and cron callbacks; preserves the existing scheduler worker configuration |
| `<ark/utility/compress_module.hpp>` | `compress()` / `decompress()` for zlib and Zstandard, including binary payloads |

Obtain interfaces during module initialization, after plugin registration:

```cpp
#include <ark/utility/guid_module.hpp>
#include <ark/utility/timer_module.hpp>

auto* guids = manager.find_module<ark::guid_module>();
auto* timers = manager.find_module<ark::timer_module>();
const auto entity = guids->create_guid();
```

Pointers are borrowed from the loaded plugin. Keep the owning plugin alive while using them. Timer member callbacks also borrow their receiver; retain the receiver until its callbacks finish or are removed. Advance the manager on its owner thread using millisecond timestamps; creating a timer does not advance it. The schedule module uses its existing background scheduler, whose thread-pool configuration remains 16. This migration does not add callback cancellation, hot reload or new shutdown guarantees. Finish producers and callbacks according to the [module lifecycle](../guides/modules.md) before unload.

The plugin keeps installation order GUID, timer, schedule, compression and reverses that order during uninstallation. Timer `pre_shut()` still shuts down its time wheel. GUID layout, compression levels/algorithms and scheduler behavior are unchanged. The library stem remains `ark_utility_plugin`, with the existing platform extension and Debug `_d` postfix.

## Compression buffers and concurrency

The core helpers `<ark/compress/zlib.hpp>`, `<ark/compress/zstd.hpp>` and
`<ark/compress/lz4.hpp>` use an owned heap buffer for each call. Successful calls
append to the supplied output string; rejected input leaves it unchanged. Callers
must supply a readable input range and synchronize any shared output string.
The existing compression policy requires the codec's worst-case output bound to
be strictly below 1 MiB. Decompression accepts at most 1 MiB of output, including
exactly 1 MiB. Zstandard requires a known frame content size and rejects larger,
unknown-size or invalid frames before decoding. Invalid compressed data returns
`false`. LZ4's existing empty-block decompression result remains `false`.

zlib, LZ4 and Zstandard normal calls have no shared writable output buffer.
Zstandard dictionary calls still mutate their instance's cached contexts: use a
separate `zstd_compress` instance per thread, or externally serialize dictionary
initialization, dictionary calls and destruction. Dictionary initialization,
ownership and copy behavior retain their existing contract. This change does not
establish same-instance dictionary concurrency or plugin unload safety.

Enable `ARK_BUILD_COMPRESSION_TESTS=ON` and the vcpkg `compression-tests` feature
to run `cmake --build <build-dir> --target check-compression`. The target is also
part of `check-core` when enabled; Applications CI enables it explicitly. It
checks the mapped public headers, capacity boundaries, malformed frames, append
behavior and parallel round trips with separate dictionary instances. LZ4 remains
a direct core helper; the Utility module still selects zlib or Zstandard.

## Source and build-tree SDK migration

Public headers now have their single source in `src/plugins/utility/`. CMake copies seven explicitly listed public headers into the build tree's `src/plugins/sdk-include/ark/utility/`: the four module interfaces plus `scheduler.hpp`, `ctpl.h` and `ctpl_stl.h`. Consume `ark::utility` to retain all existing `<ark/utility/...>` public/helper include paths, including with runtime plugins disabled. Do not add the private plugin directory to consumer include paths. The upstream thread-pool headers retain their original bytes and formatting rules. For installed consumption, use the [CMake package](../get-started/installation.md#install-and-consume-the-cmake-package) with `find_package(ark CONFIG REQUIRED COMPONENTS utility)` and the same `ark::utility` target. Installed public headers retain `<ark/utility/...>` paths; the runtime plugin still needs to be selected in the application manifest.

**Breaking source change:** `ark/utility/{guid,timer,schedule,compress}_module_impl.hpp` and `ark/utility/utility_plugin.hpp` are removed from the SDK. Implementations are defined in their cpp files; the plugin type and interface-returning factories are private. Replace direct implementation construction with public interface lookup or an application-owned implementation. Handwritten `-I <ark-source>/include` consumers must use target-provided include roots. Rebuild the plugin and consumers together; unchanged public virtual functions and exported entry signatures do not establish compatibility with old implementation-class binaries.

## Verification

`check-typed-examples` loads the actual Utility DSO with `timers`, `module_lifecycle` and `entity_operations`. It verifies all four interface RTTI keys and manager ownership, GUID generation/parsing, zlib/Zstandard binary round trips, a timer triggered by owner-thread time advancement, and normal/idempotent shutdown. `check-app-presets` exercises the shipped launcher preset; `check-core` includes both when applications, examples, Kernel and Utility are enabled. The independent `tests/consumer` project's `check-sdk` compiles public/helper paths with runtime plugins disabled and rejects visible implementation or assembly headers. These are correctness checks, not performance or arbitrary callback-lifetime acceptance.
