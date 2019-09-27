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

#include "world/include/AFCWorldNetModule.hpp"

namespace ark {

bool AFCWorldNetModule::Init()
{
    mnLastCheckTime = GetPluginManager()->GetNowTime();

    m_pKernelModule = FindModule<AFIKernelModule>();
    m_pLogModule = FindModule<AFILogModule>();
    m_pBusModule = FindModule<AFIBusModule>();
    m_pMsgModule = FindModule<AFIMsgModule>();
    m_pNetServiceManagerModule = FindModule<AFINetServiceManagerModule>();

    return true;
}

bool AFCWorldNetModule::PostInit()
{
    int ret = StartServer();
    if (ret != 0)
    {
        exit(0);
        return false;
    }

    return true;
}

int AFCWorldNetModule::StartServer()
{
    auto ret = m_pNetServiceManagerModule->CreateServer();
    ret.Then([=](const std::pair<bool, std::string>& resp) {
        if (!resp.first)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), resp.second);
            ARK_ASSERT_NO_EFFECT(0);
            exit(0);
        }

        m_pNetServer = m_pNetServiceManagerModule->GetSelfNetServer();
        if (m_pNetServer == nullptr)
        {
            ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->GetSelfBusName());
            exit(0);
        }

        // m_pNetServer->RegMsgCallback(AFMsg::EGMI_PTWG_PROXY_REFRESH, this,
        // &AFCWorldNetServerModule::OnRefreshProxyServerInfoProcess);
        // m_pNetServer->RegMsgCallback(AFMsg::EGMI_PTWG_PROXY_REGISTERED, this,
        // &AFCWorldNetServerModule::OnProxyServerRegisteredProcess);
        // m_pNetServer->RegMsgCallback(AFMsg::EGMI_PTWG_PROXY_UNREGISTERED, this,
        // &AFCWorldNetServerModule::OnProxyServerUnRegisteredProcess);
        // m_pNetServer->RegMsgCallback(AFMsg::EGMI_GTW_GAME_REGISTERED, this,
        // &AFCWorldNetServerModule::OnGameServerRegisteredProcess);
        // m_pNetServer->RegMsgCallback(AFMsg::EGMI_GTW_GAME_UNREGISTERED, this,
        // &AFCWorldNetServerModule::OnGameServerUnRegisteredProcess);
        // m_pNetServer->RegMsgCallback(AFMsg::EGMI_GTW_GAME_REFRESH, this,
        // &AFCWorldNetServerModule::OnRefreshGameServerInfoProcess);
        // m_pNetServer->RegMsgCallback(AFMsg::EGMI_ACK_ONLINE_NOTIFY, this, &AFCWorldNetModule::OnOnlineProcess);
        // m_pNetServer->RegMsgCallback(AFMsg::EGMI_ACK_OFFLINE_NOTIFY, this, &AFCWorldNetModule::OnOfflineProcess);
    });

    return 0;
}

//bool AFCWorldNetModule::PreUpdate()
//{
//    int ret = StartClient();
//    return (ret == 0);
//}
//
//int AFCWorldNetModule::StartClient()
//{
//    int ret = m_pNetServiceManagerModule->CreateClusterClients();
//    if (ret != 0)
//    {
//        ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
//        ARK_ASSERT_NO_EFFECT(0);
//        return ret;
//    }
//
//    // if need to add a member
//    AFINetClientService* pNetClientWorld =
//        m_pNetServiceManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_MASTER);
//    if (pNetClientWorld == nullptr)
//    {
//        return 0;
//    }
//
//    // pNetClientWorld->RegMsgCallback(AFMsg::EGMI_REQ_CONNECT_WORLD, this,
//    // &AFCWorldNetClientModule::OnSelectServerProcess);
//    // pNetClientWorld->RegMsgCallback(AFMsg::EGMI_REQ_KICK_CLIENT_INWORLD, this,
//    // &AFCWorldNetClientModule::OnKickClientProcess); pNetClientWorld->RegMsgCallback(this,
//    // &AFCWorldNetClientModule::InvalidMessage);
//
//    // pNetClientWorld->AddNetEventCallBack(this, &AFCWorldNetClientModule::OnSocketEvent);
//
//    return 0;
//}

