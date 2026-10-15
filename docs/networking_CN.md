# 网络构建与回归测试

[English](networking.md) | 简体中文

ARK 使用 C++20，直接采用独立仓库的
[arknet v0.1.0](https://github.com/OpenArkStudio/arknet) 实现 TCP、WebSocket、HTTP
和 RDP。RDP 使用 RUDP 端点，保留旧 ARQ 报文格式，MTU 为 1400，默认关闭 FEC。
应用消息头格式和 Update 线程回调机制保持一致。本次不改造协程接口。

## 依赖

- CMake 3.21 及以上；GCC 12+、支持 C++20 的 Clang，或 VS 2022。
- Standalone Asio 1.38.0+；设置 `ASIO_STANDALONE=OFF` 时使用 Boost 1.90+。
- RapidXML 1.13、spdlog 1.17.0，以及仓库固定版本的 readerwriterqueue、args 子模块。
- 构建应用和插件时需要 Protobuf、Zstandard 和 zlib。

vcpkg 清单固定包版本基线。CMake 从 arknet 的独立仓库获取发行标签。
验证开发版本时，将 `ARKNET_SOURCE_DIR` 设为该仓库的绝对路径。

## 构建并运行网络测试

```sh
git submodule update --init 3rdparty/args 3rdparty/spdlog 3rdparty/readerwriterqueue
cmake -S . -B network-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_NETWORK_TESTS=ON \
  -DASIO_STANDALONE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build network-build --parallel 2
cmake --build network-build --target check-network
```

测试 Boost 后端时，增加 `-DVCPKG_MANIFEST_FEATURES=boost` 并设置
`-DASIO_STANDALONE=OFF`。正式 C++ doctest 用例位于 `test/network/`，也可以直接运行
测试程序，或用 `--test-case=RUDP*` 选择用例。不需要 Python 或 Ruby。

测试覆盖拆包、二进制数据、重连、连接上限、超时、Update 线程分发、回调中关闭连接、
HTTP 状态和请求数据、停机取消、非法输入，以及日志格式、目录创建和文件轮转。
CI 检查 Linux、macOS、Windows 的两种后端，
并在 Linux 上运行内存、未定义行为和线程检查。

2026-10-13 本机验证使用 Apple M4 Pro（14 核 CPU、48 GiB 内存）、macOS 27.0.1 arm64
和 Apple Clang 21.0.0。Standalone Asio 1.38.2、Boost 1.90、ASan/UBSan 和 TSan
四组网络与日志回归均通过 31 个用例、843 条断言。macOS ASan 未启用泄漏检测，Linux CI
启用 LeakSanitizer。这些结果用于验证正确性，不表示部署容量。
默认 C++20 应用构建、示例和 8 个原有测试程序也均在本机通过。

## 构建应用

启用 `application` 清单特性以安装 Protobuf：

```sh
cmake -S . -B app-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES=application
cmake --build app-build --parallel 2
```

## 部署验收

本机回归结果不能作为 10–20 万 PCU 的容量结论。生产部署前仍需进行跨机器负载、
长时间运行和完整业务流程验收。ARQ/FEC 的性能数据见
[arknet 性能报告](https://openarkstudio.github.io/arknet/zh/performance/)。

## 端点销毁

ARK 的 TCP、WebSocket 和 RUDP 客户端、服务端可能在 IO 回调期间继续持有封装对象。
回调释放最后一个所有者时，底层 arknet 端点交给共享的单线程清理执行器销毁，
从外部线程完成关闭和线程回收。外部线程发起的销毁仍同步完成；清理执行器退出时
会排空已提交的销毁任务。业务消息和连接事件仍通过 `Update()` 派发。

## 收包缓冲基准

[收包缓冲报告](performance/receive-buffers_CN.md)记录本机内存、拆包吞吐量与 CPU 测量，包含批量组的回退。独立 C++ 程序的构建方式见[基准指南](../benchmarks/README_CN.md)。
