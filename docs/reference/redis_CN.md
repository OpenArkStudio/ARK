# Redis 协程插件

`redis_module` 使用 arknet 原生 Redis 连接池，替换原有 bredis 同步接口和回调接口。
迁移时，将 `connect`、`set`、`get` 以及命令专用包装替换为命名连接配置和
`co_await command` / `pipeline`。插件不提供阻塞兼容层，应用需要重新编译。

```cpp
// 消费模块已注册，在 init() 中配置：
auto* redis = find_module<ark::redis_module>();
auto& tasks = get_plugin_manager()->create_task_group(*this);
ark::redis_options options;
options.pool.connection.host = "127.0.0.1";
options.pool.connection.port = "6379";
options.pool.max_connections = 8;
options.max_operations = 64;
options.max_input_bytes = 1024 * 1024;
redis->configure("cache", std::move(options));

// 成员协程，通过 tasks.start(workflow()) 启动：
ark::task<> workflow() {
    auto result = co_await redis_->command("cache", {"GET", "key"});
    if (!result) {
        // 检查 result.error、transport_error 和 command_may_have_run。
        co_return;
    }
    // result.replies 拥有字符串、数组和二进制数据。
}
```

配置持有字符串、凭据、连接池、协议、响应限额和 TLS 设置。连接名重复或入口已封闭时，
`configure` 返回 false；空名称和无效限额抛出异常。消费模块可以在 Redis 模块自身的
`init` 之前配置，已安装的管理器会提供任务运行时。插件不再读取旧版
`redis.plugin.conf` 文件。配置仅限所属线程使用，不支持动态替换。启用
`ARKNET_ENABLE_SSL` 后，可设置 `options.tls = true`，并通过 `options.pool`
配置证书验证；未编译 SSL 时请求 TLS 会抛出异常。

命令工厂按值接收输入，在惰性任务启动前保存数据。返回任务还会持有连接状态，因此尚未
启动的任务在模块外观对象销毁后仍有有效状态。操作必须运行在管理器的同一个任务执行器
上；外部执行器会在提交原生请求前被拒绝。未知连接名返回原生 `not_connected`，入口
封闭后返回 `closed`。桥接层准入失败抛出异常，无效截止时间返回原生
`invalid_command`。原生传输和数据库错误保留为 `arknet::redis::result`；Redis
错误回复仍是有序 `replies` 向量中的值。

`pipeline(name, commands, timeout)` 返回包含有序回复的单个原生结果。超时覆盖连接池
获取和命令执行。`max_operations` 限制桥接记录和服务中等待所属线程交付的操作数量；
`max_input_bytes` 限制服务保留的命令存储，计入已分配容量。输入限额拒绝返回原生
`queue_full`。连接池、等待队列和响应限额分别通过 `options.pool` 配置。这些限额按
命名连接生效，不代表整个进程的内存上限。

管理器托管任务组的取消请求会转交原生服务。等待连接期间取消的请求确定尚未发送；发送
后取消可能返回 `command_may_have_run == true`。插件不会重放写操作，也不会合成成功
结果。调用者持有的原生结果及部分回复在连接池退出后仍有效。

管理器更新期间，Redis 插件先执行 `service.update()`，再调度桥接交付。管理器统一轮询
共享任务运行时，arknet 负责原生 IO 和工作线程。停止期间，应用仍需持续更新管理器：
`stop()` 可以在已接收工作流结束前返回 false，同一工作流的后续命令仍可提交。就绪检查
只观察逻辑活动，不会提前封闭服务。所有托管任务和交付调用栈返回后，`pre_shut` 封闭
入口，并在宿主生命周期调用帧中执行原生 `retire_native()`。返回 false 或抛出异常时
保留资源，`shut` 不会释放未退出的服务。`service.stop()` 不是原生退出屏障。运行中的
任务递归调用生命周期函数不构成受支持的销毁路径，管理器会将停机工作延后到宿主调用帧。

`tests/database/public_redis_module.cpp` 使用有界本机 RESP 协议端点验证公开接口：输入
与结果所有权、流水线顺序、外部执行器拒绝、限额、取消、TLS 证书验证和管理器优雅
停止。该端点不是 Redis 实现。底层服务的私有 TLS 回归还覆盖其他失败场景。以上本地
检查不代表真实 Redis
部署验收、生产容量验证或一般性的无泄漏保证。

`ark_database_public_redis_plugin_tests` 另行编译并加载实际插件动态库，验证宏注册、协程结果投递和宿主停机。该检查不代表操作系统动态库卸载、热更新或真实 Redis 服务验收。
