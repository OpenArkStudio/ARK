<p align="right">简体中文 · <a href="README.md">English</a></p>

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/assets/ark-wordmark-dark.svg">
    <img src="docs/assets/ark-wordmark-light.svg" alt="ark" width="320" height="144">
  </picture>
</p>

<h3 align="center">使用 C++20 构建模块化服务端应用的框架。</h3>

<p align="center">组织业务服务，描述实体数据，连接应用进程。</p>

<p align="center">
  <a href="https://github.com/OpenArkStudio/ark/actions/workflows/ci.yml"><img src="https://github.com/OpenArkStudio/ark/actions/workflows/ci.yml/badge.svg?event=merge_group" alt="合并队列 CI"></a>
  <a href="CMakeLists.txt"><img src="https://img.shields.io/badge/C%2B%2B-20-5267df" alt="C++20"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-Apache--2.0-5267df" alt="Apache-2.0"></a>
</p>

<p align="center"><a href="docs/get-started/installation_CN.md"><strong>开始使用 →</strong></a> &nbsp; · &nbsp; <a href="https://openarkstudio.github.io/ark/zh/">文档</a> &nbsp; · &nbsp; <a href="examples/">示例</a> &nbsp; · &nbsp; <a href="docs/project/roadmap_CN.md">开发规划</a></p>

---

ark 为 C++ 服务提供应用组织方式：插件组合、模块生命周期、实体数据及进程通信。适用于游戏服务端及其他模块化服务端应用，支持 Linux、macOS 和 Windows。核心与官方插件放在本仓库，业务插件放在所属应用仓库。

## 围绕业务组织服务

| 组织服务 | 描述数据 | 连接进程 |
| :--- | :--- | :--- |
| **插件与模块** | **实体数据** | **进程通信** |
| 通过接口注册服务，参与初始化、更新和关闭流程。 | 通过元数据描述属性、表格和容器，支持配置默认值及变更回调。 | 通过 [arknet](https://github.com/OpenArkStudio/arknet) 使用 TCP、WebSocket、HTTP 和 RUDP，并提供总线标识及消息路由。 |

通过 Protobuf 消息、Excel 配置生成、独立 C++ 回归测试和可选基准程序支持开发与验证。

## 用 C++ 类实现模块

按需覆盖服务所需的生命周期方法。这个模块记录应用更新次数，由插件将其注册到管理器。

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

继续了解[模块生命周期与接口](docs/guides/modules_CN.md)和[插件注册](docs/guides/plugins_CN.md)。

## 从示例开始

构建示例应用需要支持 C++20 的编译器、CMake 3.21 或更新版本，以及 vcpkg。[安装指南](docs/get-started/installation_CN.md)提供依赖与各平台的构建命令。Windows 需要 Visual Studio 2022 或更新版本。

完成构建并准备运行目录后，在 `run/sample1/` 下启动 `sample1`：

```sh
./bin/app --busid=1.0.1.1 --name=sample1 --app_conf=conf/sample1.app.conf
```

| 下一步 | 指南 |
| :--- | :--- |
| 运行示例应用 | [快速开始](docs/get-started/quick-start_CN.md) |
| 了解框架设计 | [架构](docs/concepts/architecture_CN.md) · [实体数据](docs/reference/kernel_CN.md) |
| 连接应用进程 | [网络接口](docs/reference/network_CN.md) · [消息总线](docs/reference/bus_CN.md) |
| 异步访问数据库 | [MySQL](docs/reference/mysql_CN.md) · [Redis](docs/reference/redis_CN.md) · [协程任务](docs/guides/tasks_CN.md) |
| 复现验证结果 | [测试](docs/networking_CN.md) · [数据库基准测试](docs/performance/database-coroutines_CN.md) |

通过 [vcpkg](vcpkg.json) 和 [Git 子模块](.gitmodules)管理依赖，arknet 的提交版本固定在 [CMakeLists.txt](CMakeLists.txt) 中。

## 客户端集成

[原 Unity 客户端](https://github.com/OpenArkStudio/ArkClient-Unity3D)尚未与当前 ark 服务端完成兼容性验证。后续再恢复维护，并与服务端一起联调。

## 参与贡献

欢迎报告问题、讨论设计及提交 PR。编码规范、测试和受保护分支的提交流程见[贡献指南](.github/CONTRIBUTING.md)。可通过 [GitHub Issues](https://github.com/OpenArkStudio/ark/issues) 和 [Discord](https://discord.gg/GmyBbcv) 交流。

## 核心开发者

- [Nick Yang](https://github.com/NickYang1988)
- [flyicegood](https://github.com/flyicegood)
- [loscaz](https://github.com/loscaz)

感谢所有[贡献者](https://github.com/OpenArkStudio/ark/graphs/contributors)。

---

[Apache-2.0](LICENSE) · 版权所有 OpenArkStudio。
