# 私有数据库接入测试

[English](README.md)

这些开发测试验证内部原生数据库服务，不替换已发布的 `mysql_module`、`redis_module`
接口，不为运行时插件切换客户端，不移除旧驱动，也不改变插件注册。
`ARK_BUILD_DATABASE_TESTS` 默认关闭。

## 构建

按[网络构建说明](../../docs/networking_CN.md)初始化既有依赖。
显式指定包含原生 Redis、MySQL 客户端的本地 arknet 仓库；开发测试不会修改生产依赖版本，
也不会下载功能分支。

```sh
cmake -S . -B database-build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_PROTO=OFF \
  -DARK_BUILD_TESTS=OFF -DARK_BUILD_CORE_TESTS=OFF \
  -DARK_BUILD_DATABASE_TESTS=ON -DARKNET_SOURCE_DIR=/absolute/path/to/arknet \
  -DASIO_STANDALONE=OFF -DARKNET_ENABLE_SSL=ON
cmake --build database-build --target ark_database_queue_tests \
  ark_database_redis_stop_tests ark_database_redis_tests ark_database_mysql_tests --parallel 2
```

使用 Standalone Asio 时设为 `ASIO_STANDALONE=ON`，必要时指定
`ARKNET_ASIO_INCLUDE_DIR`。仅在 `ARKNET_ENABLE_SSL=ON` 时构建 MySQL 测试；
即使显式使用明文 MySQL 连接，认证仍需要 OpenSSL。Redis 明文连接测试不要求此选项。
队列测试与 core/network 一样复用既有 `tests/network/doctest.h`，
真实服务检查是独立 C++ 可执行程序，不下载新测试框架。

## 运行本地回归

```sh
cmake --build database-build --target check-database
# 等价的 shell 入口；也可以直接运行可执行文件。
bash scripts/test_databases.sh database-build local
./database-build/tests/database/ark_database_queue_tests --list-test-cases
```

队列用例覆盖操作数和拥有输入的配额、已完成未派发的操作、并发完成生产者、拥有线程派发、
回调异常、重入，以及停止时释放捕获。Redis 停止回归验证捕获对象析构时销毁服务的场景；
不需要运行中的数据库，仅使用 IPv4 回环端口 1。
状态回归还通过明确构造的输入验证获取失败的错误与回复映射，不代表复现了原生连接池故障。

`check-database` 不运行真实服务测试。缺少端点配置会明确报错，不会跳过后当作成功。

## Redis 服务检查

准备隔离的 Redis 兼容服务。测试创建唯一的 `ark-private-service-...` key，验证二进制
SET/GET 后删除该 key，并检查结果所有权、模块线程派发、配额恢复、停止后的回调抑制，
以及从业务回调中销毁服务。

例如，在另一终端启动临时本地服务：

```sh
redis-server --bind 127.0.0.1 --port 26791 --protected-mode yes \
  --save '' --appendonly no
```

然后运行：

```sh
export ARK_DATABASE_REDIS_HOST=127.0.0.1
export ARK_DATABASE_REDIS_PORT=26791
# 按需设置 ARK_DATABASE_REDIS_USER / ARK_DATABASE_REDIS_PASSWORD。
bash scripts/test_databases.sh database-build redis
# 或：./database-build/tests/database/ark_database_redis_tests
```

测试结束后停止临时服务。运行脚本不会启动或停止服务、安装工具，也不会为真实服务测试选择端点。
有写入的测试不要连接业务数据。MySQL 的配置与测试内容见 [MYSQL_CN.md](MYSQL_CN.md)。
`all` 会依次运行本地、Redis 和已构建的 MySQL 程序，需要配置两种真实服务。

## 内部约定与生命周期

网络 IO 由库拥有。原生回调只把拥有型结果移动到模块预留的完成记录，业务回调由 `update()`
在构造服务的线程上派发。提交、更新、停止与销毁服务均需在该线程执行。等池期间也计入输入容器和
字符串容量；业务回调执行及其捕获释放后才归还额度。原生结果容量限制继续生效，任意用户回调捕获
不属于输入字节额度。

提交返回 false 表示未受理且无回调。已受理操作只完成一次，除非 `stop()` 移除尚未开始的业务回调。
同一个经溢出检查的截止时间覆盖等池和执行。保留原生服务端、传输错误、拥有型结果及写入结果不确定
状态，不重放写操作。公开接口将直接使用 arknet 的拥有型结果，以异步和协程调用为主。
协程调度、取消、路由和兼容接口仍独立于这些内部测试。

`stop()` 抑制业务回调并请求关闭连接池，**不等待**原生回调、传输对象、TLS 资源和 IO 线程销毁。
捕获析构重入的修复通过局部变量持有队列，先请求原生池关闭再释放捕获，不新增 IO 排空屏障。
这些辅助服务尚未注册到运行时插件，也没有通过动态库卸载验收。
