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

#include "base/AFEventDefine.hpp"
#include "game/include/AFCGameNetModule.hpp"
#include "kernel/include/AFCDataList.hpp"

namespace ark {

bool AFCGameNetModule::Init()
{
    m_pKernelModule = FindModule<AFIKernelModule>();
    m_pLogModule = FindModule<AFILogModule>();
    m_pUUIDModule = FindModule<AFIGUIDModule>();
    m_pBusModule = FindModule<AFIBusModule>();
    m_pMsgModule = FindModule<AFIMsgModule>();
    m_pNetServiceManagerModule = FindModule<AFINetServiceManagerModule>();
    m_pMapModule = FindModule<AFIMapModule>();

    m_pKernelModule->AddCommonClassEvent(this, &AFCGameNetModule::OnCommonClassEvent);

    //m_pKernelModule->AddNodeCallBack(AFEntityMetaPlayer::self_name(), AFEntityMetaBaseEntity::map_inst_id(), this,
    //    &AFCGameNetModule::OnMapInstanceEvent);
    //m_pKernelModule->AddNodeCallBack(
    //    AFEntityMetaPlayer::self_name(), AFEntityMetaBaseEntity::map_id(), this, &AFCGameNetModule::OnContainerEvent);

    m_pKernelModule->AddLeaveSceneEvent(AFEntityMetaPlayer::self_name(), this, &AFCGameNetModule::OnLeaveScene);
    m_pKernelModule->AddEnterSceneEvent(AFEntityMetaPlayer::self_name(), this, &AFCGameNetModule::OnEnterScene);

    m_pKernelModule->AddClassCallBack(AFEntityMetaPlayer::self_name(), this, &AFCGameNetModule::OnEntityEvent);

    return true;
}

bool AFCGameNetModule::PostInit()
{
    StartServer();
    return true;
}

int AFCGameNetModule::StartServer()
{
    auto ret = m_pNetServiceManagerModule->CreateServer();
    ret.Then([=](const std::pair<bool, std::string>& resp) {
        if (!resp.first)
        {
            ARK_LOG_ERROR(
                "Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), resp.second);
            ARK_ASSERT_NO_EFFECT(0);
            exit(0);
        }

        m_pNetServerService = m_pNetServiceManagerModule->GetSelfNetServer();
        if (m_pNetServerService == nullptr)
        {
            ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->GetSelfBusName());
            exit(0);
        }

        // m_pNetServerService->RegNetEventCallback(this, &AFCGameNetModule::OnSocketEvent);
    });

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

int AFCGameNetModule::OnViewDataNodeEnter(const AFIDataList& argVar, const AFGUID& self)
{
    if (argVar.GetCount() <= 0 || self == 0)
    {
        return 1;
    }

    std::shared_ptr<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);
    if (nullptr == pEntity)
    {
        return 0;
    }

    AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    ArkMaskType mask;
    mask[(size_t)ArkDataMask::PF_SYNC_VIEW] = 1;
    m_pKernelModule->EntityNodeToPBData(pEntity, entity->mutable_data(), mask);

    for (size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOther = argVar.Int64(i);

        if (self != identOther)
        {
            SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, msg, identOther);
        }
    }

    return 0;
}

int AFCGameNetModule::OnSelfDataNodeEnter(const AFGUID& self)
{
    if (self == 0)
    {
        return 1;
    }

    std::shared_ptr<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);
    if (nullptr == pEntity)
    {
        return 1;
    }

    AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    ArkMaskType mask;
    mask[(size_t)ArkDataMask::PF_SYNC_SELF] = 1;
    m_pKernelModule->EntityNodeToPBData(pEntity, entity->mutable_data(), mask);

    SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, msg, self);
    return 0;
}

int AFCGameNetModule::OnSelfDataTableEnter(const AFGUID& self)
{
    if (self == 0)
    {
        return 1;
    }

    std::shared_ptr<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

    if (nullptr == pEntity)
    {
        return 1;
    }

    AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    ArkMaskType mask;
    mask[(size_t)ArkDataMask::PF_SYNC_SELF] = 1;
    m_pKernelModule->EntityTableToPBData(pEntity, entity->mutable_data(), mask);

    SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, msg, self);
    return 0;
}

