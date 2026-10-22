# 实体访问基准

这个可选 C++ 程序保留旧实体示例的十二个热循环：字符串读取、整数读取和写入，分别直接按索引／名称访问，以及先经 Kernel 查找后访问。程序加载真实 Kernel／Utility 插件，但不加载 `entity_operations_plugin`，也不运行示例启动。测量模式保留旧 `system_clock` 毫秒耗时方式，不衡量服务容量或生产负载。

按安装说明准备依赖，开启 applications、configuration、Protobuf、Kernel／Utility 插件及 `ARK_BUILD_BENCHMARKS=ON`。
可执行文件不加入默认构建，也不是应用启动或 `check-core` 的依赖。

```sh
cmake -S . -B app-build -G Ninja -DCMAKE_BUILD_TYPE=Release -DARK_BUILD_BENCHMARKS=ON
cmake --build app-build --target ark_entity_operations_benchmark -j2
mkdir -p run/entity-benchmark/conf
cp config/kernel.plugin.conf run/entity-benchmark/conf/
cp -R examples/entity_operations/resource run/entity-benchmark/
cd run/entity-benchmark
../../app-build/benchmarks/ark_entity_operations_benchmark --plugin-dir ../../app-build/bin --iterations 5000000
```

必须显式指定正数次数，没有默认测量运行。每个循环五百万次对应旧的六千万次循环操作，不应用于正确性 CI。
Windows 为可执行文件添加 `.exe`，并使用匹配的配置输出目录。

小规模正确性 smoke 使用 `--smoke`，替代 `--iterations N`。它在关闭计时的情况下，每个循环只执行八次，并验证最终名称和 gender 值。
以下可选 target 准备相同元数据，只执行该 smoke：

```sh
cmake --build app-build --target check-entity-benchmark-smoke -j2
```

本次分离不发布基准测量或提速结论。编译和 smoke 通过不能证明性能；后续报告必须注明机器、操作系统、工具链、依赖、源码／二进制版本、显式次数及单位。未经单独授权，原始测量数据保留在本地。
