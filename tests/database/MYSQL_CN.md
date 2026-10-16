# MySQL 服务开发测试

[English](MYSQL.md) | [简体中文](MYSQL_CN.md)

该可选程序通过真实 MySQL 服务验证
`src/plugins/mysql/detail/mysql_service.hpp`。私有服务直接使用 arknet 原生连接池，
尚未修改公开 MySQL 模块接口、注册运行时模块或替换旧驱动。内部结果类型不属于已确定的插件 SDK。

## 构建

使用包含原生 MySQL connection、pool 和 `manual_runtime` 头文件的本地 arknet 仓库。
MySQL 认证与 TLS 需要 OpenSSL。开发测试开关默认关闭，普通应用和插件的构建选择保持不变。
配置时仍沿用 ark 原有的依赖查找规则，请使用项目对应的工具链或依赖路径。

```sh
cmake -S . -B build-database \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_PROTO=OFF \
  -DARK_BUILD_TESTS=OFF -DARK_BUILD_CORE_TESTS=OFF -DARK_BUILD_EXAMPLES=OFF \
  -DARK_BUILD_DATABASE_TESTS=ON -DARKNET_ENABLE_SSL=ON \
  -DARKNET_SOURCE_DIR=/path/to/arknet
cmake --build build-database --target ark_database_mysql_tests --parallel 1
```

默认使用 Boost.Asio。测试 Standalone Asio 时，使用独立构建目录并设置
`-DASIO_STANDALONE=ON`；需要指定头文件位置时，再设置
`-DARKNET_ASIO_INCLUDE_DIR=/path/to/asio/include`。

## 运行

准备独立测试数据库和账号，账号需要建表、查询、插入、删除数据和删表权限。
程序使用证书验证开启的 TLS，`HOST` 必须与服务端证书匹配。程序不会启动或停止服务端。
以下配置仅为示例：

```sh
export ARK_DATABASE_MYSQL_HOST=127.0.0.1
export ARK_DATABASE_MYSQL_PORT=3306
export ARK_DATABASE_MYSQL_USER=ark_test
export ARK_DATABASE_MYSQL_PASSWORD=test-password
export ARK_DATABASE_MYSQL_DATABASE=ark_tests
export ARK_DATABASE_MYSQL_CA=/path/to/test-ca.pem
./build-database/tests/database/ark_database_mysql_tests
```

六个变量均须提供，仅密码允许为空。配置缺失或无效时，程序明确报错退出，不会把跳过测试记为通过。
程序创建名称为 `ark_private_db_<clock>` 的独立表，成功后删表；失败时尝试对同一张表执行
`DROP TABLE IF EXISTS`，清理失败也会报错。程序不修改其他表或服务端设置。

## 验证范围

- 文本查询，以及同一独占连接上的 prepare → execute → close 操作链。
  不拼接参数、不跨连接借用缓存语句，也不自动重放写操作。
- 拥有型结果保留有符号和无符号整数、浮点数、二进制和 UTF-8 字符串、SQL NULL、
  日期时间、时间值和重复列名。后续命令不会使已保存的结果失效。
- 保留服务端错误码、SQLSTATE、预处理及参数错误和 `outcome_unknown`。
  语句清理结果单独保存，不会将已成功的执行改报为写入失败。
- 操作数与已保留输入容量的限制持续到拥有者线程派发结束。
  文本查询和预处理查询均透传原生结果限制。
- 一个总截止时间覆盖等待连接及后续命令。等待过期的写请求不发送；发送后超时保留结果不确定性。
  无法表示的时长在准入前拒绝。
- 业务回调仅由创建线程上的 `update()` 执行。
  测试覆盖业务回调内销毁服务，以及 `stop()` 释放捕获时由捕获析构同步销毁服务。
- 逻辑请求 ID 与原生连接池等待 ID、命令 ID 分离。
  测试用第二个连接确认唯一标记的服务端 sleep 已开始，再取消已发送请求。
  等待取消不会发送写请求；发送后取消保留 `outcome_unknown` 和独立的语句清理结果。
- 终态结果已排队后，取消不会改写结果；准入额度仍保留至拥有者线程完成派发。
  停止接收并取消请求后，已受理操作仍交付终态。

## 私有异步接口约定

| 方法 | 准入与完成语义 |
| --- | --- |
| `submit_query()` / `submit_execute()` | 保留 Boolean 接口：返回 `false` 表示未受理，不调用回调。 |
| `start_query()` / `start_execute()` | 返回逻辑请求 ID。拒绝时返回 0，仍交付明确的原生状态，回调可能同步执行；受理后的回调由 `update()` 派发。 |
| `cancel(id)` | 请求原生取消；返回 `true` 仅表示逻辑操作仍活跃，不保证 SQL 尚未执行。 |
| `is_pending(id)` | 观察逻辑终态。结果已排队但尚未派发、原生资源仍在清理时，也可能返回 `false`。 |

所有服务方法均由创建服务的模块线程调用。输入、结果和完成记录独立持有数据。
`diagnostics()` 最多保留取消投递与显式连接归还各一个异常；诊断不会替换已知执行结果，也不触发写入重试。

## 关闭边界

`stop()` 请求关闭原生连接池，并移除尚未开始执行的业务回调。
它**不是 IO 排空或线程退出屏障**。服务销毁后，原生处理函数和传输资源仍可能继续清理。
已开始的业务回调会正常执行至返回，已发送的命令也可能已经执行。

协程停机使用 `stop_accepting()`、`cancel_all()`，并继续由拥有者线程派发完成记录。
这两个方法保留已受理请求的终态交付；不能用旧 `stop()` 丢弃仍在等待的协程 continuation。
手动执行器的排空顺序见[协程原型](COROUTINES_CN.md)。
逻辑完成不代表传输资源清理或工作线程退出，原生连接池后续回收回调的异常保证也需单独处理。

在库的完成式关闭接口和 ark 停机顺序确定前，不能把该私有服务接入自动插件卸载。
事务、公开结果类型和旧同步接口均不在本开发测试范围内。
功能测试通过不代表生产容量或跨平台验收完成。
