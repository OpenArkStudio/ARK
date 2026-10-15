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

#include "ark/event_define.hpp"
#include "ark/kernel/data_list_impl.hpp"
#include "game/include/game_net_module_impl.hpp"

namespace ark {

bool game_net_module_impl::init() {
    m_pKernelModule = find_module<kernel_module>();
    m_pUUIDModule = find_module<guid_module>();
    m_pBusModule = find_module<bus_module>();
    m_pMsgModule = find_module<msg_module>();
    m_pNetServiceManagerModule = find_module<net_service_manager_module>();
    m_pMapModule = find_module<map_module>();

    m_pKernelModule->add_common_class_event(this, &game_net_module_impl::OnCommonClassEvent);

    //m_pKernelModule->AddNodeCallBack(AFEntityMetaPlayer::self_name(), AFEntityMetaBaseEntity::map_inst_id(), this,
    //    &AFCGameNetModule::OnMapInstanceEvent);
    //m_pKernelModule->AddNodeCallBack(
    //    AFEntityMetaPlayer::self_name(), AFEntityMetaBaseEntity::map_id(), this, &AFCGameNetModule::OnContainerEvent);

    m_pKernelModule->add_leave_scene_event(AFEntityMetaPlayer::self_name(), this, &game_net_module_impl::OnLeaveScene);
    m_pKernelModule->add_enter_scene_event(AFEntityMetaPlayer::self_name(), this, &game_net_module_impl::OnEnterScene);

    m_pKernelModule->add_class_call_back(AFEntityMetaPlayer::self_name(), this, &game_net_module_impl::OnEntityEvent);

    return true;
}

bool game_net_module_impl::post_init() {
    StartServer();
    return true;
}

int game_net_module_impl::StartServer() {
    auto ret = m_pNetServiceManagerModule->create_server();
    if (!ret)
    {
        ARK_LOG_ERROR("Cannot start server net, busid = {}", m_pBusModule->get_self_bus_name());
        ARK_ASSERT_NO_EFFECT(0);
        exit(0);
    }

    m_pNetServerService = m_pNetServiceManagerModule->get_inter_net_server();
    if (m_pNetServerService == nullptr)
    {
        ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->get_self_bus_name());
        exit(0);
    }

    return 0;
}

//bool AFCGameNetModule::PreUpdate()
//{
//    int ret = StartClient();
//    return (ret == 0);
//}
//
//int AFCGameNetModule::StartClient()
//{
//    //create all clients
//    // Create all target endpoint clients
//    int ret = m_pNetServiceManagerModule->CreateClusterClients();
//    if (ret != 0)
//    {
//        ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
//        ARK_ASSERT_NO_EFFECT(0);
//        return ret;
//    }
//
//    // if need to add a member
//    AFINetClientService* pNetClient = m_pNetServiceManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_WORLD);
//    if (pNetClient == nullptr)
//    {
//        return -1;
//    }
//
//    // TODO:will add a transfer callback
//    // pNetClient->RegMsgCallback(this, &AFCGameNetClientModule::TransPBToProxy);
//
//    return 0;
//}

int game_net_module_impl::OnViewDataNodeEnter(const data_list& argVar, const guid_t& self) {
    if (argVar.get_count() <= 0 || self == 0)
    {
        return 1;
    }

    std::shared_ptr<entity> pEntity = m_pKernelModule->get_entity(self);
    if (nullptr == pEntity)
    {
        return 0;
    }

    AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    mask_type mask;
    mask[(size_t)data_mask::PF_SYNC_VIEW] = 1;
    m_pKernelModule->entity_node_to_pb_data(pEntity, entity->mutable_data(), mask);

    for (size_t i = 0; i < argVar.get_count(); i++)
    {
        guid_t identOther = argVar.int64(i);

        if (self != identOther)
        {
            SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, msg, identOther);
        }
    }

    return 0;
}

int game_net_module_impl::OnSelfDataNodeEnter(const guid_t& self) {
    if (self == 0)
    {
        return 1;
    }

    std::shared_ptr<entity> pEntity = m_pKernelModule->get_entity(self);
    if (nullptr == pEntity)
    {
        return 1;
    }

    AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    mask_type mask;
    mask[(size_t)data_mask::PF_SYNC_SELF] = 1;
    m_pKernelModule->entity_node_to_pb_data(pEntity, entity->mutable_data(), mask);

    SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, msg, self);
    return 0;
}

