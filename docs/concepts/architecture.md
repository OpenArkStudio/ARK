# Applications, plugins and modules

| Layer | Responsibility | Source entry |
| --- | --- | --- |
| Application | Select plugins and process configuration | [Application sources](https://github.com/OpenArkStudio/ark/tree/main/src/app) |
| Plugin | Install and uninstall a group of modules | [ark::sample1_plugin](https://github.com/OpenArkStudio/ark/blob/main/examples/sample1/sample1_plugin.cpp) |
| Module | Provide an interface and participate in the application lifecycle | [ark::module](https://github.com/OpenArkStudio/ark/blob/main/include/ark/module.hpp) |
| Networking | Integrate arknet endpoints and queue application messages/events | [Network plugin](https://github.com/OpenArkStudio/ark/tree/main/plugins/net) |

The module interface offers `init`, `post_init`, `check_config`, `pre_update`, `update`, `pre_shut` and `shut`. Keep initialization and shutdown responsibilities paired. Stop callback sources before releasing modules they reference.

arknet provides IO contexts, threads and transport operations. ark's network plugin handles application message headers, session IDs and Update-thread delivery. Protocol algorithms and IO thread runners belong in arknet.

Continue with [plugin creation](../guides/plugins.md) or [networking](../networking.md).
