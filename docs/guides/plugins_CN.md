# 创建插件

插件负责安装和卸载一组模块。核心与官方插件放在 ark 仓库，业务插件放在应用仓库；
完整目录说明见[项目结构与 SDK](../reference/directory-structure_CN.md)。

## 声明插件

`sample_plugin.hpp`：

```cpp
#pragma once

#include <ark/plugin.hpp>
#include <ark/plugin_manager.hpp>

namespace ark {

ARK_DECLARE_PLUGIN(sample_plugin)

} // namespace ark
```

## 注册模块

`sample_plugin.cpp` 使用[模块示例](modules_CN.md)中的 `sample_module`：

```cpp
#include "sample_module.hpp"
#include "sample_plugin.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(sample_plugin)

void sample_plugin::install() {
    ARK_REGISTER_MODULE(sample_module, sample_module);
}

void sample_plugin::uninstall() {
    ARK_UNREGISTER_MODULE(sample_module, sample_module);
}

} // namespace ark
```

<Aside type="note" title="接口与实现">

注册和卸载宏的第一个类型是模块接口，第二个类型是具体实现。
上面的简单示例使用同一个类型；模块有独立接口时，分别填写接口与实现类型。

</Aside>

## 构建动态库

```cmake
add_library(sample_plugin SHARED sample_plugin.cpp)
target_link_libraries(sample_plugin PRIVATE ark::core)
set_target_properties(sample_plugin PROPERTIES PREFIX "")
```

应用通过 CMake 引入 ark 后，`ark::core` 提供公开头文件与编译要求。
使用网络等官方插件接口时，增加对应的 `ark::net` 等目标。

配置文件使用动态库名称，C++ 插件类型名称可以与之不同。
现有插件动态库文件名保留，新 C++ API 需要应用和插件一起重新编译。
