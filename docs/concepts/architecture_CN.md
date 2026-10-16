# 应用、插件与模块

ark 在一个应用进程中组合多个插件库。插件注册模块，模块提供接口，并接受 `plugin_manager` 的生命周期调用。先确定各层职责，再阅读[应用组合示例](application-architecture_CN.md)。

## 职责边界

| 组件 | 负责 | 不会自动提供 |
| --- | --- | --- |
| 应用启动器 | 进程标识、清单路径、插件选择与更新循环 | 集群调度或依赖解析 |
| 插件 | 一组模块的安装与移除 | 回调仍在执行时的安全代码替换 |
| 模块 | 专项接口、状态与生命周期工作 | 任意业务状态的并发访问安全 |
| Kernel 插件 | 元数据、静态配置、运行时实体及其事件 | 仅声明字段就自动持久化或同步到客户端 |
| 网络插件 | ark 消息头、会话标识，以及向应用更新线程交付消息 | 传输算法或每连接独立线程 |
| arknet | IO context、工作线程执行与传输操作 | ark 业务实体或应用生命周期的管理 |

[应用组合](application-architecture_CN.md)、[数据驱动](data-driven_CN.md)和[参考服务拓扑](server-architecture_CN.md)分别说明不同层次的关系。服务拓扑是部署参考，使用模块并不要求先部署整套服务。

## 进程内调用

插件在 `install()` 中调用 `ARK_REGISTER_MODULE(interface_type, implementation_type)`。管理器记录该接口，应用通过类型查找模块：

```cpp
#include "ark/plugin_manager.hpp"
#include "ark/utility/timer_module.hpp"

ark::timer_module* find_timers(ark::plugin_manager& manager) {
    return manager.find_module<ark::timer_module>();
}
```

返回值是借用指针，使用期间应保持所属插件已加载。模块缺失时，启用断言的构建会触发断言，关闭断言后才返回空指针；因此类型查找用于获取已注册的必需依赖，不适合探测可选服务。包含头文件或添加 CMake 依赖不会在运行时安装模块。详见[创建模块](../guides/modules_CN.md)与[创建插件](../guides/plugins_CN.md)。

## 生命周期与执行

启动时依次执行所有模块的 `init()`、`post_init()`、`check_config()` 和 `pre_update()` 阶段。启动器持续调用管理器的 `update()`，其中只有已注册更新的模块参与循环。停止时先调用 `pre_shut()`，再调用 `shut()`，然后卸载插件库。

在 `init()` 中获取依赖并初始化本地资源；需要其他模块完成初始化的工作放在后续阶段。释放回调引用的状态前，先停止定时器、订阅与异步生产者。网络业务消息通过应用更新调用推进，网络 IO 仍由 arknet 工作线程执行。

组合模块时需要遵守当前限制：

- 管理器按容器遍历模块，没有声明式依赖图，也不保证清单顺序。同一阶段内不能依赖某个模块先初始化。
- 当前管理器调用生命周期回调时没有汇总各模块的 Boolean 返回值。模块返回 `false` 不等于框架保证回滚整个启动过程或终止进程。
- 生命周期阶段不代表任意业务任务已经排空。兼容热替换与插件卸载协调仍是独立设计事项。

对应实现见 [plugin_manager.hpp](https://github.com/OpenArkStudio/ark/blob/main/include/ark/plugin_manager.hpp) 和[启动器](https://github.com/OpenArkStudio/ark/blob/main/src/app/main.cpp)。

## 头文件路径与 CMake 目标

所有公开头文件统一位于 `include/ark/`，例如 `include/ark/kernel/entity.hpp` 对应 `<ark/kernel/entity.hpp>`。核心目标定义位于 `src/core/`，插件实现位于 `src/plugins/<name>/`。`ark::core` 提供核心头文件，各插件仍保留独立可选目标，`ark::ark` 汇总可用 API 目标。

`ark/` 是文件系统中的头文件前缀，不会自动创建 C++ 命名空间；命名空间由各头文件显式声明的 `namespace ark` 决定。依赖 API 接口目标只传递编译要求，不会选择或加载插件动态库。

部分实现头文件仍与公开接口混放在 `include/` 目录中。将 `*_impl` 类型视为实现细节，不能把当前路径理解为已完成公开接口与实现的隔离。图示按职责组织，不限定物理目录布局。当前路径与目标见[目录结构](../reference/directory-structure_CN.md)。
