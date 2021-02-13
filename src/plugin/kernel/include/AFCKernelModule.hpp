/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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
#include "base/container/AFMap.hpp"
#include "base/container/AFArrayMap.hpp"
#include "base/AFPluginManager.hpp"
#include "utility/interface/AFIGUIDModule.hpp"
#include "kernel/interface/AFIDataList.hpp"
#include "kernel/interface/AFIMapModule.hpp"
#include "kernel/interface/AFIClassMetaModule.hpp"
#include "kernel/interface/AFIConfigModule.hpp"
#include "kernel/interface/AFIEntity.hpp"
#include "AFNodeManager.hpp"
#include "AFTableManager.hpp"
#include "kernel/interface/AFIContainerManager.hpp"
#include "kernel/interface/AFIEventManager.hpp"

namespace ark {

class AFCKernelModule final : public AFIKernelModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit AFCKernelModule();
    ~AFCKernelModule() override;

    bool Init() override;
    bool Update() override;
    bool PreShut() override;

    ///////////////////////////////////////////////////////////////////////
    std::shared_ptr<AFIStaticEntity> GetStaticEntity(const ID_TYPE config_id) override;
    std::shared_ptr<AFIEntity> GetEntity(const guid_t& self) override;
    std::shared_ptr<AFIEntity> CreateEntity(const guid_t& self, const int nSceneID, const int nGroupID,
        const std::string& strClassName, const ID_TYPE config_id, const AFIDataList& arg) override;

    std::shared_ptr<AFIEntity> CreateContainerEntity(const guid_t& self, const uint32_t container_index,
        const std::string& class_name, const ID_TYPE config_id) override;

    bool DestroyAll() override;
    bool DestroyEntity(const guid_t& self) override;

    //////////////////////////////////////////////////////////////////////////
    bool LogInfo(const guid_t& id) override;
    bool LogSelfInfo(const guid_t& id);
    int LogObjectData(const guid_t& guid) override;
    //////////////////////////////////////////////////////////////////////////

    bool DoEvent(const guid_t& self, const std::string& class_name, ArkEntityEvent class_event,
        const AFIDataList& args) override;
    bool DoEvent(const guid_t& self, const int event_id, const AFIDataList& args) override;

    bool Exist(const guid_t& self) override;

    bool EntityToDBData(const guid_t& self, AFMsg::pb_db_entity& pb_data) override;
    std::shared_ptr<AFIEntity> CreateEntity(const AFMsg::pb_db_entity& pb_data) override;

    // send message
    bool SendCustomMessage(const guid_t& target, const uint32_t msg_id, const AFIDataList& args) override;

    // entity to pb
    bool NodeToPBData(const uint32_t index, const AFIData& data, AFMsg::pb_entity_data* pb_data) override;
    bool RowToPBData(AFIRow* pRow, const uint32_t index, AFMsg::pb_entity_data* pb_data) override;
    bool TableRowDataToPBData(const uint32_t index, uint32_t row, const uint32_t col, const AFIData& data,
        AFMsg::pb_entity_data* pb_data) override;
    bool EntityNodeToPBData(
        std::shared_ptr<AFIEntity> pEntity, AFMsg::pb_entity_data* pb_data, const ArkMaskType mask = 0) override;
    bool EntityTableToPBData(
        std::shared_ptr<AFIEntity> pEntity, AFMsg::pb_entity_data* pb_data, const ArkMaskType mask = 0) override;

protected:
    bool DestroySelf(const guid_t& self);

    bool InnerDestroyEntity(std::shared_ptr<AFIEntity> pEntity);