int game_net_module_impl::OnSelfDataTableEnter(const guid_t& self) {
    if (self == 0)
    {
        return 1;
    }

    std::shared_ptr<entity> pEntity = m_pKernelModule->get_entity(self);

    if (nullptr == pEntity)
    {
        return 1;
    }

    AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    mask_type mask;
    mask[(size_t)data_mask::PF_SYNC_SELF] = 1;
    m_pKernelModule->entity_table_to_pb_data(pEntity, entity->mutable_data(), mask);

    SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, msg, self);
    return 0;
}

int game_net_module_impl::OnViewDataTableEnter(const data_list& argVar, const guid_t& self) {
    if (argVar.get_count() <= 0 || self == 0)
    {
        return 1;
    }

    std::shared_ptr<entity> pEntity = m_pKernelModule->get_entity(self);

    if (nullptr == pEntity)
    {
        return 1;
    }

    AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    mask_type mask;
    mask[(size_t)data_mask::PF_SYNC_VIEW] = 1;
    m_pKernelModule->entity_table_to_pb_data(pEntity, entity->mutable_data(), mask);

    for (size_t i = 0; i < argVar.get_count(); i++)
    {
        guid_t identOther = argVar.int64(i);

        if (self != identOther && msg.data_list_size() > 0)
        {
            SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, msg, identOther);
        }
    }

    return 0;
}

bool game_net_module_impl::ProcessLeaveGroup(const guid_t& self, int map_id, int old_inst_id) {
    if (old_inst_id <= 0)
    {
        return false;
    }

    data_list_impl valueAllOldObjectList;
    data_list_impl valueAllOldPlayerList;
    m_pMapModule->get_inst_entity_list(map_id, old_inst_id, valueAllOldObjectList);

    if (valueAllOldObjectList.get_count() > 0)
    {
        //broadcast self to others around
        for (size_t i = 0; i < valueAllOldObjectList.get_count(); i++)
        {
            guid_t identBC = valueAllOldObjectList.int64(i);
            auto pEntity = m_pKernelModule->get_entity(identBC);
            if (pEntity == nullptr)
            {
                continue;
            }

            if (valueAllOldObjectList.int64(i) == self)
            {
                valueAllOldObjectList.set_int64(i, 0);
            }

            const std::string& class_name = pEntity->get_class_name();
            if (AFEntityMetaPlayer::self_name() == class_name)
            {
                valueAllOldPlayerList << identBC;
            }
        }

        OnEntityListLeave(valueAllOldPlayerList, data_list_impl() << self);

        //broadcast self that leave the map
        OnEntityListLeave(data_list_impl() << self, valueAllOldObjectList);
    }

    m_pKernelModule->do_event(self, AFED_ON_CLIENT_LEAVE_SCENE, data_list_impl() << old_inst_id);

    return true;
}

