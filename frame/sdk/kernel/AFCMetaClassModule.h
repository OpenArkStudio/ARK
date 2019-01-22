/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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
#include "base/AFList.hpp"
#include "base/AFCDataList.hpp"
#include "base/AFCDataNodeManager.hpp"
#include "base/AFCDataTableManager.hpp"
#include "interface/AFIMetaClassModule.h"
#include "interface/AFIConfigModule.h"
#include "interface/AFIPluginManager.h"
#include "interface/AFILogModule.h"
#include "base/AFDataNode.hpp"

namespace ark
{

    class AFCClass : public AFIMetaClass
    {
    public:
        explicit AFCClass(const std::string& strClassName) :
            class_name_(strClassName)
        {
            m_pNodeManager = std::make_shared<AFCDataNodeManager>(NULL_GUID);
            m_pTableManager = std::make_shared<AFCDataTableManager>(NULL_GUID);
        }

        virtual ~AFCClass()
        {
            for (size_t i = 0; i < node_callbacks_.size(); ++i)
            {
                delete node_callbacks_[i];
            }

            node_callbacks_.clear();
            callback_indices_.Clear();

            for (size_t i = 0; i < table_callbacks_.GetCount(); ++i)
            {
                delete table_callbacks_[i];
            }

            table_callbacks_.Clear();
            ClearAll();
        }

        ARK_SHARE_PTR<AFIDataNodeManager>& GetNodeManager() override
        {
            return m_pNodeManager;
        }

        ARK_SHARE_PTR<AFIDataTableManager>& GetTableManager() override
        {
            return m_pTableManager;
        }

        bool AddClassCallBack(const CLASS_EVENT_FUNCTOR_PTR& cb) override
        {
            return class_events_.Add(cb);
        }