int AFCGameNetModule::OnViewDataTableEnter(const AFIDataList& argVar, const AFGUID& self)
{
    if (argVar.GetCount() <= 0 || self == 0)
    {
        return 1;
    }

    std::shared_ptr<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

    if (nullptr == pEntity)
    {
        return 1;
    }

    AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    ArkMaskType mask;
    mask[(size_t)ArkDataMask::PF_SYNC_VIEW] = 1;
    m_pKernelModule->EntityTableToPBData(pEntity, entity->mutable_data(), mask);

    for (size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOther = argVar.Int64(i);

        if (self != identOther && msg.data_list_size() > 0)
        {
            SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, msg, identOther);
        }
    }

    return 0;
}

bool AFCGameNetModule::ProcessLeaveGroup(const AFGUID& self, int map_id, int old_inst_id)
{
    if (old_inst_id <= 0)
    {
        return false;
    }

    AFCDataList valueAllOldObjectList;
    AFCDataList valueAllOldPlayerList;
    m_pMapModule->GetInstEntityList(map_id, old_inst_id, valueAllOldObjectList);

    if (valueAllOldObjectList.GetCount() > 0)
    {
        //broadcast self to others around
        for (size_t i = 0; i < valueAllOldObjectList.GetCount(); i++)
        {
            AFGUID identBC = valueAllOldObjectList.Int64(i);
            auto pEntity = m_pKernelModule->GetEntity(identBC);
            if (pEntity == nullptr)
            {
                continue;
            }

            if (valueAllOldObjectList.Int64(i) == self)
            {
                valueAllOldObjectList.SetInt64(i, 0);
            }

            const std::string& class_name = pEntity->GetClassName();
            if (AFEntityMetaPlayer::self_name() == class_name)
            {
                valueAllOldPlayerList << identBC;
            }
        }

        OnEntityListLeave(valueAllOldPlayerList, AFCDataList() << self);

        //broadcast self that leave the map
        OnEntityListLeave(AFCDataList() << self, valueAllOldObjectList);
    }

    m_pKernelModule->DoEvent(self, AFED_ON_CLIENT_LEAVE_SCENE, AFCDataList() << old_inst_id);

    return true;
}

bool AFCGameNetModule::ProcessEnterGroup(const AFGUID& self, int nSceneID, int nNewGroupID)
{
    // broadcast self enter to others
    if (nNewGroupID <= 0)
    {
        return false;
    }

    //exclude myself
    //////////////////////////////////////////////////////////////////////////
    AFCDataList valueAllObjectList;
    AFCDataList valueAllObjectListNoSelf;
    AFCDataList valuePlayerList;
    AFCDataList valuePlayerListNoSelf;
    m_pMapModule->GetInstEntityList(nSceneID, nNewGroupID, valueAllObjectList);

    for (size_t i = 0; i < valueAllObjectList.GetCount(); i++)
    {
        AFGUID identBC = valueAllObjectList.Int64(i);
        auto pEntity = m_pKernelModule->GetEntity(identBC);
        if (pEntity == nullptr)
        {
            continue;
        }

        const std::string& class_name = pEntity->GetClassName();
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
    if (valuePlayerListNoSelf.GetCount() > 0)
    {
        auto pEntity = m_pKernelModule->GetEntity(self);
        if (pEntity == nullptr)
        {
            return false;
        }

        OnEntityListEnter(valuePlayerListNoSelf, AFCDataList() << self);
        OnViewDataNodeEnter(valuePlayerListNoSelf, self);
        OnViewDataTableEnter(valuePlayerListNoSelf, self);

        const std::string class_name = pEntity->GetClassName();
        if (class_name == AFEntityMetaPlayer::self_name())
        {
            OnEntityListEnter(AFCDataList() << self, valueAllObjectListNoSelf);

            for (size_t i = 0; i < valueAllObjectListNoSelf.GetCount(); i++)
            {
                // broadcast myself data to others
                AFGUID identOld = valueAllObjectListNoSelf.Int64(i);

                OnViewDataNodeEnter(AFCDataList() << self, identOld);
                OnViewDataTableEnter(AFCDataList() << self, identOld);
            }
        }
    }

    return true;
}

int AFCGameNetModule::OnEntityListEnter(const AFIDataList& self, const AFIDataList& argVar)
{
    if (self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 1;
    }

    AFMsg::AckEntityEnterList xEntityEnterList;

    for (size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Int64(i);

        // Exclude empty entity
        if (identOld == 0)
        {
            continue;
        }

        auto pEntity = m_pKernelModule->GetEntity(identOld);
        if (pEntity == nullptr)
        {
            continue;
        }

        AFMsg::EntityEnterInfo* pEnterInfo = xEntityEnterList.add_entity_list();
        pEnterInfo->set_object_guid(identOld);
        AFVector3D xPoint;
        xPoint.x = pEntity->GetFloat("x");
        xPoint.y = pEntity->GetFloat("y");
        xPoint.z = pEntity->GetFloat("z");

        *pEnterInfo->mutable_pos() = AFIMsgModule::VecToPB(xPoint);
        pEnterInfo->set_career_type(pEntity->GetInt32("Job"));
        pEnterInfo->set_player_state(pEntity->GetInt32("State"));
        pEnterInfo->set_config_id(pEntity->GetConfigID());
        pEnterInfo->set_scene_id(pEntity->GetMapID());
        pEnterInfo->set_class_id(pEntity->GetClassName());
    }

    if (xEntityEnterList.entity_list_size() <= 0)
    {
        return 1;
    }

    for (size_t i = 0; i < self.GetCount(); i++)
    {
        AFGUID ident = self.Int64(i);

        if (ident != 0)
        {
            SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_ENTER, xEntityEnterList, ident);
        }
    }

    return 0;
}