bool game_net_module_impl::ProcessEnterGroup(const guid_t& self, int nSceneID, int nNewGroupID) {
    // broadcast self enter to others
    if (nNewGroupID <= 0)
    {
        return false;
    }

    //exclude myself
    //////////////////////////////////////////////////////////////////////////
    data_list_impl valueAllObjectList;
    data_list_impl valueAllObjectListNoSelf;
    data_list_impl valuePlayerList;
    data_list_impl valuePlayerListNoSelf;
    m_pMapModule->get_inst_entity_list(nSceneID, nNewGroupID, valueAllObjectList);

    for (size_t i = 0; i < valueAllObjectList.get_count(); i++)
    {
        guid_t identBC = valueAllObjectList.int64(i);
        auto pEntity = m_pKernelModule->get_entity(identBC);
        if (pEntity == nullptr)
        {
            continue;
        }

        const std::string& class_name = pEntity->get_class_name();
        if (AFEntityMetaPlayer::self_name() == class_name)
        {
            valuePlayerList << identBC;

            if (identBC != self)
            {
                valuePlayerListNoSelf << identBC;
            }
        }

        if (identBC != self)
        {
            valueAllObjectListNoSelf << identBC;
        }
    }

    // broadcast others that I'm here(need exclude self)
    if (valuePlayerListNoSelf.get_count() > 0)
    {
        auto pEntity = m_pKernelModule->get_entity(self);
        if (pEntity == nullptr)
        {
            return false;
        }

        OnEntityListEnter(valuePlayerListNoSelf, data_list_impl() << self);
        OnViewDataNodeEnter(valuePlayerListNoSelf, self);
        OnViewDataTableEnter(valuePlayerListNoSelf, self);

        const std::string class_name = pEntity->get_class_name();
        if (class_name == AFEntityMetaPlayer::self_name())
        {
            OnEntityListEnter(data_list_impl() << self, valueAllObjectListNoSelf);

            for (size_t i = 0; i < valueAllObjectListNoSelf.get_count(); i++)
            {
                // broadcast myself data to others
                guid_t identOld = valueAllObjectListNoSelf.int64(i);

                OnViewDataNodeEnter(data_list_impl() << self, identOld);
                OnViewDataTableEnter(data_list_impl() << self, identOld);
            }
        }
    }

    return true;
}

int game_net_module_impl::OnEntityListEnter(const data_list& self, const data_list& argVar) {
    if (self.get_count() <= 0 || argVar.get_count() <= 0)
    {
        return 1;
    }

    AFMsg::AckEntityEnterList xEntityEnterList;

    for (size_t i = 0; i < argVar.get_count(); i++)
    {
        guid_t identOld = argVar.int64(i);

        // Exclude empty entity
        if (identOld == 0)
        {
            continue;
        }

        auto pEntity = m_pKernelModule->get_entity(identOld);
        if (pEntity == nullptr)
        {
            continue;
        }

        AFMsg::EntityEnterInfo* pEnterInfo = xEntityEnterList.add_entity_list();
        pEnterInfo->set_object_guid(identOld);
        vector3d xPoint;
        xPoint.x = pEntity->get_float("x");
        xPoint.y = pEntity->get_float("y");
        xPoint.z = pEntity->get_float("z");

        *pEnterInfo->mutable_pos() = msg_module::vec_to_pb(xPoint);
        pEnterInfo->set_career_type(pEntity->get_int32("Job"));
        pEnterInfo->set_player_state(pEntity->get_int32("State"));
        pEnterInfo->set_config_id(pEntity->get_config_id());
        pEnterInfo->set_scene_id(pEntity->get_map_id());
        pEnterInfo->set_class_id(pEntity->get_class_name());
    }

    if (xEntityEnterList.entity_list_size() <= 0)
    {
        return 1;
    }

    for (size_t i = 0; i < self.get_count(); i++)
    {
        guid_t ident = self.int64(i);

        if (ident != 0)
        {
            SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_ENTER, xEntityEnterList, ident);
        }
    }

    return 0;
}

int game_net_module_impl::OnEntityListLeave(const data_list& self, const data_list& argVar) {
    if (self.get_count() <= 0 || argVar.get_count() <= 0)
    {
        return 1;
    }

    AFMsg::AckEntityLeaveList xEntityLeaveInfoList;

    for (size_t i = 0; i < argVar.get_count(); i++)
    {
        guid_t identOld = argVar.int64(i);

        // Exclude empty entity
        if (identOld != 0)
        {
            xEntityLeaveInfoList.add_entity_list(identOld);
        }
    }

    for (size_t i = 0; i < self.get_count(); i++)
    {
        guid_t ident = self.int64(i);

        if (ident != 0)
        {
            SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_LEAVE, xEntityLeaveInfoList, ident);
        }
    }

    return 0;
}

