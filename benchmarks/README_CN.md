# ARK 性能基准

`network_buffers.cpp` 测量会话对象创建和应用层消息拆包成本，是可独立运行的
C++20 程序。程序校验每条消息的内容，无须 Python 或 Ruby。Bash 脚本用于批量重复测量。

## 构建

使用 ARK 固定的依赖版本，以 Release 模式构建：

```sh
git submodule update --init 3rdparty/args 3rdparty/spdlog 3rdparty/readerwriterqueue
cmake -S . -B benchmark-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES=boost -DASIO_STANDALONE=OFF \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_TESTS=OFF -DARK_BUILD_EXAMPLES=OFF \
  -DARK_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build benchmark-build --target ark_network_buffer_benchmark --parallel 2
```

可执行文件位于 `benchmark-build/benchmarks/ark_network_buffer_benchmark`；
Windows 下名称为 `ark_network_buffer_benchmark.exe`。
选择 `ASIO_STANDALONE=ON` 并安装对应依赖，也可使用 Standalone Asio 构建。

## 运行

```sh
# 同时保留 10000 个空闲会话对象，统计收包缓冲的分配请求。
./benchmark-build/benchmarks/ark_network_buffer_benchmark --mode sessions --sessions 10000

# 每条消息按 64 字节的数据块分次传给拆包器。
./benchmark-build/benchmarks/ark_network_buffer_benchmark --mode framing \
  --payload 1024 --chunk 64 --iterations 5000000 --warmup-iterations 10000

# 每批连续输入 16 条消息。
./benchmark-build/benchmarks/ark_network_buffer_benchmark --mode framing \
  --payload 4096 --batch 16 --chunk 65536 --iterations 250000

# 每组独立启动进程，全部配置重复三次。
bash benchmarks/run_network_buffers.sh \
  ./benchmark-build/benchmarks/ark_network_buffer_benchmark 3 > results.jsonl
```

| 参数 | 含义 | 默认值 |
| --- | --- | ---: |
| `--mode` | `sessions` 测创建；`framing` 测拆包 | `framing` |
| `--sessions` | 同时保留的空闲会话对象数 | 10000 |
| `--payload` | 消息体字节数，上限为 `ARK_MSG_MAX_LENGTH` | 1024 |
| `--chunk` | 每次传给拆包器的数据块字节数 | 64 |
| `--batch` | 每次迭代的连续消息数 | 1 |
| `--iterations` | 计入测量的批次数 | 200000 |
| `--warmup-iterations` | 不计入测量的预热批次数；零表示关闭预热 | 10000 |

每次运行输出一条 JSON。`operations` 表示创建的会话数或解析的消息数。
`cpu_percent` 表示进程 CPU 占用，**100% 代表占满一个逻辑核心**。
数组和对象的分配字段统计普通 C++ 分配请求，排除对齐分配和队列内部的 `malloc`。
拆包模式下的字节数是累计申请量。计时包含消息内容校验，排除测试数据准备和预热。

程序采用单线程，在进程内测量收包缓冲与拆包成本。连接容量、网络吞吐量和 RTT
需另做端到端测试。[本机结果与 SVG 图表](../docs/performance/receive-buffers_CN.md)。

## 更新报告

可选的离线报告工具使用 Node.js，只读取已经生成的结果，不启动网络或性能测试：

```sh
node benchmarks/report_receive_buffers.mjs benchmarks/results/macmini-m4pro-20261015/receive-buffers
```

程序核对实测源码和基准源码的 SHA-256，再生成中英文 Markdown 和 SVG。C++ 基准的构建、运行不需要 Node.js。
