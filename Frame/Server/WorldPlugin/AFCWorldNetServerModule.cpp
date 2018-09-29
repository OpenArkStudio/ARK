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

#include "Common/AFProtoCPP.hpp"
#include "SDK/Core/AFDataNode.hpp"
#include "AFCWorldNetServerModule.h"

namespace ark
{

    bool AFCWorldNetServerModule::Init()
    {
        m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pNetServerManagerModule = pPluginManager->FindModule<AFINetServerManagerModule>();

        return true;
    }

    bool AFCWorldNetServerModule::PostInit()
    {
        int ret = StartServer();
        if (ret != 0)
        {
            exit(0);
            return false;
        }

        return true;
    }

    int AFCWorldNetServerModule::StartServer()
    {
        int ret = m_pNetServerManagerModule->CreateServer();
        if (ret != 0)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            ARK_ASSERT_NO_EFFECT(0);
            return ret;
        }

        m_pNetServer = m_pNetServerManagerModule->GetSelfNetServer();
        if (m_pNetServer == nullptr)
        {
            ret = -3;
            ARK_LOG_ERROR("Cannot find server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            return ret;
        }

        m_pNetServer->AddRecvCallback(AFMsg::EGMI_PTWG_PROXY_REFRESH, this, &AFCWorldNetServerModule::OnRefreshProxyServerInfoProcess);
        m_pNetServer->AddRecvCallback(AFMsg::EGMI_PTWG_PROXY_REGISTERED, this, &AFCWorldNetServerModule::OnProxyServerRegisteredProcess);
        m_pNetServer->AddRecvCallback(AFMsg::EGMI_PTWG_PROXY_UNREGISTERED, this, &AFCWorldNetServerModule::OnProxyServerUnRegisteredProcess);
        m_pNetServer->AddRecvCallback(AFMsg::EGMI_GTW_GAME_REGISTERED, this, &AFCWorldNetServerModule::OnGameServerRegisteredProcess);
        m_pNetServer->AddRecvCallback(AFMsg::EGMI_GTW_GAME_UNREGISTERED, this, &AFCWorldNetServerModule::OnGameServerUnRegisteredProcess);
        m_pNetServer->AddRecvCallback(AFMsg::EGMI_GTW_GAME_REFRESH, this, &AFCWorldNetServerModule::OnRefreshGameServerInfoProcess);
        m_pNetServer->AddRecvCallback(AFMsg::EGMI_ACK_ONLINE_NOTIFY, this, &AFCWorldNetServerModule::OnOnlineProcess);
        m_pNetServer->AddRecvCallback(AFMsg::EGMI_ACK_OFFLINE_NOTIFY, this, &AFCWorldNetServerModule::OnOfflineProcess);

        m_pNetServer->AddEventCallBack(this, &AFCWorldNetServerModule::OnSocketEvent);

        return 0;
    }

    bool AFCWorldNetServerModule::Update()
    {
        LogGameServer();

        return true;
    }

    void AFCWorldNetServerModule::OnGameServerRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

        for (int i = 0; i < xMsg.server_list_size(); ++i)
        {
            const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
            ARK_SHARE_PTR<AFServerData> pServerData = mGameMap.GetElement(xData.bus_id());

            if (nullptr == pServerData)
            {
                pServerData = std::make_shared<AFServerData>();
                mGameMap.AddElement(xData.bus_id(), pServerData);
            }

            pServerData->Init(xClientID, xData);

            ARK_LOG_INFO("GameServerRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
        }

        SynGameToProxy();
    }

    void AFCWorldNetServerModule::OnGameServerUnRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

        for (int i = 0; i < xMsg.server_list_size(); ++i)
        {
            const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
            mGameMap.RemoveElement(xData.bus_id());

            ARK_LOG_INFO("GameServer unregistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
        }
    }

    void AFCWorldNetServerModule::OnRefreshGameServerInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

        for (int i = 0; i < xMsg.server_list_size(); ++i)
        {
            const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

            ARK_SHARE_PTR<AFServerData> pServerData = mGameMap.GetElement(xData.bus_id());

            if (nullptr == pServerData)
            {
                pServerData = std::make_shared<AFServerData>();
                mGameMap.AddElement(xData.bus_id(), pServerData);
            }

            pServerData->Init(xClientID, xData);

            ARK_LOG_INFO("GameServer refersh, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
        }

        SynGameToProxy();
    }

    void AFCWorldNetServerModule::OnProxyServerRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