int AFCGameNetModule::OnEntityListLeave(const AFIDataList& self, const AFIDataList& argVar)
{
    if (self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 1;
    }

    AFMsg::AckEntityLeaveList xEntityLeaveInfoList;

    for (size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Int64(i);

        // Exclude empty entity
        if (identOld != 0)
        {
            xEntityLeaveInfoList.add_entity_list(identOld);
        }
    }

    for (size_t i = 0; i < self.GetCount(); i++)
    {
        AFGUID ident = self.Int64(i);

        if (ident != 0)
        {
            SendMsgPBToGate(AFMsg::EGMI_ACK_ENTITY_LEAVE, xEntityLeaveInfoList, ident);
        }
    }

    return 0;
}

int AFCGameNetModule::CommonClassDestoryEvent(const AFGUID& self)
{
    auto pEntity = m_pKernelModule->GetEntity(self);
    if (nullptr == pEntity)
    {
        return 0;
    }

    int map_id = pEntity->GetInt32(AFEntityMetaBaseEntity::map_id());
    int map_inst_id = pEntity->GetInt32(AFEntityMetaBaseEntity::map_inst_id());

    if (map_inst_id < 0)
    {
        return 0;
    }

    AFCDataList valueAllObjectList;
    AFCDataList valueBroadCaseList;
    AFCDataList valueBroadListNoSelf;
    m_pMapModule->GetInstEntityList(map_id, map_inst_id, valueAllObjectList);

    for (size_t i = 0; i < valueAllObjectList.GetCount(); i++)
    {
        AFGUID identBC = valueAllObjectList.Int64(i);
        auto pBCEntity = m_pKernelModule->GetEntity(identBC);
        if (nullptr == pBCEntity)
        {
            continue;
        }

        const std::string& class_name = pBCEntity->GetClassName();

        if (AFEntityMetaPlayer::self_name() == class_name)
        {
            valueBroadCaseList << identBC;

            if (identBC != self)
            {
                valueBroadListNoSelf << identBC;
            }
        }
    }

    OnEntityListLeave(valueBroadListNoSelf, AFCDataList() << self);
    return 0;
}

int AFCGameNetModule::OnCommonClassEvent(
    const AFGUID& self, const std::string& strClassName, const ArkEntityEvent eClassEvent, const AFIDataList& var)
{
    switch (eClassEvent)
    {
        case ArkEntityEvent::ENTITY_EVT_DESTROY:
            CommonClassDestoryEvent(self);
            break;

        case ArkEntityEvent::ENTITY_EVT_PRE_LOAD_DATA:
        {
            // do something
        }
        break;

        case ArkEntityEvent::ENTITY_EVT_LOAD_DATA:
            break;

        case ArkEntityEvent::ENTITY_EVT_DATA_FINISHED:
        {
            // Just broadcast to self
            if (strClassName == AFEntityMetaPlayer::self_name())
            {
                OnEntityListEnter(AFCDataList() << self, AFCDataList() << self);

                OnSelfDataNodeEnter(self);
                OnSelfDataTableEnter(self);
            }
        }
        break;

        case ArkEntityEvent::ENTITY_EVT_ALL_FINISHED:
            break;

        default:
            break;
    }

    return 0;
}

