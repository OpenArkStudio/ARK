# Create a new plugin quickly

## A simple plugin

Header file

```cpp
#pragma once

#include "interface/AFIPlugin.hpp"
#include "base/AFPluginManager.hpp"

namespace ark {

ARK_DECLARE_PLUGIN(Sample1Plugin)

} // namespace ark
```

Source file

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

## Plugin description

The plug-in installs `Install`, and registers the modules in the plug-in one by one through the `RegisterModule` function.

The plug-in uninstall, and the modules in the plug-in are de-registered from the plug-in one by one through the `DeregisterModule` function.

> [!ATTENTION]
> There are two class names in module registration and de-registration, the front is `interface class` and the `concrete class` after.