        bool DoEvent(const AFGUID& id, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& valueList) override
        {
            CLASS_EVENT_FUNCTOR_PTR cb;
            for (bool bRet = class_events_.First(cb); bRet; bRet = class_events_.Next(cb))
            {
                (*cb)(id, class_name_, eClassEvent, valueList);
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

            if (callback_indices_.GetData(name.c_str(), indexCallback))
            {
                node_callbacks_[indexCallback]->callbacks_.push_back(cb);
            }
            else
            {
                AFNodeCallBack* pNodeCB = ARK_NEW AFNodeCallBack();
                pNodeCB->callbacks_.push_back(cb);
                callback_indices_.Add(name.c_str(), node_callbacks_.size());
                node_callbacks_.push_back(pNodeCB);
            }

            return true;
        }

        bool AddCommonNodeCallback(const DATA_NODE_EVENT_FUNCTOR_PTR& cb) override
        {
            common_node_callbacks_.push_back(cb);
            return true;
        }

        bool AddTableCallBack(const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) override
        {
            AFTableCallBack* pCallBackList = table_callbacks_.GetElement(name);

            if (!pCallBackList)
            {
                pCallBackList = new AFTableCallBack();
                table_callbacks_.AddElement(name, pCallBackList);
            }

            pCallBackList->callbacks_.push_back(cb);

            return true;
        }

        int OnNodeCallback(const AFGUID& self, const std::string& name, const AFIData& old_data, const AFIData& new_data)
        {
            size_t indexCallBack(0);
            if (!callback_indices_.GetData(name.c_str(), indexCallBack))
            {
                return false;
            }

            for (auto& iter : common_node_callbacks_)
            {
                (*iter)(self, name, old_data, new_data);
            }

            for (auto& iter : node_callbacks_[indexCallBack]->callbacks_)
            {
                (*iter)(self, name, old_data, new_data);
            }

            return true;
        }

        bool InitDataNodeManager(ARK_SHARE_PTR<AFIDataNodeManager>& pNodeManager) override
        {
            ARK_SHARE_PTR<AFIDataNodeManager>& pStaticClassNodeManager = GetNodeManager();
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
            common_table_callbacks_.push_back(cb);
            return true;
        }

        bool InitDataTableManager(ARK_SHARE_PTR<AFIDataTableManager>& pTableManager) override
        {
            ARK_SHARE_PTR<AFIDataTableManager>& pStaticClassTableManager = GetTableManager();
            if (pStaticClassTableManager == nullptr)
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

        int OnEventHandler(const AFGUID& id, const DATA_TABLE_EVENT_DATA& event_data, const AFIData& old_data, const AFIData& new_data)
        {
            for (auto& iter : common_table_callbacks_)
            {
                (*iter)(id, event_data, old_data, new_data);
            }

            AFTableCallBack* pTableCallBack = table_callbacks_.GetElement(event_data.strName.c_str());

            if (nullptr != pTableCallBack)
            {
                for (auto& iter : pTableCallBack->callbacks_)
                {
                    (*iter)(id, event_data, old_data, new_data);
                }
            }

            return 0;
        }

        void SetParent(ARK_SHARE_PTR<AFIMetaClass>& pClass) override
        {
            m_pParentClass = pClass;
        }

        ARK_SHARE_PTR<AFIMetaClass>& GetParent() override
        {
            return m_pParentClass;
        }

        void SetTypeName(const char* type) override
        {
            type_name_ = type;
        }

        const std::string& GetTypeName() override
        {
            return type_name_;
        }

        const std::string& GetClassName() override
        {
            return class_name_;
        }

        bool AddConfigName(std::string& config_name) override
        {
            return config_list_.Add(config_name);
        }

        AFList<std::string>& GetConfigNameList() override
        {
            return config_list_;
        }

        void SetResPath(const std::string& path) override
        {
            class_res_path_ = path;
        }

        const std::string& GetResPath() override
        {
            return class_res_path_;
        }

    private:
        using NodeCallbacks = std::vector<DATA_NODE_EVENT_FUNCTOR_PTR>;
        struct  AFNodeCallBack
        {
            NodeCallbacks callbacks_;
        };

        using TableCallbacks = std::vector<DATA_TABLE_EVENT_FUNCTOR_PTR>;
        struct  AFTableCallBack
        {
            TableCallbacks callbacks_;
        };

        ARK_SHARE_PTR<AFIDataNodeManager> m_pNodeManager;
        ARK_SHARE_PTR<AFIDataTableManager> m_pTableManager;

        ARK_SHARE_PTR<AFIMetaClass> m_pParentClass{ nullptr };
        std::string type_name_{};
        std::string class_name_{};
        std::string class_res_path_{};

        AFList<std::string> config_list_;
        AFList<CLASS_EVENT_FUNCTOR_PTR> class_events_;
        ArrayPod<AFNodeCallBack*, 1, CoreAlloc> node_callbacks_;
        StringPod<char, size_t, StringTraits<char>, CoreAlloc> callback_indices_;
        NodeCallbacks common_node_callbacks_;

        AFArrayMap<std::string, AFTableCallBack> table_callbacks_;
        TableCallbacks common_table_callbacks_;
    };

    class AFCMetaClassModule : public AFIMetaClassModule
    {
    public:
        explicit AFCMetaClassModule() = default;
        ~AFCMetaClassModule() override;

        bool Init() override;
        bool Shut() override;

        bool Load() override;
        bool Clear() override;

        bool AddClassCallBack(const std::string& class_name, const CLASS_EVENT_FUNCTOR_PTR& cb) override;
        bool DoEvent(const AFGUID& id, const std::string& class_name, const ARK_ENTITY_EVENT class_event, const AFIDataList& args) override;
        bool AddNodeCallBack(const std::string& class_name, const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb) override;
        bool AddTableCallBack(const std::string& class_name, const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) override;
        bool AddCommonNodeCallback(const std::string& class_name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb) override;
        bool AddCommonTableCallback(const std::string& class_name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) override;

        ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager(const std::string& class_name) override;
        ARK_SHARE_PTR<AFIDataTableManager> GetTableManager(const std::string& class_name) override;
        bool InitDataNodeManager(const std::string& class_name, ARK_SHARE_PTR<AFIDataNodeManager>& pNodeManager) override;
        bool InitDataTableManager(const std::string& class_name, ARK_SHARE_PTR<AFIDataTableManager>& pTableManager) override;
        bool AddClass(const std::string& class_name, const std::string& parent_name);

    protected:
        int ComputerType(const char* type_name, AFIData& var);
        bool AddNodes(rapidxml::xml_node<>* pNodeRootNode, ARK_SHARE_PTR<AFIMetaClass>& pClass);
        bool AddTables(rapidxml::xml_node<>* pTableRootNode, ARK_SHARE_PTR<AFIMetaClass>& pClass);
        bool AddComponents(rapidxml::xml_node<>* pComponentRootNode, ARK_SHARE_PTR<AFIMetaClass>& pClass);
        bool AddClassInclude(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIMetaClass>& pClass);
        bool AddClass(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIMetaClass>& pClass);

        bool Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIMetaClass> pParentClass);

    private:
        AFIConfigModule* m_pConfigModule;
        AFILogModule* m_pLogModule;
        std::string schema_file_{};
    };

}