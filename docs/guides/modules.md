# Create a module

Modules expose services through public interfaces and are registered by plugins.
A module derives from `ark::module`; its `update()` method participates in the application loop.

## Define a module

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

`ARK_DECLARE_MODULE_FUNCTIONS` supplies the module name and manager accessors.
The registration macro assigns both. Modules use `find_module<interface_type>()`
to retrieve other registered modules. The counter illustrates frame updates.

## Lifecycle callbacks

| Callback | Purpose |
| --- | --- |
| `init()` | Initialize module resources |
| `post_init()` | Perform work that depends on other modules being initialized |
| `check_config()` | Validate module configuration |
| `pre_update()` | Prepare for frame updates |
| `update()` | Run in the application update loop |
| `pre_shut()` | Begin module shutdown |
| `shut()` | Release module resources |

![Module lifecycle](../assets/module-lifecycle.svg)

The base class supplies implementations returning `true`. Override only the
stages a module needs. See [plugin_manager](https://github.com/OpenArkStudio/ark/blob/main/include/ark/plugin_manager.hpp)
for the current invocation and error handling behavior. The diagram describes
the existing startup, update and shutdown flow.

## Register and access modules

A plugin registers a module with `ARK_REGISTER_MODULE(interface_type, implementation_type)`
in `install()` and releases it with `ARK_UNREGISTER_MODULE(interface_type, implementation_type)`
in `uninstall()`. Pair the two operations as shown in [Create a plugin](plugins.md).

The manager exposes `find_module<interface_type>()`. Access module names with
`get_name()` and `set_name()`, and their manager with `get_plugin_manager()` and
`set_plugin_manager()`. Public interfaces do not require the concrete implementation;
any retained module pointer needs a defined lifetime.

arknet executes network IO and transport operations. The ark network plugin
queues application messages and events, then dispatches business callbacks in
`update()`. See the [network reference](../reference/network.md) for its constraints.
