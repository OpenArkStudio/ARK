/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "kernel/interface/AFIKernelModule.hpp"
#include "base/AFMap.hpp"
#include "base/AFArrayMap.hpp"
#include "base/AFPluginManager.hpp"
#include "utility/interface/AFIGUIDModule.hpp"
#include "log/interface/AFILogModule.hpp"
#include "kernel/include/AFCDataList.hpp"
#include "kernel/interface/AFIMapModule.hpp"
#include "kernel/interface/AFIClassMetaModule.hpp"
#include "kernel/interface/AFIConfigModule.hpp"
#include "kernel/interface/AFIEntity.hpp"

namespace ark {

class AFCKernelModule : public AFIKernelModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit AFCKernelModule();
    ~AFCKernelModule() override;

    bool Init() override;
    bool Update() override;
    bool PreShut() override;

    ///////////////////////////////////////////////////////////////////////
    ARK_SHARE_PTR<AFIEntity> GetEntity(const AFGUID& self) override;
    ARK_SHARE_PTR<AFIEntity> CreateEntity(const AFGUID& self, const int nSceneID, const int nGroupID,
        const std::string& strClassName, const ID_TYPE config_id, const AFIDataList& arg) override;

    ARK_SHARE_PTR<AFIEntity> CreateContainerEntity(const AFGUID& self, const uint32_t container_index,
        const std::string& class_name, const ID_TYPE config_id) override;

    bool DestroyAll() override;
    bool DestroyEntity(const AFGUID& self) override;

    //////////////////////////////////////////////////////////////////////////
    bool LogInfo(const AFGUID& id) override;
    bool LogSelfInfo(const AFGUID& id);
    //////////////////////////////////////////////////////////////////////////

    bool DoEvent(const AFGUID& self, const std::string& class_name, ArkEntityEvent class_event,
        const AFIDataList& args) override;
    bool DoEvent(const AFGUID& self, const int event_id, const AFIDataList& args) override;

    bool Exist(const AFGUID& self) override;

    bool EntityToDBData(const AFGUID& self, AFMsg::pb_db_entity& pb_data) override;
    ARK_SHARE_PTR<AFIEntity> CreateEntity(const AFMsg::pb_db_entity& pb_data) override;

    // send message
    bool SendCustomMessage(const AFGUID& target, const uint32_t msg_id, const AFIDataList& args) override;

protected:
    bool DestroySelf(const AFGUID& self);

    bool AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb) override;
    bool AddEventCallBack(const AFGUID& self, const int event_id, EVENT_PROCESS_FUNCTOR&& cb) override;
    bool AddDataCallBack(const std::string& class_name, const std::string& name, DATA_NODE_EVENT_FUNCTOR&& cb) override;
    bool AddTableCallBack(const std::string& class_name, const std::string& name, DATA_TABLE_EVENT_FUNCTOR&& cb) override;

    bool AddDataCallBack(const std::string& class_name, const uint32_t index, DATA_NODE_EVENT_FUNCTOR&& cb) override;
    bool AddTableCallBack(const std::string& class_name, const uint32_t index, DATA_TABLE_EVENT_FUNCTOR&& cb) override;

    bool AddCommonClassEvent(CLASS_EVENT_FUNCTOR&& cb) override;
    bool AddCommonNodeEvent(DATA_NODE_EVENT_FUNCTOR&& cb) override;
    bool AddCommonTableEvent(DATA_TABLE_EVENT_FUNCTOR&& cb) override;

    // convert db data and entity
    bool EntityToDBData(ARK_SHARE_PTR<AFIEntity> pEntity, AFMsg::pb_db_entity& pb_data);
    bool NodeToDBData(AFINode* pNode, AFMsg::pb_db_entity_data& pb_data);
    bool TableToDBData(AFITable* pTable, AFMsg::pb_db_table& pb_data);

    template<typename T>
    bool DBDataToNode(T pData, const AFMsg::pb_db_entity_data& pb_db_entity_data);
    bool DBDataToTable(
        ARK_SHARE_PTR<AFIEntity> pEntityData, const std::string& name, const AFMsg::pb_db_table& pb_table);
    bool DBDataToContainer(
        ARK_SHARE_PTR<AFIEntity> pEntity, const std::string& name, const AFMsg::pb_db_container& pb_data);

private:
    std::list<AFGUID> delete_list_;

    AFGUID cur_exec_object_;

    AFIMapModule* m_pMapModule{nullptr};
    AFILogModule* m_pLogModule{nullptr};
    AFIGUIDModule* m_pGUIDModule{nullptr};
    AFIConfigModule* m_pConfigModule{nullptr};
    AFIClassMetaModule* m_pClassModule{nullptr};

    AFArrayMap<std::string, int32_t> inner_nodes_;
    AFSmartPtrMap<AFGUID, AFIEntity> objects_;
};

} // namespace ark