int game_net_module_impl::CommonClassDestoryEvent(const guid_t& self) {
    auto pEntity = m_pKernelModule->get_entity(self);
    if (nullptr == pEntity)
    {
        return 0;
    }

    int map_id = pEntity->get_int32(AFEntityMetaBaseEntity::map_id());
    int map_inst_id = pEntity->get_int32(AFEntityMetaBaseEntity::map_inst_id());

    if (map_inst_id < 0)
    {
        return 0;
    }

    data_list_impl valueAllObjectList;
    data_list_impl valueBroadCaseList;
    data_list_impl valueBroadListNoSelf;
    m_pMapModule->get_inst_entity_list(map_id, map_inst_id, valueAllObjectList);

    for (size_t i = 0; i < valueAllObjectList.get_count(); i++)
    {
        guid_t identBC = valueAllObjectList.int64(i);
        auto pBCEntity = m_pKernelModule->get_entity(identBC);
        if (nullptr == pBCEntity)
        {
            continue;
        }

        const std::string& class_name = pBCEntity->get_class_name();

        if (AFEntityMetaPlayer::self_name() == class_name)
        {
            valueBroadCaseList << identBC;

            if (identBC != self)
            {
                valueBroadListNoSelf << identBC;
            }
        }
    }

    OnEntityListLeave(valueBroadListNoSelf, data_list_impl() << self);
    return 0;
}

int game_net_module_impl::OnCommonClassEvent(
    const guid_t& self, const std::string& strClassName, const entity_event eClassEvent, const data_list& var) {
    switch (eClassEvent)
    {
    case entity_event::ENTITY_EVT_DESTROY:
        CommonClassDestoryEvent(self);
        break;

    case entity_event::ENTITY_EVT_PRE_LOAD_DATA:
    {
        // do something
    }
    break;

    case entity_event::ENTITY_EVT_LOAD_DATA:
        break;

    case entity_event::ENTITY_EVT_DATA_FINISHED:
    {
        // Just broadcast to self
        if (strClassName == AFEntityMetaPlayer::self_name())
        {
            OnEntityListEnter(data_list_impl() << self, data_list_impl() << self);

            OnSelfDataNodeEnter(self);
            OnSelfDataTableEnter(self);
        }
    }
    break;

    case entity_event::ENTITY_EVT_ALL_FINISHED:
        break;

    default:
        break;
    }

    return 0;
}

//int AFCGameNetModule::OnMapInstanceEvent(
//    const guid_t& self, const std::string& name, const uint32_t index, const AFIData& oldVar, const AFIData& newVar)
//{
//    // When the map changes, should be map A instance 0 to map B instance 0,
//    auto pEntity = m_pKernelModule->GetEntity(self);
//    if (pEntity == nullptr)
//    {
//        return 0;
//    }
//
//    int nSceneID = pEntity->GetInt32(AFEntityMetaBaseEntity::map_id());
//
//    // broadcast myself leaving to others
//    int nOldGroupID = oldVar.GetInt();
//    ProcessLeaveGroup(self, nSceneID, nOldGroupID);
//
//    int nNewGroupID = newVar.GetInt();
//    ProcessEnterGroup(self, nSceneID, nNewGroupID);
//
//    return 0;
//}
//
//int AFCGameNetModule::OnContainerEvent(
//    const guid_t& self, const std::string& name, const uint32_t index, const AFIData& oldVar, const AFIData& newVar)
//{
//    int nOldSceneID = oldVar.GetInt();
//    int nNowSceneID = newVar.GetInt();
//
//    ARK_LOG_INFO("Enter Scene, id  = {} scene = {}", self, nNowSceneID);
//
//    AFCDataList valueOldAllObjectList;
//    AFCDataList valueNewAllObjectList;
//    AFCDataList valueAllObjectListNoSelf;
//    AFCDataList valuePlayerList;
//    AFCDataList valuePlayerNoSelf;
//
//    m_pMapModule->GetInstEntityList(nOldSceneID, 0, valueOldAllObjectList);
//    m_pMapModule->GetInstEntityList(nNowSceneID, 0, valueNewAllObjectList);
//
//    for (size_t i = 0; i < valueOldAllObjectList.GetCount(); i++)
//    {
//        guid_t identBC = valueOldAllObjectList.Int64(i);
//
//        if (identBC == self)
//        {
//            valueOldAllObjectList.SetInt64(i, 0);
//        }
//    }
//
//    for (size_t i = 0; i < valueNewAllObjectList.GetCount(); i++)
//    {
//        guid_t identBC = valueNewAllObjectList.Int64(i);
//        auto pBCEntity = m_pKernelModule->GetEntity(identBC);
//        if (pBCEntity == nullptr)
//        {
//            continue;
//        }
//
//        const std::string& class_name = pBCEntity->GetClassName();
//
//        if (AFEntityMetaPlayer::self_name() == class_name)
//        {
//            valuePlayerList << identBC;
//
//            if (identBC != self)
//            {
//                valuePlayerNoSelf << identBC;
//            }
//        }
//
//        if (identBC != self)
//        {
//            valueAllObjectListNoSelf << identBC;
//        }
//    }
//
//    //////////////////////////////////////////////////////////////////////////
//
//    OnEntityListLeave(AFCDataList() << self, valueOldAllObjectList);
//
//    // the instance is 0
//    if (valuePlayerList.GetCount() > 0)
//    {
//        OnEntityListEnter(valuePlayerNoSelf, AFCDataList() << self);
//    }
//
//    OnEntityListEnter(AFCDataList() << self, valueAllObjectListNoSelf);
//
//    for (size_t i = 0; i < valueAllObjectListNoSelf.GetCount(); i++)
//    {
//        guid_t identOld = valueAllObjectListNoSelf.Int64(i);
//        OnViewDataNodeEnter(AFCDataList() << self, identOld);
//        OnViewDataTableEnter(AFCDataList() << self, identOld);
//    }
//
//    if (valuePlayerNoSelf.GetCount() > 0)
//    {
//        OnViewDataNodeEnter(valuePlayerNoSelf, self);
//        OnViewDataTableEnter(valuePlayerNoSelf, self);
//    }
//
//    return 0;
//}

