# 数据库协程原型

[English](COROUTINES.md)

这个可选开发原型复用 Asio 的 `awaitable`、`async_initiate` 和 `co_spawn`，
不新增协程 promise 实现，也不修改已发布的数据库模块接口。
任务组通过公开的 `<ark/task.hpp>` 提供，原私有名称保留为同一实现的别名；数据库操作函数和协程桥仍保留在 `src/plugins/` 下的私有目录中。

## 执行与所有权

`module_task<T>` 是 Asio `awaitable<T>` 的别名。
`module_task_group` 持有 `start()` 受理的任务，直到最终完成。
返回的任务 ID 用于观察和取消；丢弃 ID 不会销毁挂起的协程。
尚未启动的惰性 awaitable 被销毁时，不会发起原生请求。

共享的 arknet `manual_runtime`、任务组、协程桥和服务都在模块线程构造。
网络库持有执行上下文，这个手动运行时不创建 IO 执行线程；原生数据库连接池
继续使用网络库自己的 IO 线程。任务组操作、结果批次投递和任务完成回调都在
模块线程执行。Asio 的平台辅助机制不改变这个由调用者 `poll()` 执行业务回调的约定。

每次模块更新依次执行：

```cpp
service.update();  // 将原生终态结果移入已预留的协程桥记录。
bridge.dispatch(); // 向手动执行器投递一个有界结果批次。
tasks.poll();      // 执行就绪的 Asio handler 和协程 continuation。
```

协程桥要求任务使用同一个手动运行时的执行器。
使用其他执行器时，会在提交原生请求前拒绝。
仅在模块线程调用服务回调，并不等于已经进入这个执行器的执行上下文。

## 私有操作函数

`mysql/detail/mysql_awaitable.hpp` 提供 `mysql_query(service, bridge, sql, timeout)` 和 `mysql_execute(service, bridge, sql, parameters, timeout)`，均返回 `module_task<mysql_service_reply>`，保留原生查询结果、可选的语句关闭结果和独立的语句关闭异常。与现有服务一致，MySQL 需要启用 SSL 的开发构建。

`redis/detail/redis_awaitable.hpp` 提供 `redis_command(service, bridge, command, timeout)` 和 `redis_pipeline(service, bridge, commands, timeout)`，均返回 `module_task<arknet::redis::result>`，支持现有明文和 TLS 服务。TLS 需要启用 SSL。流水线结果顺序、二进制数据、服务端错误值和 `command_may_have_run` 字段均保持不变。

这些私有函数模板复用现有服务回调契约，不新增运行时多态接口，也不是已发布的插件 API。SQL、参数列表和命令按值传入，并在函数调用时移入惰性等待对象。因此，即使调用者在任务启动前修改或销毁原始输入，操作仍持有自己的数据。销毁尚未启动的任务不会提交请求。服务和协程桥仍通过引用使用，必须存活到返回的任务完成或被丢弃且尚未启动；操作函数不持有它们，也不在析构时排空请求。

```cpp
auto operation = ark::detail::redis_command(service, bridge, {"ECHO", payload}, 3s);
auto result = co_await std::move(operation);
```

调用者显式传入超时，从惰性操作开始提交服务请求时计时，继续使用服务已有的租约获取与命令执行总时限，不为各阶段重新分配时限。取消只转交非零的已受理服务请求 ID，并等待原生终态。ID 为零的同步拒绝也通过同一协程桥交付，没有可取消的请求。原生错误和写入结果不确定标记保持不变，不自动重试；本地 C++ 异常通过已有协程桥重新抛出。操作函数不新增执行器、工作线程或正常停止策略。

`ark_database_operations_tests` 无需数据库服务。受控服务回调验证输入所有权和结果转交，真实明文和 TLS 服务实例验证无连接的本地无效时限拒绝。这些检查不能代替另行配置的真实服务测试；`ark_database_service_awaitable_tests` 已改为使用同一组操作函数。

## 私有事务函数

`mysql/detail/mysql_transaction_awaitable.hpp` 提供四个惰性操作工厂，需要启用 SSL 的开发构建：