// void AFCWorldNetServerModule::OnGameServerRegisteredProcess(const ARK_PKG_BASE_HEAD& head, const int msg_id, const
// char* msg, const uint32_t msg_len, const AFGUID& conn_id)
//{
//    //ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ServerInfoReportList);

//    //for (int i = 0; i < x_msg.server_list_size(); ++i)
//    //{
//    //    const AFMsg::ServerInfoReport& xData = x_msg.server_list(i);
//    //    ARK_SHARE_PTR<AFServerData> pServerData = mGameMap.GetElement(xData.bus_id());

//    //    if (nullptr == pServerData)
//    //    {
//    //        pServerData = std::make_shared<AFServerData>();
//    //        mGameMap.AddElement(xData.bus_id(), pServerData);
//    //    }

//    //    pServerData->Init(conn_id, xData);

//    //    ARK_LOG_INFO("GameServerRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    //}

//    //SynGameToProxy();
//}

// void AFCWorldNetServerModule::OnGameServerUnRegisteredProcess(const ARK_PKG_BASE_HEAD& head, const int msg_id, const
// char* msg, const uint32_t msg_len, const AFGUID& conn_id)
//{
//    //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    //for (int i = 0; i < xMsg.server_list_size(); ++i)
//    //{
//    //    const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
//    //    mGameMap.RemoveElement(xData.bus_id());

//    //    ARK_LOG_INFO("GameServer unregistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    //}
//}

// void AFCWorldNetServerModule::OnRefreshGameServerInfoProcess(const ARK_PKG_BASE_HEAD& head, const int msg_id, const
// char* msg, const uint32_t msg_len, const AFGUID& conn_id)
//{
//    //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    //for (int i = 0; i < xMsg.server_list_size(); ++i)
//    //{
//    //    const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

//    //    ARK_SHARE_PTR<AFServerData> pServerData = mGameMap.GetElement(xData.bus_id());

//    //    if (nullptr == pServerData)
//    //    {
//    //        pServerData = std::make_shared<AFServerData>();
//    //        mGameMap.AddElement(xData.bus_id(), pServerData);
//    //    }

//    //    pServerData->Init(xClientID, xData);

//    //    ARK_LOG_INFO("GameServer refersh, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    //}

//    //SynGameToProxy();
//}

// void AFCWorldNetServerModule::OnProxyServerRegisteredProcess(const ARK_PKG_BASE_HEAD& head, const int msg_id, const
// char* msg, const uint32_t msg_len, const AFGUID& conn_id)
//{
//    //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    //for (int i = 0; i < xMsg.server_list_size(); ++i)
//    //{
//    //    const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

//    //    ARK_SHARE_PTR<AFServerData> pServerData = mProxyMap.GetElement(xData.bus_id());

//    //    if (!pServerData)
//    //    {
//    //        pServerData = std::make_shared<AFServerData>();
//    //        mProxyMap.AddElement(xData.bus_id(), pServerData);
//    //    }

//    //    pServerData->Init(xClientID, xData);

//    //    ARK_LOG_INFO("Proxy Registered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    //    SynGameToProxy(xClientID);
//    //}
//}

// void AFCWorldNetServerModule::OnProxyServerUnRegisteredProcess(const ARK_PKG_BASE_HEAD& head, const int msg_id, const
// char* msg, const uint32_t msg_len, const AFGUID& conn_id)
//{
//    //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    //for (int i = 0; i < xMsg.server_list_size(); ++i)
//    //{
//    //    const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

//    //    mGameMap.RemoveElement(xData.bus_id());
//    //    ARK_LOG_INFO("Proxy UnRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    //}
//}

// void AFCWorldNetServerModule::OnRefreshProxyServerInfoProcess(const ARK_PKG_BASE_HEAD& head, const int msg_id, const
// char* msg, const uint32_t msg_len, const AFGUID& conn_id)
//{
//    //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::ServerInfoReportList);

//    //for (int i = 0; i < xMsg.server_list_size(); ++i)
//    //{
//    //    const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
//    //    ARK_SHARE_PTR<AFServerData> pServerData = mProxyMap.GetElement(xData.bus_id());

//    //    if (nullptr == pServerData)
//    //    {
//    //        pServerData = std::make_shared<AFServerData>();
//    //        mGameMap.AddElement(xData.bus_id(), pServerData);
//    //    }

