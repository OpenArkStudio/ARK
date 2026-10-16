# Bus 插件

Bus 为进程分配逻辑地址，读取应用类型、进程端点和连接关系。`bus_module` 提供配置查询，`msg_module` 根据应用类型、Bus ID 或 Session 发送消息；实际连接由 Net 插件管理。

## 地址与职责

Bus 地址使用 `channel.zone.app.instance` 四段形式，例如 `1.1.31.1`。四段各占 8 位，组合为 `bus_id_t`；它是进程标识，不是 IP 地址或监听端口。

| 段 | 含义 | 示例 |
| --- | --- | --- |
| `channel` | 应用划分的渠道 | `1` |
| `zone` | 应用划分的区域 | `1` |
| `app` | `app_type` 的数值 | `31` 对应 `ark_app_game` |
| `instance` | 同类进程实例编号 | `1` |

| 接口 | 职责 |
| --- | --- |
| `bus_module` | 本进程信息、应用名称映射、目标关系和其他进程配置 |
| `msg_module` | 消息序列化、选择已有连接、填写消息头并请求发送 |
| `net_service_manager_module` | 监听服务、客户端连接、Bus 与 Session 的映射 |
| `net_client_service` / `net_server_service` | 收包回调、连接事件和连接状态处理 |

## 配置进程与连接关系

`bus_module::init()` 读取 `bus.plugin.conf` 的四个部分：

| 配置节点 | 内容 |
| --- | --- |
| `applications` | 应用名称与 `app_type` 数值的映射 |
| `relations` | 有方向的连接目标：`source` 应连接到哪些 `target` 类型 |
| `processes` | 每个 Bus ID 的监听端点、连接上限和线程配置 |
| `center` | 注册中心参数，供 `get_reg_center()` 查询 |

下面说明 Game 与 World 两类进程的配置关系。内外端点使用不同端口，避免同时监听时冲突。

```xml
<xml>
    <applications>
        <application name="world" type="30" />
        <application name="game" type="31" />
    </applications>
    <relations>
        <relation source="game" target="world" />
    </relations>
    <processes>
        <proc busid="1.1.30.1" endpoint_server="tcp://127.0.0.1:9101"
            endpoint_intranet="tcp://127.0.0.1:9111" max_connection="100" thread_num="1" />
        <proc busid="1.1.31.1" endpoint_server="tcp://127.0.0.1:9102"
            endpoint_intranet="tcp://127.0.0.1:9112" max_connection="100" thread_num="1" />
    </processes>
    <center ip="127.0.0.1" port="8500" service_name="ark-service"
        check_interval="10s" check_timeout="3s" />
</xml>
```

这是 Bus 配置示例，不是完整的双进程启动配置。应用还需加载 Bus、Net 及其依赖插件，设置各自的 Bus ID，启动监听并发起连接。`relations` 只提供目标类型列表；读取关系本身不会自动建立连接。

`center` 有配置项不等于已启用动态注册或发现。当前网络服务管理模块中的 Consul 注册 / 健康检查发现路径仍为注释，不能依靠这份配置自动获得目标进程。

## 查询本进程与目标进程

在模块完成初始化后，通过插件管理器取得 Bus 接口。以下函数收集当前应用类型对应的、静态配置中的目标进程。

```cpp
#include <vector>

#include "ark/bus/bus_module.hpp"
#include "ark/plugin_manager.hpp"

std::vector<ark::proc_config> configured_targets(ark::plugin_manager& manager) {
    auto* bus = manager.find_module<ark::bus_module>();
    if (bus == nullptr)
    {
        return {};
    }

    std::vector<ark::app_type> types;
    if (!bus->get_target_bus_relations(types))
    {
        return {};
    }

    std::vector<ark::proc_config> result;
    for (auto type : types)
    {
        std::vector<const ark::proc_config*> processes;
        bus->get_other_proc(type, processes);
        for (const auto* process : processes)
        {
            result.push_back(*process);
        }
    }
    return result;
}
```

`get_other_proc(type, list)` 向输出列表追加配置指针，返回追加后列表的总长度。上例每次使用空列表，并复制配置，避免把模块内部指针保存在外部。该查询按类型筛选，不额外限定渠道或区域；应用需要时应自行筛选。

| 查询 | 结果与限制 |
| --- | --- |
| `get_self_bus_id()` / `get_self_bus_name()` | 当前进程的整数 ID / 四段地址 |
| `get_self_proc()` | 本进程的 `proc_config`；它是静态配置，不代表监听已启动 |
| `get_app_type(name)` | 未找到时返回 `ark_app_default` |
| `get_app_name(type)` | 未找到时返回空字符串 |
| `get_other_proc(id, proc)` | 找到时复制配置并返回 `true`；未找到返回 `false` |
| `combine_bus_id(type, instance)` | 沿用本进程的渠道和区域，组合指定类型与实例的 ID |

