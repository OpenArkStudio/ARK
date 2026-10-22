# HTTP 服务端插件

可选 HTTP 插件公开 `http_server_module` 与 arknet 的 `http_router` 类型。
它显式启动 HTTP/1 监听，并在 arknet IO 工作线程上路由请求；Net 插件的 HTTP 客户端仍是独立接口。

## 构建与配置

启用 `ARK_BUILD_APPLICATIONS=ON`、`ARK_BUILD_PLUGIN_HTTP=ON`；应用使用 Boost.Asio 与 Boost.Beast。
部署带平台后缀及 Debug `_d` 的 `ark_http_plugin`，在应用清单中选择插件与配置：

```toml
[[plugins]]
name = "ark_http_plugin"
config = "http.plugin.toml"
```

配置路径相对于选定的插件配置目录解析。源码 `config/http.plugin.toml` 内容为：

```toml
[http]
host = "0.0.0.0"
port = 9090
threads = 4
```

三个字段均必需。`host` 是不含 NUL 的非空字符串，`port` 是 `0..65535` 的 TOML 整数，
`threads` 是 `1..255` 的整数。未知键、缺失／错误类型和越界值会使 `init()` 失败，诊断包含文件与字段；
语法错误还包含行列。给旧 XML 文件改名不会转换内容。参见[插件配置](../service-manifests_CN.md#http-与-consul-插件配置)。

`start_server()` 使用配置值。显式重载保留公开默认参数 `port = 8080`、`thread_num = 1`，
它们不是 TOML 默认值。零端口请求操作系统分配可用端口；公开模块不提供所选端口查询，
客户端需要发现监听地址时应选择已知配置端口。初始化只解析配置，不启动监听。

## 路由请求

启动后获取必需的已注册接口，在启动监听前注册路由：

```cpp
#include <ark/http/http_server_module.hpp>
#include <ark/plugin_manager.hpp>

bool listen(ark::plugin_manager& manager) {
    auto* server = manager.find_module<ark::http_server_module>();
    server->get_router().add(arknet::http::verb::get, "/users/:id",
        [](const auto& context, auto& response) {
            response.set(arknet::http::field::content_type, "text/plain");
            response.body() = "user:" + std::string(context.param("id"));
        });
    return server->start_server();
}
```

路由引用从模块借用；`get_router()` 每次返回同一个自有路由器。路由上下文与响应参数仅属于本次调用。
保持处理器状态有效，并同步 IO 工作线程访问的共享业务状态。路由不等待 `manager.update()`，
也不把处理器移到管理器线程。`make_200<response_body_type>()` 保留公开响应助手，
包括内容类型、长度和 keep-alive 行为。

运行中的模块拒绝再次启动。同一库中的两个模块实例不能预留相同非零端点。
`pre_shut()` 停止并释放服务端和端点预留，随后可再次启动该端点。正常管理器停服由所属线程驱动。
这是既有 HTTP 监听关闭行为，不提供通用插件热替换或 HTTP/2。arknet 继续拥有 IO 工作线程、请求解析与传输操作。

## 公共插件接口与头文件映射

公开头位于 `src/plugins/http/http_server_module.hpp`。`ark::http` 通过显式 CMake 映射，
保留 `<ark/http/http_server_module.hpp>`，关闭运行时插件与 Protobuf 后仍可使用。
具体类在 cpp 中继承 `module_base<http_server_module>`，由私有接口工厂创建；私有插件装配采用
`plugin_base` 及显式 `DllEntryPlugin`／`DllExitPlugin`。公开虚接口与路由类型、RTTI 键、库名和入口签名保持不变。
模块不注册更新回调，与原有继承的默认更新行为一致。

**头文件迁移：**将 `<ark/http/http_server_module_impl.hpp>` 与 `<ark/http/http_plugin.hpp>`
改为公开模块头，通过 `plugin_manager` 获取已加载接口。使用 `ark::http` 传递的包含目录，
不能仅依赖 `-I <源码>/include`。私有装配／解析头不进入映射；生成映射只是共置源码头的包含视图。

## 验证

启用 HTTP 目标时，`check-core` 包含的 `check-http-plugin` 加载真实 HTTP 动态库并发送真实回环 HTTP 请求。
它验证接口键、路由器身份、路径／查询路由、二进制 POST 正文、响应状态与头、无管理器更新时的 IO 工作线程派发、
所属线程停服期间活动处理器退出、重复停服及端点复用。停服中的请求允许既有连接关闭结果，
不要求停服开始后仍返回响应；未使用替身服务端或传输。

既有 37 用例 `check-network` 通过私有工厂保留 HTTP 重复绑定、重启、公开响应助手与无效 TOML 断言。
独立的 `tests/consumer/http` 仅链接 `ark::http`，在关闭应用／Protobuf 时分别编译公开头并拒绝私有头路径。
公共接口头消费命令见[目录结构](directory-structure_CN.md)，路由 API 见 [arknet HTTP](https://openarkstudio.github.io/arknet/zh/features/)。
