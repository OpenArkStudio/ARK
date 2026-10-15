# 私有数据库回调与协程基准测试

[English](DATABASE.md)

这个可选 C++ 程序比较私有 `redis_service` / `mysql_service` 的回调调用方式，以及 `database_awaitable`、Asio `awaitable`、`manual_runtime` 和 `module_task_group` 的协程调用方式。程序不启用数据库插件，也不修改公开接口或模块停止策略。

## 构建

按[数据库开发依赖说明](../tests/database/README_CN.md)准备包含原生连接池和 `manual_runtime` 的本地 arknet 源码。目标默认关闭，与 `ARK_BUILD_BENCHMARKS` 独立。

```sh
cmake -S . -B database-bench-build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_PROTO=OFF \
  -DARK_BUILD_TESTS=OFF -DARK_BUILD_CORE_TESTS=OFF \
  -DARK_BUILD_DATABASE_BENCHMARKS=ON -DARKNET_ENABLE_SSL=ON \
  -DARKNET_SOURCE_DIR=/absolute/path/to/arknet
cmake --build database-bench-build --target ark_database_benchmark
cmake --build database-bench-build --target check-database-benchmark
```

本地自检不需要数据库。`ASIO_STANDALONE=ON` 选择 Standalone Asio，默认使用 Boost.Asio。MySQL 要求 `ARKNET_ENABLE_SSL=ON`；关闭 TLS 的构建仅支持 Redis。可直接运行 C++ 可执行文件，无需 Python、Ruby 或启动脚本。程序不会启动或停止数据库服务。

## 连接配置与功能检查

仅通过现有数据库测试环境变量提供地址和凭据。程序不接受凭据参数，也不打印连接配置。

| 协议 | 必需环境变量 | 可选环境变量 |
| --- | --- | --- |
| Redis | `ARK_DATABASE_REDIS_HOST`、`ARK_DATABASE_REDIS_PORT` | `ARK_DATABASE_REDIS_USER`、`ARK_DATABASE_REDIS_PASSWORD` |
| MySQL | `ARK_DATABASE_MYSQL_HOST`、`ARK_DATABASE_MYSQL_PORT`、`ARK_DATABASE_MYSQL_USER`、`ARK_DATABASE_MYSQL_PASSWORD`、`ARK_DATABASE_MYSQL_DATABASE`、`ARK_DATABASE_MYSQL_CA` | 无 |

MySQL 密码变量必须存在，但可为空。MySQL 使用指定 CA 验证 TLS 及主机证书身份，不提供跳过验证的参数。Redis 使用私有 Redis 服务的普通 TCP 连接。应显式准备测试服务并授予只读数据库权限。工作负载仅发送 Redis `ECHO` 或 MySQL `SELECT X'...'`，不创建键、表或事务。连接池仍按原生服务的正常行为执行认证、连接重置和重新初始化。

提供环境变量后，先运行少量操作验证功能，再安排性能测量：

```sh
for protocol in redis mysql; do
  for mode in callback coroutine; do
    ./database-bench-build/benchmarks/ark_database_benchmark \
      --protocol "$protocol" --mode "$mode" --concurrency 1 --window 1 \
      --payload 513 --warmup-operations 2 --operations 4 --update-us 1000
  done
done
```

每个成功进程输出一条 JSON 记录，`valid` 为 `true`，错误计数为零，退出码为 0。原生错误、局部异常、准入拒绝、结果不符或阶段超时都会使结果无效，退出码为 1。程序会取消已接受的操作，并持续驱动真实终态完成。若排空本身失败，或超过操作超时加 10 秒的期限，程序报告失败并以退出码 2 结束，不析构仍挂起的协程帧。启动或环境配置失败时，输出错误信息并以退出码 1 结束。程序不重试请求，不分离任务，也不合成成功结果。

## 对等工作负载与边界

分别启动回调和协程进程，使用相同参数及服务配置。每个窗口槽位最多持有一个已接受的逻辑操作。回调方式在 `service.update()` 返回、完成记录释放配额后提交下一请求；协程方式通过桥接层在所有者轮询的手动执行器上恢复。两者共用载荷构建、原生操作提交、每次操作的时间记录、结果校验和报告逻辑。基准程序不增加额外请求队列；原生连接池等待上限、服务配额、桥接配额和任务上限均受窗口大小约束。