        for (int i = 0; i < xMsg.server_list_size(); ++i)
        {
            const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

            ARK_SHARE_PTR<AFServerData> pServerData = mProxyMap.GetElement(xData.bus_id());

            if (!pServerData)
            {
                pServerData = std::make_shared<AFServerData>();
                mProxyMap.AddElement(xData.bus_id(), pServerData);
            }

            pServerData->Init(xClientID, xData);

            ARK_LOG_INFO("Proxy Registered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
            SynGameToProxy(xClientID);
        }
    }

    void AFCWorldNetServerModule::OnProxyServerUnRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

        for (int i = 0; i < xMsg.server_list_size(); ++i)
        {
            const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

            mGameMap.RemoveElement(xData.bus_id());
            ARK_LOG_INFO("Proxy UnRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
        }
    }

    void AFCWorldNetServerModule::OnRefreshProxyServerInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

        for (int i = 0; i < xMsg.server_list_size(); ++i)
        {
            const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
            ARK_SHARE_PTR<AFServerData> pServerData = mProxyMap.GetElement(xData.bus_id());

            if (nullptr == pServerData)
            {
                pServerData = std::make_shared<AFServerData>();
                mGameMap.AddElement(xData.bus_id(), pServerData);
            }

            pServerData->Init(xClientID, xData);
            ARK_LOG_INFO("Proxy refresh, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
            SynGameToProxy(xClientID);
        }
    }

