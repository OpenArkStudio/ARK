# Kernel plugin

该插件是`ARK框架`中的核心插件, 主要用来管理Entity对象和进程配置数据, 其中主要包含四个模块,

- AFCClassModule 
- AFCElementModule
- AFCSceneModule
- AFCKernelModule

该四个插件对应的功能各不相同, 却各有关联，下面详细讲解各个模块的功能.

## AFCClassModule

主要负责ARK中Class的管理, 这里的Class类似Entity, 区别于Entity的是, Class是通过配置出来的, 而Entity是在运行过程中动态创建出来的, 其余功能和Entity类似.
ClassModule通过加载`DataConfig/Struct/LogicClass.xml`来获得ClassName, InstancePath(Class的结构文件路径), ResPath(Class的配置文件路径). 有些动态创建出来的Entity需要通过Class来提供结构样式, 这种Class的Res文件是没有配置的, 因为都是动态创建过程中赋值. 同时也提供ClassEvent和ClassCallback, 当该Class对应的Entity创建时, 可以对应处理事件.

下面举例说明如何使用ClassCallback

```cpp
bool AFCPropertyModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pPropertyConfigModule = pPluginManager->FindModule<AFIPropertyConfigModule>();
    m_pLevelModule = pPluginManager->FindModule<AFILevelModule>();

    //创建Player的ClassCallback
    //根据模块可以自由添加, 一个Class可以有多个ClassCallback
    m_pKernelModule->AddClassCallBack(ARK::Player::ThisName(), this, &AFCPropertyModule::OnObjectClassEvent);

    return true;
}
```

而AFCPropertyModule::OnObjectClassEvent中可以根据ClassEvent处理不同的业务逻辑，做到解耦(*注意:AddClassCallBack要在该Class对应的Entity创建前调用*)

## AFCElementModule

主要管理配置类数据, 通过上面的ClassModule获得Instance和ResPath后，对这两个分别加载进内存，形成配置文件，后续就可以通过下面接口获取配置内容了

```cpp
virtual bool ExistElement(const std::string& strConfigName);
virtual bool ExistElement(const std::string& strClassName, const std::string& strConfigName);

virtual bool GetNodeBool(const std::string& strConfigName, const std::string& strDataNodeName);
virtual int32_t GetNodeInt(const std::string& strConfigName, const std::string& strDataNodeName);
virtual int64_t GetNodeInt64(const std::string& strConfigName, const std::string& strDataNodeName);
virtual float GetNodeFloat(const std::string& strConfigName, const std::string& strDataNodeName);
virtual double GetNodeDouble(const std::string& strConfigName, const std::string& strDataNodeName);
virtual const char* GetNodeString(const std::string& strConfigName, const std::string& strDataNodeName);
```

## AFCSceneModule
用来创建场景和管理场景中的Entity列表(*为了更加灵活, 当前是按照地图全图广播的, 如有RPG项目的需求，请自行添加RPG中的九宫格/十字链表等视野管理*), 可以获取场景中的Entity数据, 从而进行数据广播等等.
*因暂时ARK框架中并无实际逻辑，所以此模块暂时并没有特别多的用处.*

## AFCKernelModule
主要用来作为Entity数据管理, 是本插件的核心模块，几乎所有的接口都是来自此模块, 主要用途为获取Entity对象, 获取/修改Entity的Node和Table数据, 同时可以注册公共的ClassEvent, DataNodeEvent, DataTableEvent(公共事件并不局限于某个Class/Entity)

公共注册接口如下:
```cpp
template<typename BaseType>
bool RegCommonClassEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const ARK_ENTITY_EVENT, const AFIDataList&))
{
    CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    return RegCommonClassEvent(std::make_shared<CLASS_EVENT_FUNCTOR>(functor));
}

template<typename BaseType>
bool RegCommonDataNodeEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
{
    DATA_NODE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    return RegCommonDataNodeEvent(std::make_shared<DATA_NODE_EVENT_FUNCTOR>(functor));
}

template<typename BaseType>
bool RegCommonDataTableEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
{
    DATA_TABLE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    return RegCommonDataTableEvent(std::make_shared<DATA_TABLE_EVENT_FUNCTOR>(functor));
}
```

数据获取接口如下:
```cpp
virtual ARK_SHARE_PTR<AFIEntity> GetEntity(const AFGUID& ident) = 0;
virtual ARK_SHARE_PTR<AFIEntity> CreateEntity(const AFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const AFIDataList& arg) = 0;


virtual bool FindNode(const AFGUID& self, const std::string& name) = 0;

virtual bool SetNodeBool(const AFGUID& self, const std::string& name, const bool value) = 0;
virtual bool SetNodeInt(const AFGUID& self, const std::string& name, const int32_t value) = 0;
virtual bool SetNodeInt64(const AFGUID& self, const std::string& name, const int64_t value) = 0;
virtual bool SetNodeFloat(const AFGUID& self, const std::string& name, const float value) = 0;
virtual bool SetNodeDouble(const AFGUID& self, const std::string& name, const double value) = 0;
virtual bool SetNodeString(const AFGUID& self, const std::string& name, const std::string& value) = 0;
virtual bool SetNodeObject(const AFGUID& self, const std::string& name, const AFGUID& value) = 0;

virtual bool GetNodeBool(const AFGUID& self, const std::string& name) = 0;
virtual int32_t GetNodeInt(const AFGUID& self, const std::string& name) = 0;
virtual int64_t GetNodeInt64(const AFGUID& self, const std::string& name) = 0;
virtual float GetNodeFloat(const AFGUID& self, const std::string& name) = 0;
virtual double GetNodeDouble(const AFGUID& self, const std::string& name) = 0;
virtual const char*  GetNodeString(const AFGUID& self, const std::string& name) = 0;
virtual const AFGUID GetNodeObject(const AFGUID& self, const std::string& name) = 0;
//////////////////////////////////////////////////////////////////////////
virtual AFDataTable* FindTable(const AFGUID& self, const std::string& name) = 0;
virtual bool ClearTable(const AFGUID& self, const std::string& name) = 0;

virtual bool SetTableBool(const AFGUID& self, const std::string& name, const int row, const int col, const bool value) = 0;
virtual bool SetTableInt(const AFGUID& self, const std::string& name, const int row, const int col, const int32_t value) = 0;
virtual bool SetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col, const int64_t value) = 0;
virtual bool SetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col, const float value) = 0;
virtual bool SetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col, const double value) = 0;
virtual bool SetTableString(const AFGUID& self, const std::string& name, const int row, const int col, const std::string& value) = 0;
virtual bool SetTableObject(const AFGUID& self, const std::string& name, const int row, const int col, const AFGUID& value) = 0;

virtual bool GetTableBool(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
virtual int32_t GetTableInt(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
virtual int64_t GetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
virtual float GetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
virtual double GetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
virtual const char* GetTableString(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
virtual const AFGUID GetTableObject(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
```

举个数据获取的例子:
```cpp
int AFCLevelModule::AddExp(const AFGUID& self, const int nExp)
{
    //通过DataNode名称获取ID为self的Entity数据
    int eJobType = m_pKernelModule->GetNodeInt(self, ARK::Player::Job());
    int nCurExp = m_pKernelModule->GetNodeInt(self, ARK::Player::EXP());
    int nLevel = m_pKernelModule->GetNodeInt(self, ARK::Player::Level());
    int nMaxExp = m_pPropertyConfigModule->CalculateBaseValue(eJobType, nLevel, ARK::Player::MAXEXP());

    //Do something

    return 0;
}
```