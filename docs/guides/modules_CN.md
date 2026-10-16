# 创建模块

模块通过公开接口提供服务，由插件注册和卸载。类型继承 `ark::module`；
应用循环通过 `update()` 分发需要逐帧处理的工作。

## 定义模块

```cpp
#pragma once

#include <cstddef>

#include <ark/module.hpp>
#include <ark/plugin_manager.hpp>

namespace ark {

class sample_module final : public module
{
    ARK_DECLARE_MODULE_FUNCTIONS

public:
    bool init() override {
        update_count_ = 0;
        return true;
    }

    bool update() override {
        ++update_count_;
        return true;
    }

private:
    std::size_t update_count_ = 0;
};

} // namespace ark
```

`ARK_DECLARE_MODULE_FUNCTIONS` 提供模块名称和管理器访问方法。
注册宏设置管理器与模块名称；模块通过 `find_module<接口类型>()` 获取其他已注册模块。
示例中的计数器展示逐帧更新。

## 生命周期回调

| 回调 | 用途 |
| --- | --- |
| `init()` | 初始化模块资源 |
| `post_init()` | 执行依赖其他模块初始化结果的工作 |
| `check_config()` | 检查模块配置 |
| `pre_update()` | 启动逐帧更新前的准备 |
| `update()` | 在应用更新循环中执行 |
| `pre_shut()` | 开始关闭模块 |
| `shut()` | 释放模块资源 |

![模块生命周期](../assets/module-lifecycle.svg)

基类为上述回调提供返回 `true` 的默认实现。只覆盖模块需要的阶段；
具体调用和错误处理以 [plugin_manager](https://github.com/OpenArkStudio/ark/blob/main/include/ark/plugin_manager.hpp) 为准。
这张图展示现有应用的启动、更新和关闭流程。

## 注册与访问

插件在 `install()` 中使用 `ARK_REGISTER_MODULE(接口类型, 实现类型)` 注册模块，
在 `uninstall()` 中使用 `ARK_UNREGISTER_MODULE(接口类型, 实现类型)` 释放模块。
两者必须对应，见[创建插件](plugins_CN.md)。

管理器提供 `find_module<接口类型>()`。模块名称由 `get_name()`、`set_name()` 访问；
所属管理器由 `get_plugin_manager()`、`set_plugin_manager()` 访问。
公开接口不依赖具体实现类，成员持有其他模块指针时，应明确其有效期。

网络 IO 和传输由 arknet 执行。ark 网络插件把业务消息与事件放入队列，
在 `update()` 中分发给业务回调；相关约束见[网络接口](../reference/network_CN.md)。