//    //    pServerData->Init(xClientID, xData);
//    //    ARK_LOG_INFO("Proxy refresh, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
//    //    SynGameToProxy(xClientID);
//    //}
//}

int AFCWorldNetModule::OnLeaveGameProcess(const AFNetMsg* msg)
{
    return 0;
}

// void AFCWorldNetServerModule::OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip,
// const int bus_id)
//{
//    switch (event)
//    {
//    case CONNECTED:
//        {
//            ARK_LOG_INFO("Connected success, id = {}", conn_id.ToString());
//            OnClientConnected(conn_id);
//        }
//        break;
//    case DISCONNECTED:
//        {
//            ARK_LOG_INFO("Connection closed, id = {}", conn_id.ToString());
//            OnClientDisconnect(conn_id);
//        }
//        break;
//    default:
//        break;
//    }
//}

void AFCWorldNetModule::SynGameToProxy()
{
    // ARK_SHARE_PTR<AFServerData> pServerData = mProxyMap.First();

    // while (nullptr != pServerData)
    //{
    //    SynGameToProxy(pServerData->xClient);
    //    pServerData = mProxyMap.Next();
    //}
}

void AFCWorldNetModule::SynGameToProxy(const AFGUID& xClientID)
{
    // AFMsg::ServerInfoReportList xData;

    // ARK_SHARE_PTR<AFServerData> pServerData = mGameMap.First();

    // while (nullptr != pServerData)
    //{
    //    AFMsg::ServerInfoReport* pData = xData.add_server_list();
    //    *pData = pServerData->xData;

    //    pServerData = mGameMap.Next();
    //}

    // m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_STS_NET_INFO, xData, xClientID, AFGUID(0));
}

// void AFCWorldNetServerModule::OnClientDisconnect(const AFGUID& xClientID)
//{
//    //for (ARK_SHARE_PTR<AFServerData> pServerData = mGameMap.First(); nullptr != pServerData; pServerData =
//    mGameMap.Next())
//    //{
//    //    if (xClientID == pServerData->xClient)
//    //    {
//    //        pServerData->xData.set_logic_status(AFMsg::EST_CRASH);
//    //        pServerData->xClient = 0;

//    //        SynGameToProxy();
//    //        return;
//    //    }
//    //}
//    ////////////////////////////////////////////////////////////////////////////
//    //int nServerID = 0;
//    //for (ARK_SHARE_PTR<AFServerData> pServerData = mProxyMap.First(); pServerData != nullptr; pServerData =
//    mProxyMap.Next())
//    //{
//    //    if (xClientID == pServerData->xClient)
//    //    {
//    //        nServerID = pServerData->xData.bus_id();
//    //        break;
//    //    }
//    //}

//    //mProxyMap.RemoveElement(nServerID);
//}

// void AFCWorldNetServerModule::OnClientConnected(const AFGUID& xClientID)
//{
//    //log
//}

void AFCWorldNetModule::LogGameServer()
{
    // if (mnLastCheckTime + 10 * 1000 > GetPluginManager()->GetNowTime())
    //{
    //    return;
    //}

    // mnLastCheckTime = GetPluginManager()->GetNowTime();
    ////////////////////////////////////////////////////////////////////////////
    // ARK_LOG_INFO("Begin Log GameServer Info---------------------------");

    // for (ARK_SHARE_PTR<AFServerData> pGameData = mGameMap.First(); pGameData != nullptr; pGameData = mGameMap.Next())
    //{
    //    ARK_LOG_INFO("ID[{}] State[{}] IP[{}] xClient[{}]",
    //                 pGameData->xData.bus_id(),
    //                 AFMsg::EServerState_Name(pGameData->xData.logic_status()),
    //                 pGameData->xData.url(),
    //                 pGameData->xClient.nLow);
    //}

    // ARK_LOG_INFO("End Log GameServer Info---------------------------");
    ////////////////////////////////////////////////////////////////////////////
    // ARK_LOG_INFO("Begin Log ProxyServer Info---------------------------");

    // for (ARK_SHARE_PTR<AFServerData> pProxyData = mProxyMap.First(); pProxyData != nullptr; pProxyData =
    // mProxyMap.Next())
    //{
    //    ARK_LOG_INFO("ID[{}] State[{}] IP[{}] xClient[{}]",
    //                 pProxyData->xData.bus_id(),
    //                 AFMsg::EServerState_Name(pProxyData->xData.logic_status()),
    //                 pProxyData->xData.url(),
    //                 pProxyData->xClient.nLow);
    //}

    // ARK_LOG_INFO("End Log ProxyServer Info---------------------------");
}

