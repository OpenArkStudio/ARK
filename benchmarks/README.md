# ARK benchmarks

`network_buffers.cpp` measures session construction and application-frame parsing.
It is a normal C++20 executable and validates every decoded message. Python and
Ruby are not required. The optional Bash runner repeats the measurement matrix.

## Build

Use ARK's pinned dependencies and a Release build:

```sh
git submodule update --init 3rdparty/args 3rdparty/spdlog 3rdparty/readerwriterqueue
cmake -S . -B benchmark-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES=boost -DASIO_STANDALONE=OFF \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_TESTS=OFF -DARK_BUILD_EXAMPLES=OFF \
  -DARK_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build benchmark-build --target ark_network_buffer_benchmark --parallel 2
```

The executable is `benchmark-build/benchmarks/ark_network_buffer_benchmark`
(`ark_network_buffer_benchmark.exe` on Windows). Standalone Asio is also supported
by selecting `ASIO_STANDALONE=ON` with the corresponding dependencies.

## Run

```sh
# Hold 10,000 idle session objects and count receive-buffer allocation requests.
./benchmark-build/benchmarks/ark_network_buffer_benchmark --mode sessions --sessions 10000

# Parse individual messages split across 64-byte input blocks.
./benchmark-build/benchmarks/ark_network_buffer_benchmark --mode framing \
  --payload 1024 --chunk 64 --iterations 5000000 --warmup-iterations 10000

# Parse 16 consecutive messages per batch.
./benchmark-build/benchmarks/ark_network_buffer_benchmark --mode framing \
  --payload 4096 --batch 16 --chunk 65536 --iterations 250000

# Run all cases three times; each case uses a fresh process.
bash benchmarks/run_network_buffers.sh \
  ./benchmark-build/benchmarks/ark_network_buffer_benchmark 3 > results.jsonl
```

| Option | Meaning | Default |
| --- | --- | ---: |
| `--mode` | `sessions` for construction; `framing` for decoding | `framing` |
| `--sessions` | Simultaneously retained idle session objects | 10000 |
| `--payload` | Message body bytes, up to `ARK_MSG_MAX_LENGTH` | 1024 |
| `--chunk` | Input bytes supplied to the decoder per call | 64 |
| `--batch` | Consecutive messages per iteration | 1 |
| `--iterations` | Measured batches | 200000 |
| `--warmup-iterations` | Unmeasured framing batches; zero disables warmup | 10000 |

One JSON record is emitted per invocation. `operations` counts sessions or decoded
messages. `cpu_percent` is process CPU usage, with **100% representing one logical
core**. Array/object allocation fields count ordinary C++ allocation requests;
aligned allocations and queue-internal `malloc` calls are excluded. Framing-mode
byte totals are cumulative requests, not resident memory. The measured phase
includes message-content validation; frame preparation and warmup are excluded.

The program runs in one thread without sockets. It measures receive-buffer and
framing costs; connection capacity, network throughput and RTT require separate
end-to-end tests. [Local results and SVG charts](../docs/performance/receive-buffers.md).

## Regenerate the report

The optional offline tool uses Node.js to read existing records; it does not run network or performance tests:

```sh
node benchmarks/report_receive_buffers.mjs benchmarks/results/macmini-m4pro-20261015/receive-buffers
```

It checks the measured implementation and benchmark source SHA-256 values, then writes bilingual Markdown and SVGs. Building and running the C++ benchmark does not require Node.js.
