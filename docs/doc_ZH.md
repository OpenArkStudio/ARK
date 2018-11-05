# 欢迎使用ARK敏捷服务器开发框架

关键字：文档，介绍，入门，教程

**ARK** 是一个使用C++语言开发的、支持高并发、高性能的跨平台敏捷服务器开发解决方案。
旨在帮助中小企业降低开发门槛，快速完成项目功能。采用敏捷开发中的分层设计思路，将功能拆分为多个插件模块，让开发人员集中处理单一业务功能，提高团队效率。<br>

特点概述：

* 通用的抽象对象系统
* 数据驱动 (DataNode & DataTable)
* 事件驱动 (Event)
* 可扩展的App、插件化、模块化 (Plugin & Module)
* 面向接口编程 (IOD)
* 高性能、高并发 (网络、Actor、逻辑)
* Component组件 (脚本系统)
* 分布式服务器架构
* 高稳定性、简易部署、支持扩展、跨平台
* 可视化配置、配套工具
* 配套客户端(Unity3D客户端(已完成))
* 企业定制化服务(存储方案、逻辑模块、新架构)


## 目录
<pre>
<a href="#简述">简述</a>
<a href="#特性详细介绍">特性详细介绍</a>
    <a href="#通用的抽象对象系统logicclass">通用的抽象对象系统(LogicClass)</a>
    <a href="#数据驱动">数据驱动</a>
    <a href="#事件驱动">事件驱动</a>
    <a href="#面向接口编程iod">面向接口编程(IOD)</a>
    <a href="#高性能高并发">高性能高并发</a>
    <a href="#组件系统component">组件系统(Component)</a>
    <a href="#分布式服务器架构">分布式服务器架构</a>
<a href="#使用教程">使用教程</a>
    <a href="#教程1添加一个模块">教程1:添加一个模块</a>
    <a href="#教程2数据驱动">教程2:数据驱动</a>
    <a href="#教程3事件系统">教程3:事件系统</a>
    <a href="#教程4异步事件系统actor">教程4:异步事件系统，actor</a>
<a href="#结束">结束</a>
</pre>

## 简述
ArkGameFrame采用成熟的敏捷开发思想——分层设计，分层的程序设计带来的好处是显而易见的，由于层间松散的耦合关系，使得我们可以专注于本层的设计，而不必关心其他层的设计，也不必担心自己的设计会影响其它层，对提高软件质量大有裨益。而且分层设计使得程序结构清晰，升级和维护都变得十分容易，更改层的具体实现代码，只要层接口保持稳定，其他层可以不必修改。即使层的接口发生变化，也只影响上层和下层，修改工作量小而且错误可以控制，不会带来意外的风险。Ark将应用程序设计成三层架构，最顶层是App，中间层是各种插件，插件下是各种对应的具化的模块功能。这种设计的优点是对应模块只处理自己的事务，降低耦合，通过接口与其他模块交互，将模块的风险降到最低。

**App设计架构图**
![App Architecture](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/docs/_images/app-architecture.png)

## 特性详细介绍
### 通用的抽象对象系统(LogicClass)
- 丰富的基础属性类型
- 对象属性的可配置性(Excel可以定义所有属性)
- 对象初始数据的可配置性(Excel可以预设值所有属性的值)
- 可动态增减属性(服务器运行过程中可以程序添加属性)
- 无需在代码中再声明任何业务类(Excel直接声明)
- 通用的设置/获取信息接口

### 数据驱动
相对于传统的服务器开发，Ark使用了一种全新的数据定义和使用的方法，我们称之为**数据节点(DataNode)**和**数据表(DataTable)**。

**数据节点(DataNode)**主要用来存储用户的基本数据，例如：姓名、性别、年龄、等级 等数据，主要表现为一个名称对应一个数据。

**数据表(DataTable)**主要用来存储一些记录，例如：道具列表、任务列表 等数据，主要表现为一个记录里包含多条数据。

Ark使用了此种模型来定义应用中的所有数据，避免了以往传统服务器中数据结构定义混乱，接口不统一、别人无法接手等问题。

**一个DataNode和DataTable的例子：**
![DataNode Sample](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/docs/_images/data-node-sample.png)
> Ark数据节点配置例子(Excel编辑)

![DataTable Sample](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/docs/_images/data-table-sample.png)
> Ark数据表配置例子(Excel编辑)

### 事件驱动
事件驱动灵感来源与处理器的处理流程，旨为只提供流水线式的处理逻辑模块，而本身不保存和涉留对象的数据。

事件驱动包含：**DataNode驱动**，**DataTable驱动**，**Event驱动**，**Class驱动**

通过**DataNode Driver**，所有只要注册过属性观测者的Processer Function均会得到所关注DataNodey的变化通知，以便做出对应的逻辑处理。

通过**DataTable Driver**，所有只要注册过Record的观测者的Processer Function均会得到所关注DataTable的变化通知，以便做出对应的逻辑处理。

通过**Event System**，所有只要注册过Event的观测者的Processer Function均会得到所关注的事件通知Processer可以产生新的事件或属性驱动，以便驱动其他逻辑模块处理逻辑。

通过**Class System**，所有只要注册过该class的观测者的Processer Function均会定时处理逻辑，以便做同步和类事件处理逻辑。

