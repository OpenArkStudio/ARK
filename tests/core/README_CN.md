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

`ARK_BUILD_CONFIG=ON`（核心测试默认启用）时，`check-application-manifest`
验证 TOML / XML 等价性、默认值、严格字段、无效输入、诊断和拥有型结果，并纳入
`check-core`。参见[应用插件清单](../../docs/service-manifests_CN.md)。

同时构建应用时，`check-app-manifest-failures` 使用真实启动器验证文件缺失、语法错误和
字段类型错误的清单，要求正常返回退出码 1 并输出清单诊断。该入口覆盖 Windows，
并纳入 `check-core`。

`check-app-startup-failures` 使用真实启动器和测试动态库，覆盖四个启动阶段各自返回 false 或抛出异常，并分别读取 TOML 和 XML 清单；还验证非标准异常和插件入口抛出异常。20 个子进程均须输出一次准确的阶段、模块或插件诊断，保留可用异常信息，以正常退出码 1 结束，且不执行后续启动调用、update、正常关闭或卸载。核心用例检查失败上下文、原始异常传播、多模块中的首次失败停止及所属线程访问。这些测试证明失败报告与进程退出，不证明部分启动资源清理或同进程恢复。

`check-kernel-initialization` 从 `resource/meta/` 加载真实元数据和静态数据，
分别使用资源相对路径及以 `/` 开头的旧路径，验证内核与配置模块的四种初始化顺序，共八种组合。

`check-logging` 运行独立的 C++ 日志路径回归测试，并由 `check-core` 一并执行。
用例覆盖相对路径、嵌套路径、扩展名、隐藏文件、含点的目录名、两个按小时轮转的日志创建函数，
以及配置中的日志目录。`output_path` 为空时使用当前工作目录。按小时轮转的日志仍采用
`目录/YYYYMMDD/文件名_hh.扩展名` 布局；只有文件名的最后一个扩展名放在小时后缀之后。
没有扩展名或以点结尾时保留完整文件名。测试只在临时目录写入文件。

```sh
cmake --build core-build --target check-logging
```

## 公开任务 SDK

`check-core` 包含 `check-task-sdk`：独立编译仅包含 `<ark/task.hpp>` 的文件，
并运行使用 `ark::core` 的普通调用方。测试覆盖拥有型返回值、停止受理、显式取消
和所属线程检查。受控子进程检查要求在仍有任务或跨线程析构时以代码 77 结束，
不会在测试所属对象中直接放弃挂起的协程。私有数据库任务测试通过别名使用同一个
公开实现。参见[协程任务](../../docs/guides/tasks_CN.md)，了解可独立运行的
`ark_task_example` 和显式生命周期要求。

## 共享业务执行器

`check-core` 包含 `check-business-runtime`：独立编译仅包含
`<ark/plugin_manager.hpp>` 的调用方，验证单个共享执行器、统一 handler 预算、
模块完成通知先于轮询、停止受理后的连续等待、迟到结果、所属线程与重入检查、
有任务时的模块注销、组引用稳定性和失败诊断。真实注销宏必须先抛出错误，不能删除
仍有任务的模块。四个受控子进程验证有任务、跨线程、更新栈内和最后完成回调内析构。
测试不需要数据库服务。

```sh
cmake --build core-build --target check-business-runtime
```

管理器用例证明受管理业务任务的排空；下面的启动器用例还验证一个受控原生线程退出屏障，二者均不证明通用动态库卸载安全。
参见[模块任务集成](../../docs/guides/tasks_CN.md#模块共用业务执行器)。

同时构建应用与配置读取器时，`check-app-business-runtime` 在真实启动器中加载测试专用原生插件。POSIX 用例请求 SIGTERM，Windows 向真实命令线程输入 `exit`。已受理任务在排空阶段收到两次受控私有桥接结果；任务组仍有空余容量时也拒绝新根任务。最后完成回调返回后，owner frame 停止原生工作并等待线程退出、join，随后才进入关闭与卸载。默认无限等待和配置正数时限的正常排空均须返回退出码 0。另有关闭钩子返回失败或抛出异常、update 抛出异常、任务停滞、任务或原生线程延迟完成的子进程，要求诊断只输出一次、模块仍保留，并存活至测试观察时限。配置的应用等待时限到达后，后续完成不得恢复卸载；非法、负数和溢出的 CLI 时限必须在加载配置前拒绝。应用时限仅在 owner loop 返回时检查，不能中断阻塞中的用户更新、就绪检查或关闭调用。测试观察时限只终止测试自己创建的子进程，不表示成功停机或生产策略；测试不使用真实数据库。
