# 应用、插件与模块

| 层级 | 职责 | 源码入口 |
| --- | --- | --- |
| 应用 | 选择插件与进程配置 | [应用源码](https://github.com/OpenArkStudio/ark/tree/main/src/app) |
| 插件 | 安装与卸载一组模块 | [ark::sample1_plugin](https://github.com/OpenArkStudio/ark/blob/main/examples/sample1/sample1_plugin.cpp) |
| 模块 | 提供接口，参与应用生命周期 | [ark::module](https://github.com/OpenArkStudio/ark/blob/main/include/ark/module.hpp) |
| 网络 | 使用 arknet 端点，排队分发业务消息与事件 | [网络插件](https://github.com/OpenArkStudio/ark/tree/main/plugins/net) |

模块接口提供 `init`、`post_init`、`check_config`、`pre_update`、`update`、`pre_shut` 和 `shut`。初始化与关闭职责应成对处理；释放模块前，先停止引用它的回调来源。

arknet 提供 IO context、线程和传输操作。ark 网络插件负责业务消息头、Session ID 与 Update 线程上的消息分发。协议算法和 IO 线程执行逻辑放在 arknet 中。

继续阅读[创建插件](../guides/plugins_CN.md)或[网络与测试](../networking_CN.md)。