**Ark事件驱动设计图**
![Event Driven](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/docs/_images/event-driven.png)

**Ark事件驱动示例代码**

DataNode驱动示例：

```cpp
m_pKernelModule->AddNodeCallBack(self, "Level", this, &AFCPropertyModule::OnObjectLevelEvent);

m_pKernelModule->SetNodeInt(self, "Level", 100);

int AFCPropertyModule::OnObjectLevelEvent(const AFGUID& self, const std::string& strPropertyName, const AFIDataList& oldVar, const AFIDataList& newVar, const AFIDataList& argVar)
{
    // do something
    return 0;
}
```

DataTable驱动代码示例：

```cpp
m_pKernelModule->AddTableCallBack(self, "TaskList", this, &AFCHeroModule::OnHeroTableEvent);

int AFCHeroModule::OnHeroTableEvent(const AFGUID& self, const std::string& strRecordName, const int nOpType, const int nRow, const int nCol, const AFIData& oldVar, const AFIData& newVar)
{
    AF_SHARED_PTR<AFIObject> pObject = m_pKernelModule->GetObject(self);
    if (nullptr == pObject)
    {
        return 1;
    }

    AF_SHARED_PTR<AFIRecord> pRecord = m_pKernelModule->FindRecord(self, strRecordName);
    if (nullptr == pRecord)
    {
        return 1;
    }

    switch (nOpType)
    {
    case AFIRecord::Add:
        // TODO
        break;
    case AFIRecord::Del:
        // TODO
        break;
    case AFIRecord::UpData:
        // TODO
        break;
    default:
        break;
    }

    return 0;
}
```

Event驱动代码示例
```cpp
m_pEventProcessModule->AddEventCallBack(self, AF_EVENT_DO_SOMETHING, this, &AFCFightValueModule::OnDoSomethingEvent);

int AFCFightValueModule::OnRefreshFightValueEvent(const AFGUID& self, const int nEventID, const AFIDataList& var)
{
    // do something
	return 0;
}
```

### 面向接口编程(IOD)
&#160; &#160; &#160; &#160;较于大多数OO式开发，Ark支持更灵活的IO(接口)式开发，让你的开发更简单纯粹。
通过模块抽象基类的虚接口让模块的功能互相调用，真正做到了软件开发的低耦合高内聚。

**Ark面向接口编程示例代码**
```cpp
class AFISceneProcessModule
    : public AFILogicModule
{

public:
    virtual bool IsCloneScene(const int nSceneID) = 0;
    virtual E_SCENE_TYPE GetCloneSceneType(const int nContainerID) = 0;
};
```

### 高性能，高并发
Ark由于在设计上的分层独立从而使得架构上本身就性能较高。同时在网络通信上使用了轻量级的跨平台网络库**brynet**作为网络底层，使用**google ProtoBuf**作为协议序列化库，brynet的高性能加上Protobuf的高压缩率，真实测试过单服承载8000以上用户高频率协议通讯。

Ark使用C++作为基础开发语言，相对于其他编程语言，在性能和效率上更是快人一步，良好的设计模式的应用让逻辑更加简单。

### 组件系统(Component)
- 提供类似Unity样式的组件组合模式，以丰富服务器后期脚本编辑以及对象行为扩展
- 系统在对象产生时自动识别以及实例化Component
- 通过事件系统，向Component注入事件消息，便于处理逻辑
- 特殊需求的特殊处理，灵活拓展对象的行为
- 不用关心其他模块，降低耦合
- 当前已经支持lua作为脚本嵌入到框架中使用(详见教程6)
- 后期将支持JavaScript，C#，python等脚本语言

### 分布式服务器架构
经过我们参考主流架构以及长期的项目经验积累，Ark被设计为理论上可无限扩展的分布式架构，拥有着真正的无理论上限的系统规模和负载可扩展性。

**主要特性：**
1. 无限扩展
2. 高可靠性
3. 简易通讯
4. 业务去中心化
5. 自动摘除问题节点
6. 节点实时权重运算和评估
7. 负载均衡

服务器架构图：
![Server Architecture](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/docs/_images/server-architecture.png)

## 使用教程
### 教程1:添加一个模块
```cpp
#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

class HelloWorld1
    : public AFILogicModule
{
public:
    HelloWorld1() = default;

    virtual bool Init();
    virtual bool PostInit();

    virtual bool Update();

    virtual bool PreShut();
    virtual bool Shut();

protected:

};
```

```cpp
#include "HelloWorld1.h"

bool HelloWorld1::Init()
{
    //初始化
    std::cout << "Hello, world1, Init" << std::endl;
    return true;
}

bool HelloWorld1::PostInit()
{
    //初始化完毕
    std::cout << "Hello, world1, AfterInit" << std::endl;
    return true;
}

bool HelloWorld1::Update()
{
    //每帧执行
    //std::cout << "Hello, world1, Execute" << std::endl;
    return true;
}

bool HelloWorld1::PreShut()
{
    //反初始化之前
    std::cout << "Hello, world1, BeforeShut" << std::endl;
    return true;
}

bool HelloWorld1::Shut()
{
    //反初始化
    std::cout << "Hello, world1, Shut" << std::endl;
    return true;
}

```

### 教程2:数据驱动
### 教程3:事件系统
# 结束
