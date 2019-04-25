/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
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

#include "base/AFMap.hpp"
#include "base/AFArrayMap.hpp"
#include "base/AFCDataList.hpp"
#include "base/AFDataTable.hpp"
#include "interface/AFIEntity.h"
#include "interface/AFIGUIDModule.h"
#include "interface/AFILogModule.h"
#include "interface/AFIKernelModule.h"
#include "interface/AFIMetaClassModule.h"
#include "interface/AFIConfigModule.h"
#include "interface/AFIMapModule.h"

namespace ark
{

    class AFCKernelModule : public AFIKernelModule
    {
    public:
        explicit AFCKernelModule();
        ~AFCKernelModule() override;

        bool Init() override;
        bool Update() override;
        bool PreShut() override;

        ///////////////////////////////////////////////////////////////////////
        ARK_SHARE_PTR<AFIEntity> GetEntity(const AFGUID& self) override;
        ARK_SHARE_PTR<AFIEntity> CreateEntity(const AFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const AFIDataList& arg) override;

        bool DestroyAll() override;
        bool DestroyEntity(const AFGUID& self) override;

        //////////////////////////////////////////////////////////////////////////
        bool FindNode(const AFGUID& self, const std::string& name) override;

        bool SetNodeBool(const AFGUID& self, const std::string& name, const bool value) override;
        bool SetNodeInt(const AFGUID& self, const std::string& name, const int32_t value) override;
        bool SetNodeInt64(const AFGUID& self, const std::string& name, const int64_t value) override;
        bool SetNodeFloat(const AFGUID& self, const std::string& name, const float value) override;
        bool SetNodeDouble(const AFGUID& self, const std::string& name, const double value) override;
        bool SetNodeString(const AFGUID& self, const std::string& name, const std::string& value) override;

        bool GetNodeBool(const AFGUID& self, const std::string& name) override;
        int32_t GetNodeInt(const AFGUID& self, const std::string& name) override;
        int64_t GetNodeInt64(const AFGUID& self, const std::string& name) override;
        float GetNodeFloat(const AFGUID& self, const std::string& name) override;
        double GetNodeDouble(const AFGUID& self, const std::string& name) override;
        const char* GetNodeString(const AFGUID& self, const std::string& name) override;
        //////////////////////////////////////////////////////////////////////////
        AFDataTable* FindTable(const AFGUID& self, const std::string& name) override;
        bool ClearTable(const AFGUID& self, const std::string& name) override;

        bool SetTableBool(const AFGUID& self, const std::string& name, const int row, const int col, const bool value) override;
        bool SetTableInt(const AFGUID& self, const std::string& name, const int row, const int col, const int32_t value) override;
        bool SetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col, const int64_t value) override;
        bool SetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col, const float value) override;
        bool SetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col, const double value) override;
        bool SetTableString(const AFGUID& self, const std::string& name, const int row, const int col, const std::string& value) override;

        bool GetTableBool(const AFGUID& self, const std::string& name, const int row, const int col) override;
        int32_t GetTableInt(const AFGUID& self, const std::string& name, const int row, const int col) override;
        int64_t GetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col) override;
        float GetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col) override;
        double GetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col) override;
        const char* GetTableString(const AFGUID& self, const std::string& name, const int row, const int col) override;

        //////////////////////////////////////////////////////////////////////////
        bool LogInfo(const AFGUID& id) override;
        bool LogSelfInfo(const AFGUID& id);
        //////////////////////////////////////////////////////////////////////////

        bool DoEvent(const AFGUID& self, const std::string& class_name, ArkEntityEvent class_event, const AFIDataList& args) override;
        bool DoEvent(const AFGUID& self, const int event_id, const AFIDataList& args) override;

    protected:
        bool DestroySelf(const AFGUID& self);

        bool RegCommonClassEvent(const CLASS_EVENT_FUNCTOR_PTR& cb) override;
        bool RegCommonDataNodeEvent(const DATA_NODE_EVENT_FUNCTOR_PTR& cb) override;
        bool RegCommonDataTableEvent(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) override;

        bool AddEventCallBack(const AFGUID& self, const int event_id, const EVENT_PROCESS_FUNCTOR_PTR& cb) override;
        bool AddClassCallBack(const std::string& class_name, const CLASS_EVENT_FUNCTOR_PTR& cb) override;

    private:
        std::list<AFGUID> delete_list_;
        //////////////////////////////////////////////////////////////////////////
        std::list<CLASS_EVENT_FUNCTOR_PTR> common_class_callbacks_;
        std::list<DATA_NODE_EVENT_FUNCTOR_PTR> common_data_node_callbacks_;
        std::list<DATA_TABLE_EVENT_FUNCTOR_PTR> common_data_table_callbacks_;

        AFGUID cur_exec_entity_;

        AFIMapModule* m_pMapModule = nullptr;
        AFILogModule* m_pLogModule = nullptr;
        AFIMetaClassModule* m_pClassModule = nullptr;
        AFIConfigModule* m_pConfigModule = nullptr;
        AFIGUIDModule* m_pGUIDModule = nullptr;

        AFArrayMap<std::string, int32_t> inner_nodes_;
        AFMapEx<AFGUID, AFIEntity> entities_;
    };

}