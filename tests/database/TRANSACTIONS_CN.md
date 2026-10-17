# 私有 MySQL 事务 scope

[English](TRANSACTIONS.md)

这个开发模块提供固定连接租约的事务基础能力，不启用已发布的 MySQL 插件、不替换 MySQL++，
也不改变插件正常停止流程或增加公开数据库 API。原有 `mysql_service` 保持不变。

## 构建与运行

沿用[数据库开发构建](README_CN.md)，启用 SSL 并指定已有 arknet 本地源码目录。
依赖 revision 保持不变。

```sh
cmake --build database-build --target ark_database_mysql_transaction_state_tests \
  ark_database_mysql_transaction_tests
./database-build/tests/database/ark_database_mysql_transaction_state_tests
./database-build/tests/database/ark_database_mysql_transaction_tests
```

状态测试不需要数据库，启用 `ARKNET_ENABLE_SSL=ON` 时纳入 `check-database`。
真实服务测试使用[单请求测试](MYSQL_CN.md)相同的 `ARK_DATABASE_MYSQL_HOST`、`PORT`、
`USER`、`PASSWORD`、`DATABASE` 和 `CA` 环境变量。缺少变量或端口无效时明确报错。
故障代理测试要求 `ARK_DATABASE_MYSQL_HOST=127.0.0.1`，其他主机在连接前拒绝。
测试始终验证 TLS，并创建和删除唯一的 `ark_transaction_<数字>` InnoDB 表。请使用允许
建表、删表和查看测试账号运行语句的临时测试数据库，不要连接业务数据库。

真实服务测试还会为透明 TCP 故障代理绑定随机 IPv4 回环端口。代理不解密 TLS，在前序请求
完成后按测试要求丢弃服务端回复；不应纳入网络库或部署配置。
语句泄漏检查观察服务端全局 `Prepared_stmt_count`；请使用没有其他负载的临时 MySQL 实例，
避免其他客户端的预处理操作干扰计数。

## 所有权与执行

`mysql_transaction_service` 接收**已有的** `std::shared_ptr<arknet::mysql::pool>`，
以及显式的 `mysql_transaction_limits{max_transactions, max_input_bytes}`，不会创建
连接池或工作线程。事务准入使用独立且明确配置的配额，原生池继续执行连接数、等待者与结果上限。
首个模块尚未自动接入 `mysql_service::update()`。

```cpp
ark::detail::mysql_transaction_service transactions(existing_pool, {16, 65536});
transactions.start({std::chrono::seconds(5), std::chrono::seconds(1)},
    [](ark::detail::mysql_transaction_reply begun) {
        // 将 begun.transaction 移入模块拥有的状态，再通过该 scope 提交查询或预处理执行，
        // 最后显式提交或回滚。
    });
// 在模块线程的正常更新过程中驱动：
transactions.update();
```

`mysql_transaction` 只能移动。获取连接、BEGIN、每条查询或预处理执行及最终 COMMIT/ROLLBACK
始终使用同一个原生 lease；每个 scope 最多一个业务步骤在途，并行步骤会被拒绝，不会排队或
改变事务。等待获取连接、空闲持有、清理和未派发的完成记录及捕获释放期间，都占用 scope 配额。
SQL 字符串、参数向量和拥有型字符串参数均按容量计入输入额度，交付记录释放后归还。固定 BEGIN 和清理命令的存储在 BEGIN
准入前创建，清理无需再申请一个业务完成记录。

原生回调只将拥有型结果移入预留状态，由 `update()` 在构造服务的线程推进 scope 并派发业务
回调。被拒绝的请求返回 ID 0，可能内联回调。请求 ID 与原生 ID 分离且不复用，旧 ID 不能取消
后续步骤。每次更新对推进、派发和回收分别最多处理 `maximum` 条记录，状态检查采用轮询游标；
`update(0)` 不推进任何工作。

既有 `database_awaitable<mysql_transaction_reply>` 和 `module_task` 可以直接包装这些
回调，不需要新的协程 promise。正式消费者使用库提供的 manual runtime，更新顺序为：

```cpp
transactions.update();
bridge.dispatch();
tasks.poll();
```

## 结果、期限与清理

- `reply.result` 保留原生拥有型查询结果，`initiation_exception` 保留本地提交异常；
  `bool(reply)` 只检查这两项业务结果，不代表清理成功或事务已提交。仅在事务确实活跃时，BEGIN 回复才包含拥有型 `transaction`。