int game_net_module_impl::OnLeaveScene(const guid_t& self, const int map_id, const int map_inst_id) {
    ProcessLeaveGroup(self, map_id, map_inst_id);
    return 0;
}

int game_net_module_impl::OnEnterScene(const guid_t& self, const int map_id, const int map_inst_id) {
    ProcessEnterGroup(self, map_id, map_inst_id);
    return 0;
}

int game_net_module_impl::GetBroadcastEntityList(const int map_id, const int inst_id, data_list& valueObject) {
    data_list_impl map_inst_entity_list;
    m_pMapModule->get_inst_entity_list(map_id, inst_id, map_inst_entity_list);

    for (size_t i = 0; i < map_inst_entity_list.get_count(); i++)
    {
        auto pEntity = m_pKernelModule->get_entity(map_inst_entity_list.int64(i));
        if (pEntity == nullptr)
        {
            continue;
        }

        const std::string& strObjClassName = pEntity->get_class_name();

        if (AFEntityMetaPlayer::self_name() == strObjClassName)
        {
            valueObject.add_int64(map_inst_entity_list.int64(i));
        }
    }

    return valueObject.get_count();
}

int game_net_module_impl::OnEntityEvent(
    const guid_t& self, const std::string& strClassName, const entity_event eClassEvent, const data_list& var) {
    switch (eClassEvent)
    {
    case entity_event::ENTITY_EVT_DESTROY:
        // save data to db by yourself logic
        ARK_LOG_INFO("Player Offline, player_id = {}", self);
        break;

    case entity_event::ENTITY_EVT_LOAD_DATA:
        // load data from db by yourself logic
        ARK_LOG_INFO("Player online, player_id = {}", self);
        break;

    case entity_event::ENTITY_EVT_ALL_FINISHED:
        // load data finished from db by yourself logic
        ARK_LOG_INFO("Player ready, player_id = {}", self);
        break;

    default:
        break;
    }

    return 0;
}

void game_net_module_impl::SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const guid_t& self) {
    //std::shared_ptr<GateBaseInfo> pData = mRoleBaseData.find_value(self);

    //if (nullptr == pData)
    //{
    //    return;
    //}

    // std::shared_ptr<GateServerInfo> pProxyData = mProxyMap.GetElement(pData->nGateID);

    // if (nullptr != pProxyData)
    //{
    //    m_pNetServerService->SendPBMsg(nMsgID, xMsg, pProxyData->xServerData.conn_id_, pData->xClientID);
    //}
}

