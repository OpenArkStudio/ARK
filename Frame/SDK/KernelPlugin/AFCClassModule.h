/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#pragma once

#include "rapidxml/rapidxml.hpp"
#include "SDK/Core/AFList.hpp"
#include "SDK/Core/AFCDataList.hpp"
#include "SDK/Core/AFCDataNodeManager.h"
#include "SDK/Core/AFCDataTableManager.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIConfigModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Core/AFDataNode.hpp"

class AFCClass : public AFIClass
{
public:
    explicit AFCClass(const std::string& strClassName) :
        mstrClassName(strClassName)
    {
        m_pNodeManager = std::make_shared<AFCDataNodeManager>(NULL_GUID);
        m_pTableManager = std::make_shared<AFCDataTableManager>(NULL_GUID);
    }

    virtual ~AFCClass()
    {
        for (size_t i = 0; i < mxNodeCBs.size(); ++i)
        {
            delete mxNodeCBs[i];
        }

        mxNodeCBs.clear();
        mxCallBackIndices.Clear();

        for (size_t i = 0; i < mxTableCallbacks.GetCount(); ++i)
        {
            delete mxTableCallbacks[i];
        }

        mxTableCallbacks.Clear();
        ClearAll();
    }

    ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager() override
    {
        return m_pNodeManager;
    }

    ARK_SHARE_PTR<AFIDataTableManager> GetTableManager() override
    {
        return m_pTableManager;
    }

    bool AddClassCallBack(const CLASS_EVENT_FUNCTOR_PTR& cb) override
    {
        return mxClassEventInfo.Add(cb);
    }

    bool DoEvent(const AFGUID& objectID, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& valueList) override
    {
        CLASS_EVENT_FUNCTOR_PTR cb;
        for (bool bRet = mxClassEventInfo.First(cb); bRet; bRet = mxClassEventInfo.Next(cb))
        {
            (*cb)(objectID, mstrClassName, eClassEvent,  valueList);
        }

        return true;
    }

    bool AddNodeCallBack(const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb) override
    {
        size_t index(0);

        if (!GetNodeManager()->GetNode(name.c_str()))
        {
            return false;
        }

        size_t indexCallback{ 0 };

        if (mxCallBackIndices.GetData(name.c_str(), indexCallback))
        {
            mxNodeCBs[indexCallback]->mxCallBackList.push_back(cb);
        }
        else
        {
            AFNodeCallBack* pNodeCB = new AFNodeCallBack();
            pNodeCB->mxCallBackList.push_back(cb);
            mxCallBackIndices.Add(name.c_str(), mxNodeCBs.size());
            mxNodeCBs.push_back(pNodeCB);
        }

        return true;
    }

    bool AddCommonNodeCallback(const DATA_NODE_EVENT_FUNCTOR_PTR& cb) override
    {
        mxCommonCallBackList.push_back(cb);
        return true;
    }

    bool AddTableCallBack(const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) override
    {
        AFTableCallBack* pCallBackList = mxTableCallbacks.GetElement(name);

        if (!pCallBackList)
        {
            pCallBackList = new AFTableCallBack();
            mxTableCallbacks.AddElement(name, pCallBackList);
        }

        pCallBackList->mxCallbackList.push_back(cb);

        return true;
    }

    int OnNodeCallback(const AFGUID& self, const std::string& name, const AFIData& oldData, const AFIData& newData)
    {
        size_t indexCallBack(0);
        if (!mxCallBackIndices.GetData(name.c_str(), indexCallBack))
        {
            return false;
        }

        for (auto& iter : mxCommonCallBackList)
        {
            (*iter)(self, name, oldData, newData);
        }

        for (auto& iter : mxNodeCBs[indexCallBack]->mxCallBackList)
        {
            (*iter)(self, name, oldData, newData);
        }

        return true;
    }

    bool InitDataNodeManager(ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager) override
    {
        ARK_SHARE_PTR<AFIDataNodeManager> pStaticClassNodeManager = GetNodeManager();
        if (!pStaticClassNodeManager)
        {
            return false;
        }

        size_t staticNodeCount = pStaticClassNodeManager->GetNodeCount();
        for (size_t i = 0; i < staticNodeCount; ++i)
        {
            AFDataNode* pStaticConfigNode = pStaticClassNodeManager->GetNodeByIndex(i);

            if (pStaticConfigNode == nullptr)
            {
                continue;
            }

            bool bRet = pNodeManager->AddNode(pStaticConfigNode->GetName(), pStaticConfigNode->GetValue(), pStaticConfigNode->GetFeature());

            if (!bRet)
            {
                ARK_ASSERT_NO_EFFECT(0);
            }
        }

        pNodeManager->RegisterCallback(this, &AFCClass::OnNodeCallback);
        return true;
    }

    bool AddCommonTableCallback(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) override
    {
        mxTableCommonCallbacks.push_back(cb);
        return true;
    }

