# 协程任务

`<ark/task.hpp>` 提供 `ark::task<T>` 和 `ark::task_group`，用于显式管理 Asio 协程。应用线程通过任务组受理、观察和取消任务，并轮询网络库提供的 `arknet::manual_runtime`。该接口不会自动把任务绑定到模块，也不决定应用的停机策略。

`ark::task<T>` 是 Asio `awaitable<T>` 的类型别名，没有自定义 coroutine promise。任务组启动返回 `void` 的根任务；根任务可以通过 `co_await task<T>` 获得拥有型返回值，也可以等待兼容的 Asio 异步操作。

## 构建并运行示例

先按[安装与构建](../get-started/installation_CN.md)初始化已有依赖。在 macOS 或 Linux 中，从 ark 源码目录运行以下命令，并提供已配置的依赖路径：

```sh
cmake -S . -B task-build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_PROTO=OFF \
  -DARK_BUILD_CONFIG=OFF -DARK_BUILD_TESTS=OFF \
  -DARK_BUILD_CORE_TESTS=ON -DARK_BUILD_EXAMPLES=ON
cmake --build task-build --target ark_task_example check-task-sdk
./task-build/examples/task_group/ark_task_example
```

PowerShell 使用相同选项，写成单行：

```powershell
cmake -S . -B task-build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DARK_BUILD_APPLICATIONS=OFF -DARK_BUILD_PROTO=OFF -DARK_BUILD_CONFIG=OFF -DARK_BUILD_TESTS=OFF -DARK_BUILD_CORE_TESTS=ON -DARK_BUILD_EXAMPLES=ON
cmake --build task-build --target ark_task_example check-task-sdk
.\task-build\examples\task_group\ark_task_example.exe
```

如有需要，补上项目构建时使用的依赖 toolchain 或 prefix 选项。默认使用 Boost.Asio；仅构建核心任务时也支持 `-DASIO_STANDALONE=ON`。开启 `ARK_BUILD_EXAMPLES=ON` 后，即使关闭应用，也会构建这个普通可执行程序；它不需要插件动态库、生成的 Protobuf 消息或数据库服务。

[完整示例](../../examples/task_group/main.cpp)输出 `result=42 completed=1`。它启动一个协程后停止受理根任务，继续轮询直到已受理任务完成。示例中的两秒检查仅用于失败时以代码 2 结束测试程序，避免销毁尚未完成的协程；它不是应用停机超时或放弃任务的策略。

在已经通过 CMake 加入 ark 的项目中，使用公开目标 `ark::core`：

```cmake
target_link_libraries(my_tasks PRIVATE ark::core)
```

该目标传递 C++20、公开头文件路径和 arknet 依赖要求，无须加入私有的 `src/plugins` 目录。任务实现在公开头文件中。

## 在所属线程启动有返回值的任务

子协程可以挂起并返回一个值：

```cpp
#include <ark/task.hpp>

ark::task<int> next_value(int value) {
    co_await asio::post(asio::use_awaitable);
    co_return value + 1;
}

ark::task<> compute(int& result) {
    result = co_await next_value(41);
}
```

示例的 `main()` 创建共享的 `arknet::manual_runtime` 和任务组，再提交 `compute(result)` 及完成回调。回调接收 `std::exception_ptr`，正常完成时为空。协程引用的状态必须存活到任务结束。协程 lambda 不能在闭包销毁后继续引用其捕获；使用具名协程函数，并明确参数的所有权，更容易检查生命周期。

runtime 和任务组必须在同一线程构造、操作和销毁。`manual_runtime` 不创建执行任务体或恢复协程的工作线程，而是在构造线程调用 `poll()` 时执行就绪的 Asio handler。定时器或名称解析等平台服务可能使用内部辅助线程。原生数据库或网络连接池的 IO 所有权仍由各自接口定义。

## API 与任务准入

| API | 行为 |
| --- | --- |
| `task_group(runtime, maximum = 64)` | 保持共享 runtime 存活，并限制已受理根任务数量；runtime 为空或上限为零时抛出 `std::invalid_argument` |
| `start(task<>, completion = {})` | 返回非零观察和取消 ID；达到上限或已停止受理时返回零，被拒绝的惰性协程不会启动，也不调用完成回调 |
| `stop_accepting()` | 停止受理新根任务，不取消或轮询已受理任务；在所属线程重复调用无副作用 |
| `cancel(id)` / `cancel_all()` | 请求 terminal cancellation；仍持有任务直到实际完成 |
| `close()` | 停止受理并请求取消，不轮询，也不等待 |
| `poll(maximum = 64)` | 在共享 runtime 上执行至多指定数量的就绪 handler，不等待；零表示不执行 |
| `pending()` | 返回已受理且尚未移除完成记录的根任务数量 |
| `closed()` | 只有停止受理且所有已受理根任务均已完成时才为真 |

丢弃任务 ID 不会取消任务。丢弃尚未启动的 `task<T>` 不会执行协程体。任务体的异常通过可选的完成回调传递，不会变成成功结果；用户完成回调抛出的异常会从 `poll()` 继续抛出。

轮询共享 runtime 时，也可能执行同一 runtime 上其他任务组或操作的 handler。handler 数量上限不是协程步骤数、耗时预算，也不保证业务代码在一帧内完成。runtime 不允许重入轮询。在其他线程调用操作会抛出 `std::logic_error`；析构的要求更严格，见下文。

## 任务完成后再销毁所属对象

如果希望自然完成，调用 `stop_accepting()` 后继续所属线程原有的更新流程。已受理任务可以继续执行后续 `co_await`；如果它们需要提交下一步操作，底层服务和操作桥接对象必须继续存活并受理操作。

如果需要取消，调用 `close()`，并继续驱动所需的服务完成通知、桥接分发和 runtime 轮询，直到任务实际完成。能否响应取消取决于异步操作和协程代码；取消请求不会强行销毁协程帧，也不能证明远端写入没有发生。

<Aside type="caution" title="任务组销毁前必须排空">

只能在构造线程销毁任务组，并且此时 `pending() == 0`。仍有任务或跨线程析构都会调用 `std::terminate`。析构不会分离任务、递归轮询或阻塞等待完成。runtime、异步操作所属对象及协程引用的状态都必须在所需期间继续存活。

</Aside>

最后一个完成回调会先移除任务组中的对应记录，因此该用户回调可以销毁已经为空的任务组对象；这不允许销毁仍有其他操作的对象。`closed()` 只描述本组跟踪的根任务，不代表传输层排空、回调屏障、动态库可卸载或模块停机流程已经完成设计。

## 继续阅读

- [创建模块](modules_CN.md)：生命周期回调与借用的模块接口。
- [创建插件](plugins_CN.md)：插件注册、构建与运行时选择。
- [数据库协程测试](../../tests/database/COROUTINES_CN.md)：内部原生操作桥接、所属线程更新顺序与显式测试清理；公开数据库插件迁移仍是独立工作。
