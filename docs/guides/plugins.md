# Create a plugin

Plugins install and uninstall a group of modules. Core and official plugins live
in ark; business plugins live with their application. See [Project layout and SDK](../reference/directory-structure.md).

## Declare the plugin

`sample_plugin.hpp`:

```cpp
#pragma once

#include <ark/plugin.hpp>
#include <ark/plugin_manager.hpp>

namespace ark {

ARK_DECLARE_PLUGIN(sample_plugin)

} // namespace ark
```

## Register a module

`sample_plugin.cpp` uses `sample_module` from [Create a module](modules.md):

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

<Aside type="note" title="Interface and implementation">

The first macro argument is the module interface; the second is the concrete
implementation. The simple example uses the same type for both. When the module
has a separate interface, supply its interface and implementation types separately.

</Aside>

## Build the shared library

```cmake
add_library(sample_plugin SHARED sample_plugin.cpp)
target_link_libraries(sample_plugin PRIVATE ark::core)
set_target_properties(sample_plugin PROPERTIES PREFIX "")
```

After the application includes ark through CMake, `ark::core` provides public
headers and compile requirements. Add targets such as `ark::net` when using
other official plugin interfaces.

Configuration files use the shared-library name, which can differ from the C++
plugin type. Existing shared-library filenames are retained. Rebuild applications
and plugins together for the new C++ API.
