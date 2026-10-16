# 常见问题

## 是否需要 C++20？

需要。CMake 要求 C++20，并关闭编译器扩展。详见[安装与构建](../get-started/installation_CN.md)。

## arknet 是否属于这个仓库？

arknet 是独立的 header-only 网络库。ark 使用固定发布版本；联合开发时可通过 `ARKNET_SOURCE_DIR` 指定本地仓库。

## 网络测试是否需要 Python 或 Ruby？

不需要。测试是项目内可直接编译运行的 C++ doctest 程序。独立的 Excel 配置生成器有自己的依赖。

## 是否已经验证 10–20 万同时连接？

这是多进程部署的预设目标。本机正确性测试不能证明该容量，验收范围见[基准测试与容量验收](../performance/benchmark_CN.md)。