| 函数 | 输入 |
| --- | --- |
| `mysql_transaction_begin(service, bridge, options)` | 按值持有事务总超时与独立清理时限 |
| `mysql_transaction_query(transaction, bridge, sql)` | 按值持有 SQL，沿用现有事务截止时间 |
| `mysql_transaction_commit(transaction, bridge)` | 使用同一事务句柄及其剩余时限 |
| `mysql_transaction_rollback(transaction, bridge)` | 通过同一事务句柄显式回滚 |

这些普通函数在调用时持有输入，通过已有协程桥返回 `module_task<mysql_transaction_reply>`，直到任务被推进才提交原生请求。丢弃尚未启动的任务不会提交请求。被引用的服务、协程桥和事务句柄必须存活且保持原位置，直到任务完成或被丢弃且尚未启动；创建惰性步骤后再移动句柄，会改变该步骤实际引用的目标。成功 BEGIN 后，先从回复中移出句柄，再创建后续步骤。只有 BEGIN 接收选项，后续步骤不重置事务总截止时间或独立清理预算。

回复整体移动转交，保留仅可移动的拥有型句柄、查询结果、可选回滚结果和事务快照。回复中的 `initiation_exception` 仍与清理、取消和租约归还诊断一起保留在回复中，不会因重新抛出而丢弃其他字段。受理前发起请求时直接抛出的 C++ 异常，仍走协程桥的异常路径。结果不确定的状态保持不变，不自动重试，也不伪造提交成功。取消只转交已受理的逻辑请求 ID，并等待实际回调；ID 为零的同步拒绝也由手动执行器恢复任务。

`ark_database_mysql_transaction_awaitable_tests` 无需数据库。受控回调验证输入所有权、结果转交、取消与额度为一的连续步骤；真实服务和空句柄实例仅验证本地拒绝，不能证明真实租约或数据库行为。另行配置的 `ark_database_mysql_transaction_tests` 使用同一组函数执行 BEGIN/query/COMMIT，以及显式 BEGIN/ROLLBACK。真实服务检查按[事务说明](TRANSACTIONS_CN.md)运行，测试不会启动数据库。

## 结果、容量与取消

`database_awaitable<Result>` 为每个受理的操作预留一条记录，完成后等待交付的
结果仍占用记录。记录数可以配置。批次调用标准 Asio 完成 handler 前先归还
记录额度；这时源服务回调已返回，也已归还自身额度。因此两者上限均为一时，
仍可依次执行 `co_await A; co_await B`。原生客户端的结果大小限制继续生效；
任意协程局部变量不计入输入字节额度。

完成签名为 `void(std::exception_ptr, Result)`。网络、服务端、取消和超时错误
保留 arknet 原生结果字段，包括写入结果不确定的标记。MySQL 私有结果封装将
语句清理结果与执行结果分开保存。C++ 异常仍按异常传播，不会转换为成功结果。
协程桥已满时抛出 `std::length_error`，关闭后抛出 `std::logic_error`；
两者都不会提交原生请求，也不会增加隐藏等待队列。
原生服务同步返回的拒绝结果仍占用已预留记录，并由执行器批次异步交付。

如果模块线程投递结果批次时抛出异常，已就绪记录仍被持有并占用额度，之后可
重试 `dispatch()`。重复或迟到的原生完成不能二次交付任务。
原生请求受理前发生记录分配失败时，会在已进入的手动执行器中通过 Asio 传播异常。

取消使用服务的逻辑请求 ID；在原生 ID 发布前收到的取消请求也会补发。
取消不会伪造终态、覆盖已就绪结果或重放命令。已受理操作继续占用记录，直到
服务返回终态响应。服务使用同一个截止时间覆盖连接获取和后续命令阶段。
如果提交原生取消请求时抛出异常，`cancellation_exception()` 保留第一次本地异常；
操作继续等待真实终态，该诊断不会替换原生成功或写入结果不确定的响应。

## 停止任务准入