- `start_execute(sql, parameters, completion)` 在持有的 lease 上完成一次准备、执行和显式语句关闭。
  参数和结果拥有数据，支持嵌入 NUL 字节。`mysql_transaction_execute()` 提供与其他事务操作一致的
  惰性 Asio 协程工厂。
- `reply.statement_close` 和 `statement_close_exception` 独立保留语句清理诊断，不覆盖业务结果
  或自动回滚结果。MySQL 的 [COM_STMT_CLOSE](https://dev.mysql.com/doc/dev/mysql-server/8.4.10/page_protocol_com_stmt_close.html)
  没有服务端回复；成功表示原生关闭操作完成，不表示收到服务端确认或事务提交。
- `reply.rollback` 单独保存自动清理的原生结果；显式回滚则通过 `reply.result` 返回该操作的
  结果。两者都不会覆盖原始 SQL 错误或已经成功的写入结果。
- `transaction_status` 与 `snapshot()` 区分 `not_started`、`active`、`committed`、
  `rolled_back` 和 `unconfirmed`，保留清理、取消提交、lease 归还异常，回滚错误码和不确定
  标记，以及是否请求丢弃连接。查询成功不等于事务已经提交。
- 在获取 lease 前形成一个绝对期限，覆盖 BEGIN、所有业务步骤和 COMMIT，每个原生调用只获得
  剩余时间。在途调用使用原生超时，不再由 owner 并发提交到期取消。空闲到期和 handle 弃置
  由 owner 的下一次更新观察。准备、执行和语句关闭共用该业务期限；语句关闭到期或失败时保留
  业务结果，以未确认的事务结果丢弃连接。
- 自动回滚使用单独配置的 grace deadline，整次清理只使用一个期限。重复取消不会取消内部回滚或
  刷新清理时间。回复丢失或连接失效时，以不确定结果丢弃连接，不重放写操作或 COMMIT。
- 准备结果已由原生回调发布、owner 尚未处理时，取消仍会阻止执行。owner 关闭语句并尝试回滚，
  不会取消已完成的原生请求。已发送操作的不确定结果保持原样并丢弃连接，不再次执行、尝试语句
  关闭或虚构回滚结果。

正常方法和快照读取在模块线程执行。handle 析构或 `abort()` 只记录弃置请求，可以在其他线程
执行；owner 需要继续更新才能观察回滚，不能将析构当作已完成的异步 RAII 回滚。事务服务析构会
抑制未派发的回调，并请求丢弃其持有的 lease，不关闭调用方共享的连接池，也不代表 IO 或动态库
排空屏障。测试 owner 在工作结束后显式调用原生池的 `close_and_wait()`。

scope 显式发送 `COMMIT AND NO CHAIN NO RELEASE` 及对应回滚，避免服务端的
`completion_type` 意外开启下一笔事务或断开连接。语义见 MySQL [官方事务语法](https://dev.mysql.com/doc/refman/8.4/en/commit.html)。

原子性范围是 InnoDB 等事务引擎中的正常事务型 DML/查询。DDL、非事务表、通过查询接口自行
发送事务控制 SQL，以及内部提交的存储过程，不在该保证内。模块不自研 SQL 解析器；发现事务
意外结束时停止 scope 并返回不确定结果，不能撤回已经发生的隐式提交。参见[隐式提交](https://dev.mysql.com/doc/refman/8.4/en/implicit-commit.html)和 [InnoDB 错误处理](https://dev.mysql.com/doc/refman/8.4/en/innodb-error-handling.html)。任何已受理 SQL 错误都会
结束 scope 并尝试清理，不假定 MySQL 已回滚整笔事务。

## 验证范围

正式原生程序覆盖固定连接身份、提交前隔离、显式和自动回滚、完成模式、二进制拥有型结果、
准入上限、空闲与获取连接期限、已发送取消、旧 ID、弃置及抛异常的回调、跨线程移动与弃置，
以及模块线程协程所有权。
预处理执行还覆盖二进制和有类型参数、提交与回滚、准备和执行错误、参数数量错误、语句释放、
准备与执行之间的期限耗尽、已发送准备或执行的取消，以及原生结果已发布但 owner 尚未推进时的取消。
故障代理验证实际写入已经提交但客户端丢失 COMMIT 回复的情况，并单独验证清理回复丢失。
这些是功能和生命周期检查，不代表吞吐量或部署容量测试。

接入原有服务入口、公开插件配置、savepoint 和正常应用停止流程，
继续作为独立模块推进。本机测试不代表 Windows/Linux、任意分配失败或公开插件验收已经完成。