`--concurrency` 表示原生连接池的连接数上限，不表示应用工作线程数。程序只有一个所有者线程，数据库连接池使用正常的原生 IO 线程。`--window` 表示未完成逻辑请求的上限，包含等待连接租约或所有者交付的请求。Redis 每次执行一条 ECHO，MySQL 每次执行一条文本 SELECT，不测量预处理语句或协议流水线。

载荷采用固定字节序列 `00 01 ... ff 00 ...`。程序逐次比较原生自持有结果与完整二进制字符串，包含嵌入的 NUL 字节。每个槽位保留上一结果，并在下一操作后再次校验，覆盖跨原生操作的所有权语义。延迟向量按指定操作数在预热前预分配；两种方式均不为单个延迟样本分配记录，并使用相同的时钟调用。

| 参数 | 默认值 | 接受范围及含义 |
| --- | --- | --- |
| `--protocol` | `redis` | `redis` 或 `mysql` |
| `--mode` | `callback` | `callback` 或 `coroutine` |
| `--concurrency` | 4 | 1–64 个原生池连接 |
| `--window` | 16 | 1–256 个逻辑操作 |
| `--payload` | 64 | 每次操作返回 1–65536 字节二进制数据 |
| `--warmup-operations` | 1000 | 1–100000 次不计入测量的已完成预热操作 |
| `--operations` | 10000 | 1–1000000 次测量操作 |
| `--update-us` | 1000 | 所有者更新间隔，0–1000000 微秒；0 表示连续轮询 |
| `--update-budget` | 64 | 每次更新中，服务交付、桥接交付和手动执行器各自最多处理 1–4096 项 |
| `--timeout-ms` | 3000 | 原生获取连接和执行共用的 1–30000 毫秒超时 |
| `--deadline-seconds` | 60 | 每阶段 1–300 秒期限；超时后取消并排空未完成操作 |

要求 `concurrency <= window <= operations`，且 `window <= warmup-operations`。更新休眠不追赶错过的周期，处理耗时可能使实际更新间隔更长。`--help` 列出支持的参数。每次只改变一个维度，保存完整参数、源码版本、编译器及编译选项、数据库版本及配置，以及原始 JSON。闭合循环负载不能直接证明饱和容量。

## 测量范围

连接池、服务和运行时构建，以及预热请求均在测量区间之外。预热结束时，所有逻辑原生结果均已交付，桥接记录和任务记录均已释放。私有服务 API 不暴露后台租约重置的完成状态；第一次测量获取连接时可能等待最后一次预热的重置，与后续获取连接等待前一操作重置的行为一致。本程序不声明 IO 静止屏障或插件卸载保证。

测量从第一次测量提交前开始，在全部测量结果和任务完成后结束，包含获取连接及重置等待、网络与数据库耗时、所有者更新间隔、命令及结果所有权处理、内容校验和下一请求提交。协程任务的启动、完成及桥接开销也计入。延迟单独从所有者提交计至所有者收到结果、开始内容校验之前；两种方式一致。P50 和 P99 对所有已交付测量请求采用最近秩百分位数；失败运行的指标均无效。

进程 CPU 占用统计全部客户端线程，100% 表示一个逻辑核心。即使数据库与客户端位于同一机器，也不包含外部数据库进程的 CPU。峰值 RSS 是整个进程生命周期的常驻内存高水位，包含启动、预热和预分配延迟存储，不表示测量区间独立内存、内存分配流量或数据库内存。JSON 记录中写明两种范围。吞吐量等于成功逻辑操作数除以测量墙钟时间。错误计数分别覆盖原生结果、C++ 异常、内容或所有权错误、准入拒绝、任务失败和阶段超时；一个失败请求可能计入多个类别。

正式对比应在安静机器上固定服务位置及配置，多次启动独立进程，并交替运行回调和协程方式。Sanitizer 和少量操作的功能检查仅用于验证正确性，不应作为性能测量结果发布。这个开发目标不附带性能结论。
