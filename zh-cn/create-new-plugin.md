# 快速开发一个新插件

## 一个简单插件的搭建

头文件

```cpp
#pragma once

#include "SDK/Core/AFPlatform.hpp"
#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

namespace ark
{

    class AFDirPlugin : public AFIPlugin
    {
    public:
        //默认的构造函数
        explicit AFDirPlugin() = default;

        //插件版本(暂未使用, 原本是用来检查版本匹配)
        int GetPluginVersion() override;

        //获取插件名称
        const std::string GetPluginName() override;

        //安装插件
        void Install() override;

        //卸载插件
        void Uninstall() override;
    };

}
```

源文件

```cpp
#include "AFDirPlugin.h"
#include "AFCDirNetModule.h"

namespace ark
{
    //定义插件dll入口和出口
    ARK_DLL_PLUGIN_ENTRY(AFDirPlugin)
    ARK_DLL_PLUGIN_EXIT(AFDirPlugin)

    //////////////////////////////////////////////////////////////////////////
    int AFDirPlugin::GetPluginVersion()
    {
        return 0;
    }

    const std::string AFDirPlugin::GetPluginName()
    {
        return GET_CLASS_NAME(AFDirPlugin);
    }

    void AFDirPlugin::Install()
    {
        //注册插件内模块
        RegisterModule<AFIDirNetModule, AFCDirNetModule>();
    }

    void AFDirPlugin::Uninstall()
    {
        //卸载插件内模块
        DeregisterModule<AFIDirNetModule, AFCDirNetModule>();
    }

}
```

## 插件流程说明

插件安装`Install`, 将插件内的模块通过`RegisterModule`一个个注册到插件内.  
插件卸载`Uninstall`, 将插件内的模块通过`DeregisterModule`一个个从插件内反注册.

> [!ATTENTION]
> 注意: 这里模块`module`注册和反注册中的两个类名, 前面为`Interface类`, 后面为`Concrete类`.