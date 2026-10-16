# 快速开发一个新插件

## 一个简单插件的搭建

头文件

```cpp
#pragma once

#include "interface/AFIPlugin.hpp"
#include "base/AFPluginManager.hpp"

namespace ark {

ARK_DECLARE_PLUGIN(Sample1Plugin)

} // namespace ark
```

源文件

```cpp
#include "Sample1Plugin.h"
#include "Sample1Module.h"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(Sample1Plugin)

void Sample1Plugin::Install()
{
    ARK_REGISTER_MODULE(Sample1Module, Sample1Module);
}

void Sample1Plugin::Uninstall()
{
    ARK_DEREGISTER_MODULE(Sample1Module, Sample1Module);
}

} // namespace ark
```

## 插件流程说明

插件安装`Install`, 将插件内的模块通过`ARK_REGISTER_MODULE`一个个注册到插件内.  
插件卸载`Uninstall`, 将插件内的模块通过`ARK_DEREGISTER_MODULE`一个个从插件内卸载.

> [!ATTENTION]
> 注意: 这里模块`module`注册和卸载中的两个类名, 前面为`接口(Interface)类`, 后面为`实现(Concrete)类`.  
> 文中具体的宏的实现请在项目代码中查看。