//int AFCGameNetModule::OnMapInstanceEvent(
//    const AFGUID& self, const std::string& name, const uint32_t index, const AFIData& oldVar, const AFIData& newVar)
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
//    const AFGUID& self, const std::string& name, const uint32_t index, const AFIData& oldVar, const AFIData& newVar)
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
//        AFGUID identBC = valueOldAllObjectList.Int64(i);
//
//        if (identBC == self)
//        {
//            valueOldAllObjectList.SetInt64(i, 0);
//        }
//    }
//
//    for (size_t i = 0; i < valueNewAllObjectList.GetCount(); i++)
//    {
//        AFGUID identBC = valueNewAllObjectList.Int64(i);
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
//        AFGUID identOld = valueAllObjectListNoSelf.Int64(i);
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

int AFCGameNetModule::OnLeaveScene(const AFGUID& self, const int map_id, const int map_inst_id)
{
    ProcessLeaveGroup(self, map_id, map_inst_id);
    return 0;
}

int AFCGameNetModule::OnEnterScene(const AFGUID& self, const int map_id, const int map_inst_id)
{
    ProcessEnterGroup(self, map_id, map_inst_id);
    return 0;
}

int AFCGameNetModule::GetBroadcastEntityList(const int map_id, const int inst_id, AFIDataList& valueObject)
{
    AFCDataList map_inst_entity_list;
    m_pMapModule->GetInstEntityList(map_id, inst_id, map_inst_entity_list);

    for (size_t i = 0; i < map_inst_entity_list.GetCount(); i++)
    {
        auto pEntity = m_pKernelModule->GetEntity(map_inst_entity_list.Int64(i));
        if (pEntity == nullptr)
        {
            continue;
        }

        const std::string& strObjClassName = pEntity->GetClassName();

        if (AFEntityMetaPlayer::self_name() == strObjClassName)
        {
            valueObject.AddInt64(map_inst_entity_list.Int64(i));
        }
    }

    return valueObject.GetCount();
}

int AFCGameNetModule::OnEntityEvent(
    const AFGUID& self, const std::string& strClassName, const ArkEntityEvent eClassEvent, const AFIDataList& var)
{
    switch (eClassEvent)
    {
        case ArkEntityEvent::ENTITY_EVT_DESTROY:
            // save data to db by yourself logic
            ARK_LOG_INFO("Player Offline, player_id = {}", self);
            break;

        case ArkEntityEvent::ENTITY_EVT_LOAD_DATA:
            // load data from db by yourself logic
            ARK_LOG_INFO("Player online, player_id = {}", self);
            break;

        case ArkEntityEvent::ENTITY_EVT_ALL_FINISHED:
            // load data finished from db by yourself logic
            ARK_LOG_INFO("Player ready, player_id = {}", self);
            break;

        default:
            break;
    }

    return 0;
}

void AFCGameNetModule::SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const AFGUID& self)
{
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

void AFCGameNetModule::SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const AFGUID& self)
{
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

std::shared_ptr<AFINetServerService> AFCGameNetModule::GetNetServerService()
{
    return m_pNetServerService;
}

bool AFCGameNetModule::AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID)
{
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

//bool AFCGameNetModule::AddPlayerGate(const AFGUID& actor_id, const int gate_bus)
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
//bool AFCGameNetModule::RemovePlayerGate(const AFGUID& actor_id)
//{
//    mActorIDBusIDData.erase(actor_id);
//    return true;
//}

bool AFCGameNetModule::RemovePlayerGateInfo(const AFGUID& nRoleID)
{
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

//std::shared_ptr<AFIGameNetModule::GateBaseInfo> AFCGameNetModule::GetPlayerGateInfo(const AFGUID& nRoleID)
//{
//    return mRoleBaseData.find_value(nRoleID);
//}

//std::shared_ptr<AFIGameNetModule::GateServerInfo> AFCGameNetModule::GetGateServerInfo(const int nGateID)
//{
//    return mProxyMap.find_value(nGateID);
//}
//
//std::shared_ptr<AFIGameNetModule::GateServerInfo> AFCGameNetModule::GetGateServerInfoByClientID(const AFGUID& nClientID)
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
