# 创建插件

插件负责安装和卸载一组模块。动态库导出供 ark 启动器调用的入口，模块则提供供业务代码使用的接口。核心与官方插件放在 ark 仓库，业务插件放在应用仓库。

本文把[创建模块](modules_CN.md)中的计数器和调用模块组成一个业务插件。示例将 `sample_module.hpp`、`sample_plugin.hpp`、`sample_plugin.cpp` 和 `CMakeLists.txt` 放在同一个目录中。

## 声明插件

保存为 `sample_plugin.hpp`：

```cpp
#pragma once

#include <ark/plugin.hpp>
#include <ark/plugin_manager.hpp>

namespace ark {

ARK_DECLARE_PLUGIN(sample_plugin)

} // namespace ark
```

`ARK_DECLARE_PLUGIN` 生成插件类、管理器与配置访问方法，以及 `install()` 和 `uninstall()` 声明。下面的宏使用未限定命名空间的 ark 类型，因此示例在 `namespace ark` 中声明这些类型。

## 配对注册与注销

保存为 `sample_plugin.cpp`：

```cpp
#include "sample_module.hpp"
#include "sample_plugin.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(sample_plugin)

void sample_plugin::install() {
    ARK_REGISTER_MODULE(counter_module, counter_module_impl);
    ARK_REGISTER_MODULE(sample_module, sample_module);
}

void sample_plugin::uninstall() {
    ARK_UNREGISTER_MODULE(sample_module, sample_module);
    ARK_UNREGISTER_MODULE(counter_module, counter_module_impl);
}

} // namespace ark
```

| 操作 | 含义 |
| --- | --- |
| `ARK_REGISTER_MODULE(counter_module, counter_module_impl)` | 创建实现对象，设置管理器和名称，以公开接口注册 |
| `ARK_REGISTER_MODULE(sample_module, sample_module)` | 简单模块可以使用同一个类型作为接口和实现 |
| `ARK_UNREGISTER_MODULE(...)` | 移除对应模块与更新列表中的记录，并删除模块对象 |
| `ARK_DECLARE_PLUGIN_DLL_FUNCTION(sample_plugin)` | 定义导出的 `DllEntryPlugin`、`DllExitPlugin` 和插件配置存储 |

<Aside type="note" title="每个动态库只定义一组入口">

每个动态库只在一个翻译单元中使用 DLL 宏。它生成固定的 C 入口名称；在多个模块源文件中重复使用，或在同一个库中定义两个独立插件入口，都会造成重复定义。C 入口仍传递 C++ 类型，不代表跨编译器的稳定 ABI。

</Aside>

`install()` 只负责注册，跨模块依赖放在[创建模块](modules_CN.md)说明的生命周期阶段中处理。每个接口与实现的注册都应有对应的注销。示例先移除调用模块，再移除计数器；这个局部顺序不代表不同插件之间具有依赖排序。

## 构建动态库

安装 ark 的[构建依赖](../get-started/installation_CN.md)，再把下面的代码保存为业务插件目录中的 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.21)
project(sample_business LANGUAGES CXX)

set(ARK_SOURCE_DIR "" CACHE PATH "ark source checkout")
set(ARK_BUILD_APPLICATIONS OFF CACHE BOOL "Build ark applications" FORCE)
set(ARK_BUILD_PROTO OFF CACHE BOOL "Build protocol messages" FORCE)
set(ARK_BUILD_TESTS OFF CACHE BOOL "Build ark tests" FORCE)
set(ARK_BUILD_CORE_TESTS OFF CACHE BOOL "Build core tests" FORCE)
set(ARK_BUILD_EXAMPLES OFF CACHE BOOL "Build ark examples" FORCE)
add_subdirectory("${ARK_SOURCE_DIR}" ark)

add_library(sample_plugin SHARED sample_plugin.cpp)
target_link_libraries(sample_plugin PRIVATE ark::core)
set_target_properties(sample_plugin PROPERTIES
    PREFIX ""
    DEBUG_POSTFIX "_d"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
```

配置时传入 ark 源码目录。下面的命令同时适用于单配置生成器，以及 Visual Studio 等多配置生成器：

```sh
cmake -S . -B out -DARK_SOURCE_DIR="path/to/ark" -DCMAKE_BUILD_TYPE=Release
cmake --build out --config Release --target sample_plugin
```

这个构建关闭 ark 自带的应用与示例，不影响外部项目的 `sample_plugin` 目标。`ark::core` 提供公开头文件、C++20 要求和传递依赖。接口使用官方插件 API 时，再增加 `ark::net` 等 SDK 目标。完整目标列表见[项目结构与 SDK](../reference/directory-structure_CN.md)。

## 在运行时选择动态库

构建要求与运行选择是不同的步骤：

| 配置 | 作用 |
| --- | --- |
| `target_link_libraries(sample_plugin PRIVATE ark::net)` | 提供网络 API 头文件和编译要求，不会加载网络插件 |
| 在 ark 应用构建中设置 `ARK_BUILD_APPLICATIONS=ON` 与 `ARK_BUILD_PLUGIN_NET=ON` | 构建选中的官方插件动态库 |
| 在应用清单中增加插件条目 | 让启动器在启动时请求加载这个动态库 |

将这个 Release 示例生成的动态库从 `out/bin/Release/` 复制到部署目录的 `bin/`，并将下面的清单保存为 `conf/sample.app.conf`。Debug 构建产物单独放在 `out/bin/Debug/`：

```xml
<xml>
  <plugins plugin_lib_dir="bin/" plugin_conf_dir="conf/">
    <plugin name="sample_plugin" conf="" />
  </plugins>
</xml>
```

通过[应用启动器](applications_CN.md)选择这个清单。`sample_plugin` 是动态库文件名的主体，不是 C++ 插件类的 RTTI 名称，也不是模块注册名。加载器会补充 `.dll`、`.so` 或 `.dylib` 后缀；Debug 模式还会在扩展名前补充 `_d`。CMake 目标显式移除了 `lib` 前缀，并设置了相同的 Debug 后缀。启动器和插件应使用相同构建模式，以及兼容的 SDK 与 ABI 设置。

路径相对于进程工作目录解析。当前加载器直接拼接 `plugin_lib_dir` 与库文件名，以及 `plugin_conf_dir` 与 `conf`，因此目录末尾需要保留分隔符。示例中的空 `conf` 表示插件没有需要读取的配置文件，但插件收到的解析路径仍是配置目录前缀，不是空字符串。提供配置文件时，可通过 `sample_plugin::get_plugin_conf()` 读取这个路径。

## 排查模块不可用

| 现象 | 检查项 |
| --- | --- |
| 无法打开动态库 | 部署工作目录、文件名前缀与扩展名、Debug 后缀及动态库依赖 |
| 找不到入口 | DLL 宏是否只定义一次，导出符号是否可见 |
| 查找模块触发断言，或关闭断言时返回 `nullptr` | 运行清单是否包含提供模块的库，`install()` 是否以 `T` 注册，调用方与提供方是否使用兼容接口 |
| 头文件可以编译，但没有可用服务 | SDK 目标只提供头文件，不会在运行时安装模块 |

管理器不能保证任意未完成回调下的插件卸载安全。应按[创建模块](modules_CN.md)中的约束完成服务取消和对象生命周期管理。这个示例没有工作线程任务、事件订阅或热更新能力。