// void AFCWorldNetModule::OnOnlineProcess(const AFNetMsg* msg)
//{
//    //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::RoleOnlineNotify);
//}

// void AFCWorldNetModule::OnOfflineProcess(const AFNetMsg* msg)
//{
//    //ARK_PROCESS_MSG(xHead, msg, nLen, AFMsg::RoleOfflineNotify);
//}

bool AFCWorldNetModule::SendMsgToGame(
    const int nGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer)
{
    // ARK_SHARE_PTR<AFServerData> pData = mGameMap.GetElement(nGameID);

    // if (nullptr != pData)
    //{
    //    m_pNetServer->SendPBMsg(eMsgID, xData, pData->xClient, nPlayer);
    //}

    return true;
}

bool AFCWorldNetModule::SendMsgToGame(const AFIDataList& argObjectVar, const AFIDataList& argGameID,
    const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData)
{
    if (argGameID.GetCount() != argObjectVar.GetCount())
    {
        return false;
    }

    for (size_t i = 0; i < argObjectVar.GetCount(); i++)
    {
        const AFGUID& identOther = argObjectVar.Int64(i);
        const int64_t nGameID = argGameID.Int(i);

        SendMsgToGame(nGameID, eMsgID, xData, identOther);
    }

    return true;
}

bool AFCWorldNetModule::SendMsgToPlayer(
    const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer)
{
    int nGameID = GetPlayerGameID(nPlayer);

    if (nGameID < 0)
    {
        return false;
    }

    return SendMsgToGame(nGameID, eMsgID, xData, nPlayer);
}

int AFCWorldNetModule::OnObjectListEnter(const AFIDataList& self, const AFIDataList& argVar)
{
    if (self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 0;
    }

    AFMsg::AckEntityEnterList xEntityEnterList;

    for (size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Int64(i);
        auto pEntity = m_pKernelModule->GetEntity(identOld);
        if (pEntity == nullptr)
        {
            continue;
        }

        if (identOld != 0)
        {
            AFMsg::EntityEnterInfo* pEnter = xEntityEnterList.add_entity_list();
            pEnter->set_object_guid(identOld);
            pEnter->set_career_type(pEntity->GetInt32("Job"));
            pEnter->set_player_state(pEntity->GetInt32("State"));
            pEnter->set_config_id(pEntity->GetConfigID());
            pEnter->set_scene_id(pEntity->GetInt32("SceneID"));
            pEnter->set_class_id(pEntity->GetString("ClassName"));
        }
    }

    if (xEntityEnterList.entity_list_size() <= 0)
    {
        return 0;
    }

    for (size_t i = 0; i < self.GetCount(); i++)
    {
        AFGUID ident = self.Int64(i);

        if (ident != 0)
        {
            SendMsgToPlayer(AFMsg::EGMI_ACK_ENTITY_ENTER, xEntityEnterList, ident);
        }
    }

    return 1;
}

int AFCWorldNetModule::OnObjectListLeave(const AFIDataList& self, const AFIDataList& argVar)
{
    if (self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 1;
    }

    AFMsg::AckEntityLeaveList xEntityLeaveList;

    for (size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Int64(i);

        if (!identOld == 0)
        {
            xEntityLeaveList.add_entity_list(identOld);
        }
    }

    for (size_t i = 0; i < self.GetCount(); i++)
    {
        AFGUID ident = self.Int64(i);

        if (ident == 0)
        {
            SendMsgToPlayer(AFMsg::EGMI_ACK_ENTITY_LEAVE, xEntityLeaveList, ident);
        }
    }

    return 0;
}