## 建立 TCP 连接

目标进程应先通过 `net_service_manager_module::create_server()` 启动监听。下面从静态配置取目标的内网端点，并将 TCP 连接请求交给网络服务管理模块。

```cpp
#include "ark/bus/bus_module.hpp"
#include "ark/net/net_service_manager_module.hpp"
#include "ark/plugin_manager.hpp"

bool queue_tcp_connection(ark::plugin_manager& manager, ark::bus_id_t target_id) {
    auto* bus = manager.find_module<ark::bus_module>();
    auto* network = manager.find_module<ark::net_service_manager_module>();
    if (bus == nullptr || network == nullptr)
    {
        return false;
    }

    ark::proc_config target;
    if (!bus->get_other_proc(target_id, target))
    {
        return false;
    }

    const auto& endpoint = target.intranet_ep;
    if (endpoint.proto() != ark::proto_type::tcp || endpoint.get_ip().empty() || endpoint.get_port() == 0)
    {
        return false;
    }

    return network->create_client_service(
        ark::bus_addr(target_id), ark::proto_type::tcp, endpoint.get_ip(), endpoint.get_port());
}
```

返回 `true` 表示连接请求已加入客户端服务，连接在后续 `update()` 中处理；它不表示已经连接成功。通过客户端服务的 `reg_net_event_callback()` 观察连接事件，通过 `reg_msg_callback()` 注册消息处理。接收端使用对应服务端接口注册回调。

当前客户端服务按状态处理断线重连，重试间隔写在实现中，为 30 秒；它不是此配置文件中的可调退避策略。重连不会替应用重放此前发送失败的业务消息。

## 发送与接收消息

连接可用后，通过 `msg_module` 发送应用自己的 Protobuf 消息。下面的函数接收调用方定义的消息 ID 和消息类型，不预设业务协议。

```cpp
#include "ark/bus/msg_module.hpp"

bool send_to_process(ark::msg_module& messages, ark::bus_id_t target_id, ark::msg_id_t message_id,
    const google::protobuf::Message& body) {
    return messages.send_msg_by_bus_id(target_id, message_id, body);
}
```

| 发送接口 | 连接选择 |
| --- | --- |
| `send_msg_by_bus_id()` | 使用源进程与目标 Bus ID 对应的已有连接，生成进程间消息头 |
| `send_def_ss_msg_by_app_type()` | 在指定应用类型的客户端服务中，通过一致性哈希选择连接；当前选择键为本进程 Bus ID |
| `send_def_cs_msg_by_app_type()` | 与上项采用相同的连接选择方式，使用客户端消息头 |
| `send_to_client()` | 通过外部监听服务，发送到指定 Session |
| `send_to_intra()` | 通过内网监听服务，发送到指定 Session |

进程间消息头携带消息 ID、源 / 目标 Bus ID 和 Actor ID。发送失败会返回 `false`，例如没有服务、连接不可用或序列化失败；返回 `true` 不表示远端业务已处理成功。需要请求响应、超时和幂等保证时，由应用协议实现。

接收流程是：网络服务拆出 `net_msg` → 按消息 ID 调用已注册回调 → 使用 `msg_module::decode_protobuf()` 解析业务消息。解析函数返回 `false` 表示 Protobuf 解析失败；`read_message_body()` 可复制原始消息体并取得 Actor ID。回调中的 `net_msg*` 是借用数据，需要延后处理时应先复制所需内容。

## 覆盖范围与后续工作

本页示例覆盖配置查询、显式 TCP 连接请求和已有连接上的消息发送。它不构成经过验收的多进程部署示例，也不保证所有协议在服务管理层均已接入。底层传输能力见 [Net 插件](network_CN.md)。

TODO：提供独立的双进程示例与测试，覆盖连接建立、消息往返、进程重启、重连和跨机器路由；动态注册 / 发现另行实现和验证。

## API 来源

- [Bus 地址与进程配置](https://github.com/OpenArkStudio/ark/blob/main/include/ark/bus.hpp)
- [Bus 接口](https://github.com/OpenArkStudio/ark/blob/main/include/ark/bus/bus_module.hpp)
- [消息接口](https://github.com/OpenArkStudio/ark/blob/main/include/ark/bus/msg_module.hpp)
- [网络服务管理接口](https://github.com/OpenArkStudio/ark/blob/main/include/ark/net/net_service_manager_module.hpp)
- [网络构建与测试](../networking_CN.md)
