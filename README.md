<p align="right">English · <a href="README_CN.md">简体中文</a></p>

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/OpenArkStudio/.github/main/assets/ark-logo-dark.svg">
    <img src="https://raw.githubusercontent.com/OpenArkStudio/.github/main/assets/ark-logo-light.svg" alt="ark" width="307">
  </picture>
</p>

<h3 align="center">A C++20 framework for modular server applications.</h3>

<p align="center">Compose services. Model entities. Connect processes.</p>

<p align="center">
  <a href="https://github.com/OpenArkStudio/ark/actions/workflows/ci.yml"><img src="https://github.com/OpenArkStudio/ark/actions/workflows/ci.yml/badge.svg?event=merge_group" alt="Merge queue CI"></a>
  <a href="CMakeLists.txt"><img src="https://img.shields.io/badge/C%2B%2B-20-5267df" alt="C++20"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-Apache--2.0-5267df" alt="Apache-2.0"></a>
</p>

<p align="center"><a href="docs/get-started/installation.md"><strong>Get started →</strong></a> &nbsp; · &nbsp; <a href="https://openarkstudio.github.io/ark/">Documentation</a> &nbsp; · &nbsp; <a href="examples/">Examples</a> &nbsp; · &nbsp; <a href="docs/project/roadmap.md">Roadmap</a></p>

---

ark provides the application structure around your C++ services: plugin composition, module lifecycles, entity data and communication between processes. Use it for game servers and other modular server applications on Linux, macOS and Windows. Core components and official plugins live here; business plugins live with their application.

## Built around your services

| Compose | Model | Connect |
| :--- | :--- | :--- |
| **Plugins and modules** | **Entity data** | **Process communication** |
| Register services behind interfaces and participate in initialization, updates and shutdown. | Describe properties, tables and containers through metadata, configuration defaults and change callbacks. | Use TCP, WebSocket, HTTP and RUDP through [arknet](https://github.com/OpenArkStudio/arknet), with bus identifiers and message routing. |

Protobuf messages, Excel configuration generation, native C++ regression tests and optional benchmarks support the development workflow.

## A module is a C++ class

Override the lifecycle stages the service needs. This module counts application updates; a plugin registers it with the manager.

```cpp
#include <cstddef>

#include <ark/module.hpp>
#include <ark/plugin_manager.hpp>

namespace ark {

class sample_module final : public module
{
    ARK_DECLARE_MODULE_FUNCTIONS

public:
    bool update() override {
        ++update_count_;
        return true;
    }

private:
    std::size_t update_count_ = 0;
};

} // namespace ark
```

Continue with [module lifecycle and interfaces](docs/guides/modules.md) and [plugin registration](docs/guides/plugins.md).

## Start with an example

Build the example applications with a C++20 compiler, CMake 3.21 or newer, and vcpkg. The [installation guide](docs/get-started/installation.md) covers dependencies and platform-specific commands. Windows requires Visual Studio 2022 or newer.

After building and preparing the runtime directory, start `sample1` from `run/sample1/`:

```sh
./bin/app --busid=1.0.1.1 --name=sample1 --app_conf=conf/sample1.app.conf
```

| Next step | Guide |
| :--- | :--- |
| Run the bundled applications | [Quick start](docs/get-started/quick-start.md) |
| Understand the framework | [Architecture](docs/concepts/architecture.md) · [Entity data](docs/reference/kernel.md) |
| Connect application processes | [Networking](docs/reference/network.md) · [Message bus](docs/reference/bus.md) |
| Access databases asynchronously | [MySQL](docs/reference/mysql.md) · [Redis](docs/reference/redis.md) · [Coroutine tasks](docs/guides/tasks.md) |
| Reproduce validation results | [Tests](docs/networking.md) · [Database benchmarks](docs/performance/database-coroutines.md) |

Dependencies are managed with [vcpkg](vcpkg.json); see [dependency selection](docs/dependencies.md). The arknet revision is pinned in [CMakeLists.txt](CMakeLists.txt).

## Client integration

The [legacy Unity client](https://github.com/OpenArkStudio/ArkClient-Unity3D) has not been verified against the current ark server. Restoring it and testing the client and server together is planned follow-up work.

## Contribute

Bug reports, discussions and pull requests are welcome. See the [contribution guide](.github/CONTRIBUTING.md) for coding style, tests and the protected-branch workflow. Find us on [GitHub Issues](https://github.com/OpenArkStudio/ark/issues) and [Discord](https://discord.gg/GmyBbcv).

## Core developers

- [Nick Yang](https://github.com/NickYang1988)
- [flyicegood](https://github.com/flyicegood)
- [loscaz](https://github.com/loscaz)

Thanks to all [contributors](https://github.com/OpenArkStudio/ark/graphs/contributors).

---

[Apache-2.0](LICENSE) · Copyright OpenArkStudio.