    bool AddEventCallBack(const guid_t& self, const int event_id, EVENT_PROCESS_FUNCTOR&& cb) override;
    bool AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb, const int32_t prio) override;
    bool AddNodeCallBack(const std::string& class_name, const std::string& name, DATA_NODE_EVENT_FUNCTOR&& cb,
        const int32_t prio) override;
    bool AddTableCallBack(const std::string& class_name, const std::string& name, DATA_TABLE_EVENT_FUNCTOR&& cb,
        const int32_t prio) override;

    bool AddNodeCallBack(
        const std::string& class_name, const uint32_t index, DATA_NODE_EVENT_FUNCTOR&& cb, const int32_t prio) override;
    bool AddTableCallBack(const std::string& class_name, const uint32_t index, DATA_TABLE_EVENT_FUNCTOR&& cb,
        const int32_t prio) override;

    bool AddContainerCallBack(
        const std::string& class_name, const uint32_t index, CONTAINER_EVENT_FUNCTOR&& cb, const int32_t prio) override;
    bool AddCommonContainerCallBack(CONTAINER_EVENT_FUNCTOR&& cb, const int32_t prio) override;

    bool AddCommonClassEvent(CLASS_EVENT_FUNCTOR&& cb, const int32_t prio) override;

    bool AddLeaveSceneEvent(const std::string& class_name, SCENE_EVENT_FUNCTOR&& cb, const int32_t prio) override;
    bool AddEnterSceneEvent(const std::string& class_name, SCENE_EVENT_FUNCTOR&& cb, const int32_t prio) override;

    bool AddMoveEvent(const std::string& class_name, MOVE_EVENT_FUNCTOR&& cb, const int32_t prio) override;

    // data sync call back
    void AddSyncCallBack();

    int OnSyncNode(const guid_t& self, const uint32_t index, const ArkDataMask mask_value, const AFIData& data);
    int OnSyncTable(
        const guid_t& self, const TABLE_EVENT_DATA& event, const ArkDataMask mask_value, const AFIData& data);
    int OnSyncContainer(const guid_t& self, const uint32_t index, const ArkDataMask mask,
        const ArkContainerOpType op_type, uint32_t src_index, uint32_t dest_index);
    int OnDelaySyncData(const guid_t& self, const ArkDataMask mask_value, const AFDelaySyncData& data);

    int OnSyncTableAdd(const guid_t& self, const TABLE_EVENT_DATA& event, const ArkDataMask mask_value);
    int OnSyncTableDelete(const guid_t& self, const TABLE_EVENT_DATA& event, const ArkDataMask mask_value);
    int OnSyncTableUpdate(
        const guid_t& self, const TABLE_EVENT_DATA& event, const ArkDataMask mask_value, const AFIData& data);

    int OnSyncContainerPlace(const guid_t& self, const uint32_t index, const ArkDataMask mask, uint32_t src_index);
    int OnSyncContainerRemove(const guid_t& self, const uint32_t index, const ArkDataMask mask, uint32_t src_index);
    int OnSyncContainerDestroy(const guid_t& self, const uint32_t index, const ArkDataMask mask, uint32_t src_index);
    int OnSyncContainerSwap(
        const guid_t& self, const uint32_t index, const ArkDataMask mask, uint32_t src_index, uint32_t dest_index);

    bool DelayTableToPB(const AFDelaySyncTable& table, const uint32_t index, AFMsg::pb_delay_entity& data,
        AFMsg::pb_entity_data& pb_entity);
    bool DelayContainerToPB(std::shared_ptr<AFIEntity> pEntity, const AFDelaySyncContainer& container,
        const uint32_t index, AFMsg::pb_delay_entity& data);

    bool TryAddContainerPBEntity(std::shared_ptr<AFIContainer> pContainer, const guid_t& self,
        AFMsg::pb_entity_data& pb_entity_data, guid_t& parent_id, AFMsg::pb_entity_data*& pb_container_entity);

    int SendSyncMsg(const guid_t& self, const ArkDataMask mask_value, const google::protobuf::Message& msg);
    int SendToView(const guid_t& self, const google::protobuf::Message& msg);
    int SendToSelf(const guid_t& self, const google::protobuf::Message& msg);

    // convert db data and entity
    bool EntityToDBData(std::shared_ptr<AFIEntity> pEntity, AFMsg::pb_db_entity& pb_data);
    bool NodeToDBData(AFINode* pNode, AFMsg::pb_db_entity_data& pb_data);
    bool TableToDBData(AFITable* pTable, AFMsg::pb_db_table& pb_data);

    bool DBDataToNode(std::shared_ptr<AFNodeManager> pNodeManager, const AFMsg::pb_db_entity_data& pb_db_entity_data);
    bool DBDataToTable(std::shared_ptr<AFIEntity> pEntity, const std::string& name, const AFMsg::pb_db_table& pb_table);
    bool DBDataToContainer(
        std::shared_ptr<AFIEntity> pEntity, const std::string& name, const AFMsg::pb_db_container& pb_data);

    // convert client pb and entity
    bool NodeToPBData(AFINode* pNode, AFMsg::pb_entity_data* pb_data);
    bool TableToPBData(AFITable* pTable, const uint32_t index, AFMsg::pb_table* pb_data);
    bool ContainerToPBData(std::shared_ptr<AFIContainer> pContainer, AFMsg::pb_container* pb_data);

    bool EntityToPBData(std::shared_ptr<AFIEntity> pEntity, AFMsg::pb_entity* pb_data);
    bool EntityToPBDataByMask(std::shared_ptr<AFIEntity> pEntity, ArkMaskType mask, AFMsg::pb_entity* pb_data);

    bool EntityContainerToPBData(
        std::shared_ptr<AFIEntity> pEntity, AFMsg::pb_entity_data* pb_data, const ArkMaskType mask = 0);

    // call back
    int OnContainerCallBack(const guid_t& self, const uint32_t index, const ArkContainerOpType op_type,
        const uint32_t src_index, const uint32_t dest_index);

    bool CopyData(std::shared_ptr<AFIEntity> pEntity, std::shared_ptr<AFIStaticEntity> pStaticEntity);

    // get entity data
    std::shared_ptr<AFNodeManager> GetNodeManager(std::shared_ptr<AFIStaticEntity> pStaticEntity) const;
    std::shared_ptr<AFNodeManager> GetNodeManager(std::shared_ptr<AFIEntity> pEntity) const;
    std::shared_ptr<AFNodeManager> GetNodeManager(AFIRow* pRow) const;
    std::shared_ptr<AFTableManager> GetTableManager(std::shared_ptr<AFIEntity> pEntity) const;
    std::shared_ptr<AFIContainerManager> GetContainerManager(std::shared_ptr<AFIEntity> pEntity) const;
    std::shared_ptr<AFIEventManager> GetEventManager(std::shared_ptr<AFIEntity> pEntity) const;

private:
    std::list<guid_t> delete_list_;

    guid_t cur_exec_object_{NULL_GUID};

    using SYNC_FUNCTOR = std::function<bool(const guid_t&, const google::protobuf::Message&)>;
    std::map<ArkDataMask, SYNC_FUNCTOR> sync_functors;

    AFIMapModule* m_pMapModule{nullptr};
    AFIGUIDModule* m_pGUIDModule{nullptr};
    AFIConfigModule* m_pConfigModule{nullptr};
    AFIClassMetaModule* m_pClassModule{nullptr};

    AFArrayMap<std::string, int32_t> inner_nodes_;
    AFSmartPtrMap<guid_t, AFIEntity> objects_;
};

} // namespace ark
