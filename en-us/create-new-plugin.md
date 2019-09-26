# Create a new plugin quickly

## A simple plugin

Header file

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
        //Default constructor function
        explicit AFDirPlugin() = default;

        //Plugin version(Not used for now, plan to match version number)
        int GetPluginVersion() override;

        //plugin name
        const std::string GetPluginName() override;

        //install plugin
        void Install() override;

        //remove plugin
        void Uninstall() override;
    };

}
```

Source file

```cpp
#include "AFDirPlugin.h"
#include "AFCDirNetModule.h"

namespace ark
{
    //Define plugin DLL entry and exit
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
        //Registration plug-in modules
        RegisterModule<AFIDirNetModule, AFCDirNetModule>();
    }

    void AFDirPlugin::Uninstall()
    {
        //Uninstall plug-in modules
        DeregisterModule<AFIDirNetModule, AFCDirNetModule>();
    }

}
```

## Plugin description

The plug-in installs `Install`, and registers the modules in the plug-in one by one through the `RegisterModule` function.

The plug-in uninstall, and the modules in the plug-in are de-registered from the plug-in one by one through the `DeregisterModule` function.

> [!ATTENTION]
> There are two class names in module registration and de-registration, the front is `interface class` and the `concrete class` after.