    int AFCWorldNetServerModule::OnLeaveGameProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        return 0;
    }

    void AFCWorldNetServerModule::OnSocketEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
    {
        if (eEvent == DISCONNECTED)
        {
            ARK_LOG_INFO("Connection closed, id = {}", xClientID.ToString());
            OnClientDisconnect(xClientID);
        }
        else  if (eEvent == CONNECTED)
        {
            ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString());
            OnClientConnected(xClientID);
        }
    }

    void AFCWorldNetServerModule::SynGameToProxy()
    {
        ARK_SHARE_PTR<AFServerData> pServerData = mProxyMap.First();

        while (nullptr != pServerData)
        {
            SynGameToProxy(pServerData->xClient);
            pServerData = mProxyMap.Next();
        }
    }

    void AFCWorldNetServerModule::SynGameToProxy(const AFGUID& xClientID)
    {
        AFMsg::ServerInfoReportList xData;

        ARK_SHARE_PTR<AFServerData> pServerData = mGameMap.First();

        while (nullptr != pServerData)
        {
            AFMsg::ServerInfoReport* pData = xData.add_server_list();
            *pData = pServerData->xData;

            pServerData = mGameMap.Next();
        }

        m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_STS_NET_INFO, xData, xClientID, AFGUID(0));
    }

    void AFCWorldNetServerModule::OnClientDisconnect(const AFGUID& xClientID)
    {
        //不管是game还是proxy都要找出来,替他反注册
        for (ARK_SHARE_PTR<AFServerData> pServerData = mGameMap.First(); nullptr != pServerData; pServerData = mGameMap.Next())
        {
            if (xClientID == pServerData->xClient)
            {
                pServerData->xData.set_logic_status(AFMsg::EST_CRASH);
                pServerData->xClient = 0;

                SynGameToProxy();
                return;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        int nServerID = 0;
        for (ARK_SHARE_PTR<AFServerData> pServerData = mProxyMap.First(); pServerData != nullptr; pServerData = mProxyMap.Next())
        {
            if (xClientID == pServerData->xClient)
            {
                nServerID = pServerData->xData.bus_id();
                break;
            }
        }

        mProxyMap.RemoveElement(nServerID);
    }

    void AFCWorldNetServerModule::OnClientConnected(const AFGUID& xClientID)
    {
        //log
    }

    void AFCWorldNetServerModule::LogGameServer()
    {
        if (mnLastCheckTime + 10 * 1000 > GetPluginManager()->GetNowTime())
        {
            return;
        }

        mnLastCheckTime = GetPluginManager()->GetNowTime();
        //////////////////////////////////////////////////////////////////////////
        ARK_LOG_INFO("Begin Log GameServer Info---------------------------");

        for (ARK_SHARE_PTR<AFServerData> pGameData = mGameMap.First(); pGameData != nullptr; pGameData = mGameMap.Next())
        {
            ARK_LOG_INFO("ID[{}] State[{}] IP[{}] xClient[{}]",
                         pGameData->xData.bus_id(),
                         AFMsg::EServerState_Name(pGameData->xData.logic_status()),
                         pGameData->xData.url(),
                         pGameData->xClient.nLow);
        }

        ARK_LOG_INFO("End Log GameServer Info---------------------------");
        //////////////////////////////////////////////////////////////////////////
        ARK_LOG_INFO("Begin Log ProxyServer Info---------------------------");

        for (ARK_SHARE_PTR<AFServerData> pProxyData = mProxyMap.First(); pProxyData != nullptr; pProxyData = mProxyMap.Next())
        {
            ARK_LOG_INFO("ID[{}] State[{}] IP[{}] xClient[{}]",
                         pProxyData->xData.bus_id(),
                         AFMsg::EServerState_Name(pProxyData->xData.logic_status()),
                         pProxyData->xData.url(),
                         pProxyData->xClient.nLow);
        }

        ARK_LOG_INFO("End Log ProxyServer Info---------------------------");
    }


    void AFCWorldNetServerModule::OnOnlineProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::RoleOnlineNotify);
    }

    void AFCWorldNetServerModule::OnOfflineProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::RoleOfflineNotify);
    }

    bool AFCWorldNetServerModule::SendMsgToGame(const int nGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer)
    {
        ARK_SHARE_PTR<AFServerData> pData = mGameMap.GetElement(nGameID);

        if (nullptr != pData)
        {
            m_pNetServer->SendPBMsg(eMsgID, xData, pData->xClient, nPlayer);
        }

        return true;
    }

    bool AFCWorldNetServerModule::SendMsgToGame(const AFIDataList& argObjectVar, const AFIDataList& argGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData)
    {
        if (argGameID.GetCount() != argObjectVar.GetCount())
        {
            return false;
        }

        for (size_t i = 0; i < argObjectVar.GetCount(); i++)
        {
            const AFGUID& identOther = argObjectVar.Object(i);
            const int64_t  nGameID = argGameID.Int(i);

            SendMsgToGame(nGameID, eMsgID, xData, identOther);
        }

        return true;
    }

    bool AFCWorldNetServerModule::SendMsgToPlayer(const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer)
    {
        int nGameID = GetPlayerGameID(nPlayer);

        if (nGameID < 0)
        {
            return false;
        }

        return SendMsgToGame(nGameID, eMsgID, xData, nPlayer);
    }

    int AFCWorldNetServerModule::OnObjectListEnter(const AFIDataList& self, const AFIDataList& argVar)
    {
        if (self.GetCount() <= 0 || argVar.GetCount() <= 0)
        {
            return 0;
        }

        AFMsg::AckEntityEnterList xEntityEnterList;

        for (size_t i = 0; i < argVar.GetCount(); i++)
        {
            AFGUID identOld = argVar.Object(i);

            //排除空对象
            if (!identOld.IsNULL())
            {
                AFMsg::EntityEnterInfo* pEnter = xEntityEnterList.add_entity_list();
                *(pEnter->mutable_object_guid()) = AFIMsgModule::GUIDToPB(identOld);
                pEnter->set_career_type(m_pKernelModule->GetNodeInt(identOld, "Job"));
                pEnter->set_player_state(m_pKernelModule->GetNodeInt(identOld, "State"));
                pEnter->set_config_id(m_pKernelModule->GetNodeString(identOld, "ConfigID"));
                pEnter->set_scene_id(m_pKernelModule->GetNodeInt(identOld, "SceneID"));
                pEnter->set_class_id(m_pKernelModule->GetNodeString(identOld, "ClassName"));
            }
        }

        if (xEntityEnterList.entity_list_size() <= 0)
        {
            return 0;
        }

        for (size_t i = 0; i < self.GetCount(); i++)
        {
            AFGUID ident = self.Object(i);

            if (!ident.IsNULL())
            {
                //可能在不同的网关呢,得到后者所在的网关FD
                SendMsgToPlayer(AFMsg::EGMI_ACK_ENTITY_ENTER, xEntityEnterList, ident);
            }
        }

        return 1;
    }

    int AFCWorldNetServerModule::OnObjectListLeave(const AFIDataList& self, const AFIDataList& argVar)
    {
        if (self.GetCount() <= 0 || argVar.GetCount() <= 0)
        {
            return 1;
        }

        AFMsg::AckEntityLeaveList xEntityLeaveList;

        for (size_t i = 0; i < argVar.GetCount(); i++)
        {
            AFGUID identOld = argVar.Object(i);

            //排除空对象
            if (!identOld.IsNULL())
            {
                AFMsg::PBGUID* pIdent = xEntityLeaveList.add_entity_list();
                *pIdent = AFIMsgModule::GUIDToPB(argVar.Object(i));
            }
        }

        for (size_t i = 0; i < self.GetCount(); i++)
        {
            AFGUID ident = self.Object(i);

            if (ident.IsNULL())
            {
                //可能在不同的网关呢,得到后者所在的网关FD
                SendMsgToPlayer(AFMsg::EGMI_ACK_ENTITY_LEAVE, xEntityLeaveList, ident);
            }
        }

        return 0;
    }

    int AFCWorldNetServerModule::OnViewDataNodeEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self)
    {
        if (argVar.GetCount() <= 0 || self.IsNULL())
        {
            return 1;
        }

        AFMsg::MultiEntityDataNodeList xPublicMsg;
        ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

        if (nullptr == pEntity)
        {
            return 1;
        }

        ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager = pEntity->GetNodeManager();
        AFFeatureType nFeature;
        nFeature[AFDataNode::PF_PUBLIC] = 1;
        AFIMsgModule::NodeListToPB(self, pNodeManager, *xPublicMsg.add_multi_entity_data_node_list(), nFeature);

        for (size_t i = 0; i < argVar.GetCount(); i++)
        {
            AFGUID identOther = argVar.Object(i);
            const int64_t nGameID = argGameID.Int(i);

            if (self != identOther)
            {
                SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, xPublicMsg, identOther);
            }
        }

        return 0;
    }

    int AFCWorldNetServerModule::OnSelfDataNodeEnter(const AFGUID& self, const AFIDataList& argGameID)
    {
        if (self.IsNULL())
        {
            return 1;
        }

        const int64_t nGameID = argGameID.Int(0);

        ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

        if (nullptr == pEntity)
        {
            return 1;
        }

        AFMsg::MultiEntityDataNodeList xPrivateMsg;
        ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager = pEntity->GetNodeManager();
        AFFeatureType nFeature;
        nFeature[AFDataNode::PF_PRIVATE] = 1;
        AFIMsgModule::NodeListToPB(self, pNodeManager, *xPrivateMsg.add_multi_entity_data_node_list(), nFeature);

        SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, xPrivateMsg, self);
        return 0;
    }

    int AFCWorldNetServerModule::OnSelfDataTableEnter(const AFGUID& self, const AFIDataList& argGameID)
    {
        if (self.IsNULL())
        {
            return 1;
        }

        const int64_t nGameID = argGameID.Int(0);
        AFMsg::MultiEntityDataTableList xPrivateMsg;

        ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

        if (nullptr == pEntity)
        {
            return 1;
        }

        AFFeatureType nFeature;
        nFeature[AFDataNode::PF_PRIVATE] = 1;
        ARK_SHARE_PTR<AFIDataTableManager> pTableManager = pEntity->GetTableManager();
        AFIMsgModule::TableListToPB(self, pTableManager, *xPrivateMsg.add_multi_entity_data_table_list(), nFeature);
        SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, xPrivateMsg, self);
        return 0;
    }

    int AFCWorldNetServerModule::OnViewDataTableEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self)
    {
        if (argVar.GetCount() <= 0 || self.IsNULL())
        {
            return 1;
        }

        AFMsg::MultiEntityDataTableList xPublicMsg;

        ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

        if (nullptr == pEntity)
        {
            return 1;
        }

        ARK_SHARE_PTR<AFIDataTableManager> pTableManager = pEntity->GetTableManager();

        AFFeatureType nFeature;
        nFeature[AFDataNode::PF_PUBLIC] = 1;
        AFIMsgModule::TableListToPB(self, pTableManager, *xPublicMsg.add_multi_entity_data_table_list(), nFeature);

        for (size_t i = 0; i < argVar.GetCount(); i++)
        {
            AFGUID identOther = argVar.Object(i);
            const int64_t nGameID = argGameID.Int(i);

            if (self != identOther && xPublicMsg.multi_entity_data_table_list_size() > 0)
            {
                SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, xPublicMsg, identOther);
            }
        }

        return 0;
    }

    ARK_SHARE_PTR<AFServerData> AFCWorldNetServerModule::GetSuitProxyForEnter()
    {
        return mProxyMap.First();
    }

    AFINetServerService* AFCWorldNetServerModule::GetNetServer()
    {
        return m_pNetServer;
    }

    int AFCWorldNetServerModule::GetPlayerGameID(const AFGUID self)
    {
        //do something
        return -1;
    }

}