在构造时的模块线程调用 `tasks.stop_accepting()` 后，后续 `start()` 返回零。
重复调用不产生额外影响；即使准入已经关闭，其他线程调用仍抛出 `std::logic_error`。
它不会取消或推进已受理任务；这些任务仍可继续执行多次 `co_await`，并在模块线程完成。

已受理任务还可能提交后续操作时，应继续允许服务和协程桥受理请求；它们各自的
`stop_accepting()` 会拒绝这些新请求。任务准入关闭且没有剩余任务时，
`tasks.closed()` 才返回真。之后调用 `tasks.close()` 仍会请求取消剩余任务，
并需要显式推进，直到任务完成。

这项私有能力不代表已经选定进程停服或插件卸载策略，不增加截止时间，也不改变
下面的析构与排空要求。

## 显式关闭

在模块线程发起关闭：

```cpp
service.stop_accepting();
bridge.stop_accepting();
tasks.close(); // 停止任务准入，并请求取消已受理任务。
```

继续执行正常更新顺序，直到 `tasks.closed()` 且 `bridge.pending() == 0`，
再销毁任务组、协程桥、服务及任务引用的业务对象。最后一个任务的完成回调会先
移除任务组成员记录，再执行用户完成逻辑；因此最后的回调可以销毁已清空的
任务组对象。正在执行的 `poll` 和结果批次仍持有运行时状态，直到本次调用返回。

任务组和协程桥只能在构造线程、无未完成操作时析构。违反条件会调用
`std::terminate`；析构不会递归 `poll`、阻塞等待、分离任务或直接销毁挂起的
Asio 协程帧。忽略取消的任务仍需等待真实终态。原型尚未承诺自动模块析构、
插件卸载或任意放弃挂起任务时的安全性。现有服务的 `stop()` 仍会抑制回调，
不是排空屏障；协程关闭完成前调用它会破坏上述顺序。

## 编译与运行

按照[开发测试构建说明](README_CN.md)，显式指定包含
`arknet/base/manual_runtime.hpp` 的本地 arknet 源码目录。
本地用例不需要数据库：

```sh
cmake --build database-build --target ark_database_task_tests \
  ark_database_task_contract_tests ark_database_queue_tests
./database-build/tests/database/ark_database_task_tests
cmake -DPROGRAM="$PWD/database-build/tests/database/ark_database_task_contract_tests" \
  -P tests/database/task_group_contract.cmake
```

契约测试通过明确的子进程模式安装受控终止 handler：仍有未完成任务或在错误线程
析构时返回 77，CMake 校验退出码，不产生崩溃转储。本地用例覆盖模块执行器恢复、
ID 发布前取消、结果就绪后取消、重复完成、投递失败后重试及额度为一的串行操作。

编译独立的真实服务程序，并设置与 [Redis](README_CN.md) 和
[MySQL](MYSQL_CN.md) 测试相同的必填环境变量。程序不启动、停止或选择数据库服务器：

```sh
cmake --build database-build --target ark_database_service_awaitable_tests
./database-build/tests/database/ark_database_service_awaitable_tests redis
./database-build/tests/database/ark_database_service_awaitable_tests mysql
# 等价的 shell 入口：
bash scripts/test_databases.sh database-build redis-awaitable
bash scripts/test_databases.sh database-build mysql-awaitable
```

Redis 验证二进制 ECHO/PING 结果所有权、原生拒绝和阻塞读取的取消。
MySQL 验证文本查询、预处理查询的有类型结果、独立语句清理结果、原生拒绝以及
`SELECT SLEEP` 的取消。两者均使用额度一，检查执行器恢复和显式关闭排空。
MySQL 需要 `ARKNET_ENABLE_SSL=ON` 及与服务器名称匹配的可信证书。
这些是本地功能检查，不代表生产容量或跨平台验收结论。

公开的 `ark::task<T>` 和 `ark::task_group` 用法见[协程任务](../../docs/guides/tasks_CN.md)。这里的 `module_task<T>` 和 `module_task_group` 是同一实现的私有兼容别名，不会建立第二套调度器。
