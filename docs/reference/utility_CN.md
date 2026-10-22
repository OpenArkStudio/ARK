# Utility 插件

`ark_utility_plugin` 提供 GUID、定时器、调度和压缩模块。使用方链接 `ark::utility` 获取公开头文件和编译要求，再通过应用清单选择 `ark_utility_plugin` 加载实现。仅链接 SDK 目标不会加载模块。

## 公开接口

| 头文件 | 接口与行为 |
| --- | --- |
| `<ark/utility/guid_module.hpp>` | `guid_module::create_guid()` 和 `parse_uid()`；保留现有 GUID 模式及由总线地址生成的 worker 位 |
| `<ark/utility/timer_module.hpp>` | 由管理器时间戳驱动的一次或重复定时器；Utility 中只有该模块注册所属线程更新 |
| `<ark/utility/schedule_module.hpp>` | 间隔、指定时间和 cron 回调；保留现有调度工作线程配置 |
| `<ark/utility/compress_module.hpp>` | zlib 与 Zstandard 的 `compress()` / `decompress()`，支持二进制负载 |

插件注册完成后，在模块初始化中获取接口：

```cpp
#include <ark/utility/guid_module.hpp>
#include <ark/utility/timer_module.hpp>

auto* guids = manager.find_module<ark::guid_module>();
auto* timers = manager.find_module<ark::timer_module>();
const auto entity = guids->create_guid();
```

接口指针借用于已加载插件，使用期间必须保持插件存活。定时器成员回调还借用接收者，接收者应保留到回调完成或被移除。在所属线程以毫秒时间戳推进管理器；创建定时器本身不会推进时间。schedule 模块继续使用已有后台调度器，线程池配置保持为 16。本次迁移不增加回调取消、热更新或新的停机保证。卸载前，按[模块生命周期](../guides/modules_CN.md)完成生产者和回调的收尾。

插件按 GUID、timer、schedule、compress 顺序安装模块，卸载顺序相反。timer 的 `pre_shut()` 仍关闭时间轮。GUID 布局、压缩级别与算法、scheduler 行为不变。动态库主体名称仍为 `ark_utility_plugin`，保留平台扩展名和 Debug `_d` 后缀。

## 源码与构建树 SDK 迁移

公开头文件的唯一源码现在位于 `src/plugins/utility/`。CMake 将显式列出的七个公开头复制到构建树的 `src/plugins/sdk-include/ark/utility/`：四个模块接口，以及 `scheduler.hpp`、`ctpl.h`、`ctpl_stl.h`。通过 `ark::utility` 消费，保留现有 `<ark/utility/...>` 公开接口与辅助头路径，关闭运行时插件时同样有效。不要把私有插件目录加入使用方的包含路径。上游线程池头文件保留原字节和格式规则。本次没有新增 `find_package(ark)` 安装包。

**源码不兼容变更：** SDK 移除 `ark/utility/{guid,timer,schedule,compress}_module_impl.hpp` 和 `ark/utility/utility_plugin.hpp`。实现类定义在各自 cpp 文件中，插件类型及返回接口的工厂属于私有装配。将直接构造实现类的代码改为公开接口查找，或应用自行维护实现。仅手写 `-I <ark-source>/include` 的调用需改用目标传递的包含目录。插件和使用方应一起重新编译；公开虚函数及动态入口签名保持不变，不代表兼容旧实现类二进制。

## 验证

`check-typed-examples` 与 Timers、`module_lifecycle` 一起加载真实 Utility DSO，验证四个接口的 RTTI 注册键与管理器归属、GUID 生成与解析、zlib/Zstandard 二进制往返、由所属线程时间推进触发的定时器，以及正常和重复停机。`check-app-presets` 使用仓库启动器预设；应用、示例、Kernel、Utility 开启时，`check-core` 包含这两组检查。独立 `tests/consumer` 项目的 `check-sdk` 在关闭运行时插件时编译公开接口与辅助头路径，并拒绝可见的实现或装配头。这些检查验证正确性，不代表性能或任意回调生命周期验收。
