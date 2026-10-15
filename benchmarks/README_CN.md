# 网络缓冲基准测试

[English](README.md)

直接编译并运行 C++ 程序，测量 ark 的消息拆包缓冲。程序不创建网络连接，也不用于测量网络容量。

```sh
cmake -S . -B bench-build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_BENCHMARKS=ON
cmake --build bench-build --target ark_network_buffer_benchmark
./bench-build/benchmarks/ark_network_buffer_benchmark \
  --mode framing --payload 1024 --chunk 64 --batch 16 --iterations 1000000
```

依赖初始化见[网络构建](../docs/networking_CN.md)。

| 参数 | 含义 |
| --- | --- |
| `--mode sessions --sessions 10000` | 创建并保留会话，统计普通 C++ 内存申请 |
| `--mode framing` | 拆分消息，逐条检查消息 ID、内容和数量 |
| `--payload`、`--chunk` | 消息大小和输入分片大小，单位为字节 |
| `--batch` | 每批输入的消息数 |
| `--warmup-iterations` | 不计入测量的预热批次数，默认 10000 |
| `--initial-buffer` | 初始容量，单位为字节；省略时使用库的默认值 |

JSON 输出包含吞吐量、进程 CPU 占用比例及 C++ 内存申请次数和字节数。**100% CPU 表示一个逻辑核心。** 申请字节数不等于常驻内存，不包含对齐分配、队列内部的 `malloc` 和套接字缓冲。

可用 shell 脚本重复运行标准用例；C++ 程序本身可独立执行：

```sh
bash benchmarks/run_network_buffers.sh ./bench-build/benchmarks/ark_network_buffer_benchmark 3
```

`report_receive_buffers.mjs` 生成中英文 Markdown 和 SVG 图表，生成前检查测量数量与源文件哈希。仓库只保留正式报告使用的数据，其他本地输出放在源码目录之外。

单独启用的[私有数据库基准测试](DATABASE_CN.md)使用不写入数据的 Redis/MySQL 操作，比较原生服务回调与 Asio 协程桥接方式。