void game_net_module_impl::SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const guid_t& self) {
    //std::shared_ptr<GateBaseInfo> pData = mRoleBaseData.find_value(self);

    //if (nullptr == pData)
    //{
    //    return;
    //}

    // std::shared_ptr<GateServerInfo> pProxyData = mProxyMap.GetElement(pData->nGateID);

    // if (nullptr != pProxyData)
    //{
    //    m_pNetServerService->SendMsg(nMsgID, strMsg, pProxyData->xServerData.conn_id_, pData->xClientID);
    //}
}

std::shared_ptr<net_server_service> game_net_module_impl::GetNetServerService() {
    return m_pNetServerService;
}

bool game_net_module_impl::AddPlayerGateInfo(const guid_t& nRoleID, const guid_t& nClientID, const int nGateID) {
    //if (nGateID <= 0)
    //{
    //    return false;
    //}

    //if (nClientID == 0)
    //{
    //    return false;
    //}

    //std::shared_ptr<AFCGameNetModule::GateBaseInfo> pBaseData = mRoleBaseData.find_value(nRoleID);

    //if (nullptr != pBaseData)
    //{
    //    ARK_LOG_ERROR("player is already exist, cannot enter game again, id = {}", nClientID);
    //    return false;
    //}

    //std::shared_ptr<GateServerInfo> pServerData = mProxyMap.find_value(nGateID);

    //if (nullptr == pServerData)
    //{
    //    return false;
    //}

    //if (!pServerData->xRoleInfo.insert(std::make_pair(nRoleID, pServerData->xServerData.conn_id_)).second)
    //{
    //    return false;
    //}

    //if (!mRoleBaseData.insert(nRoleID, std::make_shared<GateBaseInfo>(nGateID, nClientID)).second)
    //{
    //    pServerData->xRoleInfo.erase(nRoleID);
    //    return false;
    //}

    return true;
}

//bool AFCGameNetModule::AddPlayerGate(const guid_t& actor_id, const int gate_bus)
//{
//    if (actor_id == NULL_GUID)
//    {
//        ARK_LOG_ERROR("player does not exist, acrot id is null");
//        return false;
//    }
//
//    auto iter = mActorIDBusIDData.find(actor_id);
//    if (iter != mActorIDBusIDData.end())
//    {
//        ARK_LOG_ERROR("player is already exist, cannot enter game again, actor id = {}", actor_id);
//        return false;
//    }
//
//    return mActorIDBusIDData.insert(std::make_pair(actor_id, gate_bus)).second;
//}
//
//bool AFCGameNetModule::RemovePlayerGate(const guid_t& actor_id)
//{
//    mActorIDBusIDData.erase(actor_id);
//    return true;
//}

bool game_net_module_impl::RemovePlayerGateInfo(const guid_t& nRoleID) {
    //std::shared_ptr<GateBaseInfo> pBaseData = mRoleBaseData.find_value(nRoleID);

    //if (nullptr == pBaseData)
    //{
    //    return false;
    //}

    //mRoleBaseData.erase(nRoleID);

    //std::shared_ptr<GateServerInfo> pServerData = mProxyMap.find_value(pBaseData->nGateID);

    //if (nullptr == pServerData)
    //{
    //    return false;
    //}

    //pServerData->xRoleInfo.erase(nRoleID);
    return true;
}

//std::shared_ptr<AFIGameNetModule::GateBaseInfo> AFCGameNetModule::GetPlayerGateInfo(const guid_t& nRoleID)
//{
//    return mRoleBaseData.find_value(nRoleID);
//}

//std::shared_ptr<AFIGameNetModule::GateServerInfo> AFCGameNetModule::GetGateServerInfo(const int nGateID)
//{
//    return mProxyMap.find_value(nGateID);
//}
//
//std::shared_ptr<AFIGameNetModule::GateServerInfo> AFCGameNetModule::GetGateServerInfoByClientID(const guid_t& nClientID)
//{
//    int nGateID = -1;
//    for (auto iter : mProxyMap)
//    {
//        auto pServerData = iter.second;
//        if (nClientID == pServerData->xServerData.conn_id_)
//        {
//            nGateID = pServerData->xServerData.server_info_.bus_id();
//            if (nGateID != -1)
//            {
//                return pServerData;
//            }
//        }
//    }
//
//    return nullptr;
//}

} // namespace ark