int AFCWorldNetModule::OnViewDataNodeEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self)
{
    if (argVar.GetCount() <= 0 || self == 0)
    {
        return 1;
    }

    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);
    if (nullptr == pEntity)
    {
        return 1;
    }

	AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    ArkMaskType mask;
    mask[(size_t)ArkNodeMask::PF_PUBLIC] = 1;
    AFIMsgModule::NodeToPBDataByMask(pEntity, mask, entity->mutable_data());

    for (size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOther = argVar.Int64(i);
        const int64_t nGameID = argGameID.Int(i);

        if (self != identOther)
        {
            SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, msg, identOther);
        }
    }

    return 0;
}

int AFCWorldNetModule::OnSelfDataNodeEnter(const AFGUID& self, const AFIDataList& argGameID)
{
    if (self == 0)
    {
        return 1;
    }

    const int64_t nGameID = argGameID.Int(0);

    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

    if (nullptr == pEntity)
    {
        return 1;
    }

	AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    ArkMaskType mask;
    mask[(size_t)ArkNodeMask::PF_PRIVATE] = 1;
    AFIMsgModule::NodeToPBDataByMask(pEntity, mask, entity->mutable_data());

    SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, msg, self);
    return 0;
}

int AFCWorldNetModule::OnSelfDataTableEnter(const AFGUID& self, const AFIDataList& argGameID)
{
    if (self == 0)
    {
        return 1;
    }

    auto pEntity = m_pKernelModule->GetEntity(self);
    if (nullptr == pEntity)
    {
        return 1;
    }

	AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    ArkMaskType mask;
    mask[(size_t)ArkTableNodeMask::PF_PRIVATE] = 1;
    AFIMsgModule::TableToPBDataByMask(pEntity, mask, entity->mutable_data());

	const int64_t nGameID = argGameID.Int(0);
    SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, msg, self);

    return 0;
}

int AFCWorldNetModule::OnViewDataTableEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self)
{
    if (argVar.GetCount() <= 0 || self == 0)
    {
        return 1;
    }

    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);
    if (nullptr == pEntity)
    {
        return 1;
    }

	AFMsg::multi_entity_data_list msg;
    AFMsg::pb_entity* entity = msg.add_data_list();
    entity->set_id(self);

    ArkMaskType mask;
    mask[(size_t)ArkTableNodeMask::PF_PUBLIC] = 1;
    AFIMsgModule::TableToPBDataByMask(pEntity, mask, entity->mutable_data());

    for (size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOther = argVar.Int64(i);
        const int64_t nGameID = argGameID.Int(i);

        if (self != identOther && msg.data_list_size() > 0)
        {
            SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, msg, identOther);
        }
    }

    return 0;
}

ARK_SHARE_PTR<AFServerData> AFCWorldNetModule::GetSuitProxyForEnter()
{
    return mProxyMap.begin()->second;
}

std::shared_ptr<AFINetServerService> AFCWorldNetModule::GetNetServer()
{
    return m_pNetServer;
}

int AFCWorldNetModule::GetPlayerGameID(const AFGUID self)
{
    // do something
    return -1;
}

//////////////////////////////////////////////////////////////////////////

void AFCWorldNetModule::RefreshWorldInfo()
{
    // Will add
}

void AFCWorldNetModule::OnSelectServerProcess(const AFNetMsg* msg)
{
    // ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ReqConnectWorld);

    // ARK_SHARE_PTR<AFServerData> xServerData = GetSuitProxyForEnter();

    // if (xServerData)
    //{
    //    AFMsg::AckConnectWorldResult xData;

    //    xData.set_world_id(x_msg.world_id());
    //    xData.set_sender(x_msg.sender());
    //    xData.set_login_id(x_msg.login_id());
    //    xData.set_account(x_msg.account());
    //    xData.set_world_url(xServerData->server_info_.url());
    //    xData.set_world_key(x_msg.account());

    //    //m_pWorldNetServerModule->GetNetServer()->SendPBMsg(AFMsg::EGMI_ACK_CONNECT_WORLD, xData,
    //    xServerData->conn_id_, actor_id);

    //    //TODO:will fix this
    //    //m_pNetClientModule->SendSuitByPB(xMsg.account(), AFMsg::EGMI_ACK_CONNECT_WORLD, xData, xHead.GetPlayerID());
    //}
}

void AFCWorldNetModule::OnKickClientProcess(const AFNetMsg* msg)
{
    // ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ReqKickFromWorld);
    // TODO
}
} // namespace ark