    bool InitDataTableManager(ARK_SHARE_PTR<AFIDataTableManager> pTableManager) override
    {
        ARK_SHARE_PTR<AFIDataTableManager> pStaticClassTableManager = GetTableManager();
        if (!pStaticClassTableManager)
        {
            return false;
        }

        size_t staticTableCount = pStaticClassTableManager->GetCount();

        for (size_t i = 0; i < staticTableCount; ++i)
        {
            AFDataTable* pStaticTable = pStaticClassTableManager->GetTableByIndex(i);

            if (pStaticTable == nullptr)
            {
                continue;
            }

            AFCDataList col_type_list;
            pStaticTable->GetColTypeList(col_type_list);
            pTableManager->AddTable(NULL_GUID, pStaticTable->GetName(), col_type_list, pStaticTable->GetFeature());
        }

        pTableManager->RegisterCallback(this, &AFCClass::OnEventHandler);
        return true;
    }

    int OnEventHandler(const AFGUID& entity_id, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldData, const AFIData& newData)
    {
        for (auto& iter : mxTableCommonCallbacks)
        {
            (*iter)(entity_id, xEventData, oldData, newData);
        }

        AFTableCallBack* pTableCallBack = mxTableCallbacks.GetElement(xEventData.strName.c_str());

        if (nullptr != pTableCallBack)
        {
            for (auto& iter : pTableCallBack->mxCallbackList)
            {
                (*iter)(entity_id, xEventData, oldData, newData);
            }
        }

        return 0;
    }

    void SetParent(ARK_SHARE_PTR<AFIClass> pClass) override
    {
        m_pParentClass = pClass;
    }

    ARK_SHARE_PTR<AFIClass> GetParent() override
    {
        return m_pParentClass;
    }

    void SetTypeName(const char* strType) override
    {
        mstrType = strType;
    }

    const std::string& GetTypeName() override
    {
        return mstrType;
    }

    const std::string& GetClassName() override
    {
        return mstrClassName;
    }

    bool AddConfigName(std::string& strConfigName) override
    {
        return mxConfigList.Add(strConfigName);
    }

    AFList<std::string>& GetConfigNameList() override
    {
        return mxConfigList;
    }

    void SetResPath(const std::string& strPath) override
    {
        mstrClassResPath = strPath;
    }

    const std::string& GetResPath() override
    {
        return mstrClassResPath;
    }

private:
    struct  AFNodeCallBack
    {
        std::vector<DATA_NODE_EVENT_FUNCTOR_PTR> mxCallBackList;
    };

    using TableCallbacks = std::vector<DATA_TABLE_EVENT_FUNCTOR_PTR>;
    struct  AFTableCallBack
    {
        TableCallbacks mxCallbackList;
    };

    ARK_SHARE_PTR<AFIDataNodeManager> m_pNodeManager;
    ARK_SHARE_PTR<AFIDataTableManager> m_pTableManager;

    ARK_SHARE_PTR<AFIClass> m_pParentClass{nullptr};
    std::string mstrType{};
    std::string mstrClassName{};
    std::string mstrClassResPath{};

    AFList<std::string> mxConfigList;
    AFList<CLASS_EVENT_FUNCTOR_PTR> mxClassEventInfo;
    ArrayPod<AFNodeCallBack*, 1, CoreAlloc> mxNodeCBs;
    StringPod<char, size_t, StringTraits<char>, CoreAlloc> mxCallBackIndices;
    std::vector<DATA_NODE_EVENT_FUNCTOR_PTR> mxCommonCallBackList;

    AFArrayMap<std::string, AFTableCallBack> mxTableCallbacks;
    TableCallbacks mxTableCommonCallbacks;
};

class AFCClassModule : public AFIClassModule
{
public:
    explicit AFCClassModule() = default;
    ~AFCClassModule() override;

    bool Init() override;
    bool Shut() override;

    bool Load() override;
    bool Clear() override;

    bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb) override;
    bool DoEvent(const AFGUID& objectID, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& valueList) override;
    bool AddNodeCallBack(const std::string& strClassName, const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb) override;
    bool AddTableCallBack(const std::string& strClassName, const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) override;
    bool AddCommonNodeCallback(const std::string& strClassName, const DATA_NODE_EVENT_FUNCTOR_PTR& cb) override;
    bool AddCommonTableCallback(const std::string& strClassName, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) override;

    ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager(const std::string& strClassName) override;
    ARK_SHARE_PTR<AFIDataTableManager> GetTableManager(const std::string& strClassName) override;
    bool InitDataNodeManager(const std::string& strClassName, ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager) override;
    bool InitDataTableManager(const std::string& strClassName, ARK_SHARE_PTR<AFIDataTableManager> pTableManager) override;
    bool AddClass(const std::string& strClassName, const std::string& strParentName);

protected:
    virtual int ComputerType(const char* pstrTypeName, AFIData& var);
    virtual bool AddNodes(rapidxml::xml_node<>* pNodeRootNode, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddTables(rapidxml::xml_node<>* pTableRootNode, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddComponents(rapidxml::xml_node<>* pComponentRootNode, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddClassInclude(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddClass(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIClass> pClass);

    virtual bool Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIClass> pParentClass);

private:
    AFIConfigModule* m_pConfigModule;
    AFILogModule* m_pLogModule;
    std::string mstrSchemaFile{};
};