# 核心回归测试

[English](README.md)

doctest 程序验证字符串增长与比较、哈希键、对象所有权、常量迭代器、类型转换、
模块类型查询和成员函数回调。依赖初始化见[网络构建](../../docs/networking_CN.md)。

```sh
cmake -S . -B core-build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_TESTS=OFF -DARK_BUILD_CORE_TESTS=ON
cmake --build core-build --target check-core
```

运行 `core-build/tests/core/ark_core_tests --help` 查看 doctest 的筛选参数。
启用 `ARK_BUILD_CORE_TESTS=ON` 时，`check-network` 也会运行这些用例，现有 CI
矩阵会执行该入口。测试不需要数据库服务。

启用 `ARK_BUILD_PROTO=ON` 时，还验证 Protobuf 解析失败、Actor ID、二进制消息、
旧版报文字节和对外 JSON 字段名，以及新的 `ark::protobuf` 描述符和枚举名称；
需要按应用构建说明初始化 Protobuf 依赖。

`check-kernel-initialization` 从 `resource/meta/` 加载真实元数据和静态数据，
分别使用资源相对路径及以 `/` 开头的旧路径，验证内核与配置模块的四种初始化顺序，共八种组合。
