# Network buffer benchmarks

[简体中文](README_CN.md)

Compile and run the C++ program directly. It measures ark's message-framing buffers; it does not open sockets or measure network capacity.

```sh
cmake -S . -B bench-build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_BENCHMARKS=ON
cmake --build bench-build --target ark_network_buffer_benchmark
./bench-build/benchmarks/ark_network_buffer_benchmark \
  --mode framing --payload 1024 --chunk 64 --batch 16 --iterations 1000000
```

Initialize dependencies as described in [network builds](../docs/networking.md).

| Option | Meaning |
| --- | --- |
| `--mode sessions --sessions 10000` | Construct and retain sessions; count ordinary C++ allocation requests |
| `--mode framing` | Decode frames, checking every message's ID, content and delivery count |
| `--payload`, `--chunk` | Message size and input fragment size, in bytes |
| `--batch` | Messages submitted per batch |
| `--warmup-iterations` | Unmeasured warmup batches; default 10000 |
| `--initial-buffer` | Initial capacity in bytes; omitted uses the library default |

JSON output includes throughput, process CPU percentage and C++ allocation counts/bytes. **100% CPU means one logical core.** Requested allocation bytes are not resident memory; aligned allocations, queue-internal `malloc` and socket buffers are excluded.

The optional shell runner repeats the standard cases; no interpreter is needed to launch the C++ executable itself:

```sh
bash benchmarks/run_network_buffers.sh ./bench-build/benchmarks/ark_network_buffer_benchmark 3
```

Published report generation uses `report_receive_buffers.mjs`, which checks measurement counts and source hashes before producing bilingual Markdown and SVG charts. Only data supporting a published report belongs in the repository; other local output stays outside the source tree.
