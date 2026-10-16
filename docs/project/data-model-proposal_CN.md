# 嵌套数据与配置提案

<Aside type="note" title="草案">

这份历史提案探索嵌套值，以及实体定义与配置定义的分离。它不是运行时 API、已确定的业务资源格式，也不是自动升级数据定义的功能。

</Aside>

## 设计方向

原笔记希望数据抽象能够表示嵌套结构，而不局限于固定的基础值类型。例如一份业务记录可以包含结构、数组或带键的值集合。这是数据建模目标，没有定义线格式、持久化布局或 C++ 所有权。

![元数据与嵌套值的分离设想](../assets/draft-data-model_CN.svg)

笔记同时区分运行时实体定义，以及可复用配置的定义与值：

| 历史文件名 | 设想中的含义 |
| --- | --- |
| `EntityClass.meta` | 运行时实例对象的定义 |
| `ConfigClass.meta` | 配置记录定义，类似表格的列 |
| `Config.config` | 使用上述定义的配置记录 |

这些是旧提案中的名称，不是当前加载器新要求的文件。现有资源清单是 `resource/meta/entity_class.config` 和 `resource/meta/config_class.config`，详见[数据驱动加载](../concepts/data-driven_CN.md)。

## 当前接口与拟扩展能力

当前 Kernel 提供类型化实体节点、带定义的表行，以及保存子实体的容器。现阶段应用使用这些接口。实体容器并不自动等同于任意嵌套值的通用数组，Protobuf 转换函数也不会为未来的值体系定义迁移规则。

后续扩展需要明确类型验证、嵌套引用、所有权与移除行为、变更通知、序列化及兼容性，还需要说明类与字段索引、已有配置记录如何映射到新结构。此提案尚未给出这些规则，也没有定义安全重载协议。

## 迁移边界

在明确设计并验证迁移前，保持现有元数据名称、生成索引、保存数据和外部消息契约一致。服务进程配置与业务数据是不同问题；选择服务清单格式，不代表同时选择实体或配置资源的表示方式。

现有模型见[数据结构](../guides/data-structures_CN.md)，元数据草图见[相关设计说明](data-design_CN.md)。中文[原始来源](https://github.com/OpenArkStudio/ark/blob/main/docs/site/legacy-source/design/data.md)保留在[源码归档](https://github.com/OpenArkStudio/ark/blob/main/docs/site/legacy-source/README.md)中，不声称存在历史英文原文。
