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

#include "AFCWorldNetServerModule.h"
#include "AFWorldNetServerPlugin.h"
#include "SDK/Proto/AFProtoCPP.hpp"
#include "SDK/Core/AFDataNode.h"

bool AFCWorldNetServerModule::Init()
{
    m_pNetModule = ARK_NEW AFINetServerModule(pPluginManager);
    return true;
}

bool AFCWorldNetServerModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pWorldLogicModule = pPluginManager->FindModule<AFIWorldLogicModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();

    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_PTWG_PROXY_REFRESH, this, &AFCWorldNetServerModule::OnRefreshProxyServerInfoProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_PTWG_PROXY_REGISTERED, this, &AFCWorldNetServerModule::OnProxyServerRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_PTWG_PROXY_UNREGISTERED, this, &AFCWorldNetServerModule::OnProxyServerUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_GTW_GAME_REGISTERED, this, &AFCWorldNetServerModule::OnGameServerRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_GTW_GAME_UNREGISTERED, this, &AFCWorldNetServerModule::OnGameServerUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_GTW_GAME_REFRESH, this, &AFCWorldNetServerModule::OnRefreshGameServerInfoProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_ACK_ONLINE_NOTIFY, this, &AFCWorldNetServerModule::OnOnlineProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_ACK_OFFLINE_NOTIFY, this, &AFCWorldNetServerModule::OnOfflineProcess);

    m_pNetModule->AddEventCallBack(this, &AFCWorldNetServerModule::OnSocketEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr == xLogicClass)
    {
        return false;
    }

    AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
    std::string strConfigName;
    for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
    {
        const int nServerType = m_pElementModule->GetNodeInt(strConfigName, "Type");
        const int nServerID = m_pElementModule->GetNodeInt(strConfigName, "ServerID");
        if(nServerType == ARK_SERVER_TYPE::ARK_ST_WORLD && pPluginManager->AppID() == nServerID)
        {
            const int nPort = m_pElementModule->GetNodeInt(strConfigName, "Port");
            const int nMaxConnect = m_pElementModule->GetNodeInt(strConfigName, "MaxOnline");
            const int nCpus = m_pElementModule->GetNodeInt(strConfigName, "CpuCount");
            const std::string strServerName(m_pElementModule->GetNodeString(strConfigName, "Name"));
            const std::string strIP(m_pElementModule->GetNodeString(strConfigName, "IP"));

            int nRet = m_pNetModule->Start(nMaxConnect, strIP, nPort, nCpus, nServerID);
            if(nRet < 0)
            {
                ARK_LOG_ERROR("Cannot init server net, Port = {}", nPort);
                ARK_ASSERT(nRet, "Cannot init server net", __FILE__, __FUNCTION__);
                exit(0);
            }
        }
    }

    return true;
}

bool AFCWorldNetServerModule::Shut()
{
    return true;
}

bool AFCWorldNetServerModule::Update()
{
    LogGameServer();

    return m_pNetModule->Update();
}

void AFCWorldNetServerModule::OnGameServerRegisteredProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<ServerData> pServerData =  mGameMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = std::make_shared<ServerData>();
            mGameMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xClient = xClientID;
        *(pServerData->pData) = xData;

        ARK_LOG_INFO("GameServerRegistered, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name().c_str());
    }

    SynGameToProxy();
}

void AFCWorldNetServerModule::OnGameServerUnRegisteredProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        mGameMap.RemoveElement(xData.server_id());

        ARK_LOG_INFO("GameServer unregistered, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name().c_str());
    }
}

void AFCWorldNetServerModule::OnRefreshGameServerInfoProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        ARK_SHARE_PTR<ServerData> pServerData =  mGameMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = std::make_shared<ServerData>();
            mGameMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xClient = xClientID;
        *(pServerData->pData) = xData;

        ARK_LOG_INFO("GameServer refersh, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name().c_str());
    }

    SynGameToProxy();
}

void AFCWorldNetServerModule::OnProxyServerRegisteredProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        ARK_SHARE_PTR<ServerData> pServerData =  mProxyMap.GetElement(xData.server_id());
        if(!pServerData)
        {
            pServerData = std::make_shared<ServerData>();
            mProxyMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xClient = xClientID;
        *(pServerData->pData) = xData;

        ARK_LOG_INFO("Proxy Registered, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name().c_str());
        SynGameToProxy(xClientID);
    }
}

void AFCWorldNetServerModule::OnProxyServerUnRegisteredProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        mGameMap.RemoveElement(xData.server_id());
        ARK_LOG_INFO("Proxy UnRegistered, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name().c_str());
    }
}

void AFCWorldNetServerModule::OnRefreshProxyServerInfoProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        ARK_SHARE_PTR<ServerData> pServerData =  mProxyMap.GetElement(xData.server_id());
        if(nullptr == pServerData)
        {
            pServerData = std::make_shared<ServerData>();
            mGameMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->xClient = xClientID;
        *(pServerData->pData) = xData;

        ARK_LOG_INFO("Proxy refresh, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name().c_str());
        SynGameToProxy(xClientID);
    }
}

int AFCWorldNetServerModule::OnLeaveGameProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    return 0;
}

void AFCWorldNetServerModule::OnSocketEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if(eEvent == DISCONNECTED)
    {
        ARK_LOG_INFO("Connection closed, id = {}", xClientID.ToString().c_str());
        OnClientDisconnect(xClientID);
    }
    else  if(eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString().c_str());
        OnClientConnected(xClientID);
    }
}

void AFCWorldNetServerModule::SynGameToProxy()
{
    AFMsg::ServerInfoReportList xData;

    ARK_SHARE_PTR<ServerData> pServerData =  mProxyMap.First();
    while(nullptr != pServerData)
    {
        SynGameToProxy(pServerData->xClient);

        pServerData = mProxyMap.Next();
    }
}

void AFCWorldNetServerModule::SynGameToProxy(const AFGUID& xClientID)
{
    AFMsg::ServerInfoReportList xData;

    ARK_SHARE_PTR<ServerData> pServerData =  mGameMap.First();
    while(nullptr != pServerData)
    {
        AFMsg::ServerInfoReport* pData = xData.add_server_list();
        *pData = *(pServerData->pData);

        pServerData = mGameMap.Next();
    }

    m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_STS_NET_INFO, xData, xClientID, AFGUID(0));
}

void AFCWorldNetServerModule::OnClientDisconnect(const AFGUID& xClientID)
{
    //不管是game还是proxy都要找出来,替他反注册
    ARK_SHARE_PTR<ServerData> pServerData =  mGameMap.First();
    while(nullptr != pServerData)
    {
        if(xClientID == pServerData->xClient)
        {
            pServerData->pData->set_server_state(AFMsg::EST_CRASH);
            pServerData->xClient = 0;

            SynGameToProxy();
            return;
        }

        pServerData = mGameMap.Next();
    }

    //////////////////////////////////////////////////////////////////////////

    int nServerID = 0;
    pServerData =  mProxyMap.First();
    while(pServerData)
    {
        if(xClientID == pServerData->xClient)
        {
            nServerID = pServerData->pData->server_id();
            break;
        }

        pServerData = mProxyMap.Next();
    }

    mProxyMap.RemoveElement(nServerID);
}

void AFCWorldNetServerModule::OnClientConnected(const AFGUID& xClientID)
{

}

void AFCWorldNetServerModule::LogGameServer()
{
    if(mnLastCheckTime + 10 * 1000 > GetPluginManager()->GetNowTime())
    {
        return;
    }

    mnLastCheckTime = GetPluginManager()->GetNowTime();
    //////////////////////////////////////////////////////////////////////////
    ARK_LOG_INFO("Begin Log GameServer Info---------------------------");

    for(ARK_SHARE_PTR<ServerData> pGameData = mGameMap.First(); pGameData != nullptr; pGameData = mGameMap.Next())
    {
        ARK_LOG_INFO("Type[{}] ID[{}] State[{}] IP[{}] xClient[{}]",
                     pGameData->pData->server_type(),
                     pGameData->pData->server_id(),
                     AFMsg::EServerState_Name(pGameData->pData->server_state()).c_str(),
                     pGameData->pData->server_ip().c_str(),
                     pGameData->xClient.nLow);
    }

    ARK_LOG_INFO("End Log GameServer Info---------------------------");
    //////////////////////////////////////////////////////////////////////////
    ARK_LOG_INFO("Begin Log ProxyServer Info---------------------------");

    for(ARK_SHARE_PTR<ServerData> pGameData = mProxyMap.First(); pGameData != nullptr; pGameData = mProxyMap.Next())
    {
        ARK_LOG_INFO("Type[{}] ID[{}] State[{}] IP[{}] xClient[{}]",
                     pGameData->pData->server_type(),
                     pGameData->pData->server_id(),
                     AFMsg::EServerState_Name(pGameData->pData->server_state()).c_str(),
                     pGameData->pData->server_ip().c_str(),
                     pGameData->xClient.nLow);
    }
    ARK_LOG_INFO("End Log ProxyServer Info---------------------------");
}


void AFCWorldNetServerModule::OnOnlineProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    CLIENT_MSG_PROCESS_NO_OBJECT(nMsgID, msg, nLen, AFMsg::RoleOnlineNotify);

}

void AFCWorldNetServerModule::OnOfflineProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    CLIENT_MSG_PROCESS_NO_OBJECT(nMsgID, msg, nLen, AFMsg::RoleOfflineNotify);
}

bool AFCWorldNetServerModule::SendMsgToGame(const int nGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer)
{
    ARK_SHARE_PTR<ServerData> pData = mGameMap.GetElement(nGameID);
    if(nullptr != pData)
    {
        m_pNetModule->SendMsgPB(eMsgID, xData, pData->xClient, nPlayer);
    }

    return true;
}

bool AFCWorldNetServerModule::SendMsgToGame(const AFIDataList& argObjectVar, const AFIDataList& argGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData)
{
    if(argGameID.GetCount() != argObjectVar.GetCount())
    {
        return false;
    }

    for(size_t i = 0; i < argObjectVar.GetCount(); i++)
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
    if(nGameID < 0)
    {
        return false;
    }

    return SendMsgToGame(nGameID, eMsgID, xData, nPlayer);
}

int AFCWorldNetServerModule::OnObjectListEnter(const AFIDataList& self, const AFIDataList& argVar)
{
    if(self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 0;
    }

    AFMsg::AckEntityEnterList xEntityEnterList;
    for(size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Object(i);
        //排除空对象
        if(identOld.IsNULL())
        {
            continue;
        }

        AFMsg::EntityEnterInfo * pEnter = xEntityEnterList.add_entity_list();
        *(pEnter->mutable_object_guid()) = AFINetServerModule::GUIDToPB(identOld);
        //*pEntryInfo->mutable_pos() = AFINetServerModule::GUIDToPB(m_pKernelModule->GetObject(identOld, "Pos")); todo
        pEnter->set_career_type(m_pKernelModule->GetNodeInt(identOld, "Job"));
        pEnter->set_player_state(m_pKernelModule->GetNodeInt(identOld, "State"));
        pEnter->set_config_id(m_pKernelModule->GetNodeString(identOld, "ConfigID"));
        pEnter->set_scene_id(m_pKernelModule->GetNodeInt(identOld, "SceneID"));
        pEnter->set_class_id(m_pKernelModule->GetNodeString(identOld, "ClassName"));

    }

    if(xEntityEnterList.entity_list_size() <= 0)
    {
        return 0;
    }

    for(size_t i = 0; i < self.GetCount(); i++)
    {
        AFGUID ident = self.Object(i);
        if(ident.IsNULL())
        {
            continue;
        }

        //可能在不同的网关呢,得到后者所在的网关FD
        SendMsgToPlayer(AFMsg::EGMI_ACK_ENTITY_ENTER, xEntityEnterList, ident);
    }

    return 1;
}


int AFCWorldNetServerModule::OnObjectListLeave(const AFIDataList& self, const AFIDataList& argVar)
{
    if(self.GetCount() <= 0 || argVar.GetCount() <= 0)
    {
        return 0;
    }

    AFMsg::AckEntityLeaveList xEntityLeaveList;
    for(size_t i = 0; i < argVar.GetCount(); i++)
    {
        AFGUID identOld = argVar.Object(i);
        //排除空对象
        if(identOld.IsNULL())
        {
            continue;
        }

        AFMsg::PBGUID* pIdent = xEntityLeaveList.add_entity_list();
        *pIdent = AFINetServerModule::GUIDToPB(argVar.Object(i));
    }

    for(size_t i = 0; i < self.GetCount(); i++)
    {
        AFGUID ident = self.Object(i);
        if(ident.IsNULL())
        {
            continue;
        }
        //可能在不同的网关呢,得到后者所在的网关FD
        SendMsgToPlayer(AFMsg::EGMI_ACK_ENTITY_LEAVE, xEntityLeaveList, ident);
    }

    return 1;
}


int AFCWorldNetServerModule::OnDataTableEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self)
{
    if(argVar.GetCount() <= 0 || self.IsNULL())
    {
        return 0;
    }

    if(argVar.GetCount() != argGameID.GetCount())
    {
        return 1;
    }

    AFMsg::MultiEntityDataTableList xPublicMsg;
    AFMsg::MultiEntityDataTableList xPrivateMsg;

    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);
    if(nullptr == pEntity)
    {
        return 1;
    }

    AFMsg::EntityDataTableList* pPublicData = NULL;
    AFMsg::EntityDataTableList* pPrivateData = NULL;

    ARK_SHARE_PTR<AFIDataTableManager> pTableManager = pEntity->GetTableManager();
    size_t nTableCount = pTableManager->GetCount();
    for(size_t i = 0; i < nTableCount; ++i)
    {
        AFDataTable* pTable = pTableManager->GetTableByIndex(i);
        if(pTable == nullptr)
        {
            continue;
        }

        if(!pTable->IsPublic() && !pTable->IsPrivate())
        {
            continue;
        }

        AFMsg::EntityDataTableBase* pPrivateTableBase = NULL;
        AFMsg::EntityDataTableBase* pPublicTableBase = NULL;
        if(pTable->IsPublic())
        {
            if(!pPublicData)
            {
                pPublicData = xPublicMsg.add_multi_entity_data_table_list();
                *(pPublicData->mutable_entity_id()) = AFINetServerModule::GUIDToPB(self);
            }
            pPublicTableBase = pPublicData->add_data_table_list();
            pPublicTableBase->set_table_name(pTable->GetName());

            OnDataTableEnterPack(pTable, pPublicTableBase);
        }

        if(pTable->IsPrivate())
        {
            if(!pPrivateData)
            {
                pPrivateData = xPrivateMsg.add_multi_entity_data_table_list();
                *(pPrivateData->mutable_entity_id()) = AFINetServerModule::GUIDToPB(self);
            }
            pPrivateTableBase = pPrivateData->add_data_table_list();
            pPrivateTableBase->set_table_name(pTable->GetName());

            OnDataTableEnterPack(pTable, pPrivateTableBase);
        }
    }

    for(size_t i = 0; i < argVar.GetCount(); i++)
    {
        const AFGUID& identOther = argVar.Object(i);
        const int64_t nGameID = argGameID.Int(i);
        if(self == identOther)
        {
            if(xPrivateMsg.multi_entity_data_table_list_size() > 0)
            {
                SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, xPrivateMsg, identOther);
            }
        }
        else
        {
            if(xPublicMsg.multi_entity_data_table_list_size() > 0)
            {
                SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_TABLE_ENTER, xPublicMsg, identOther);
            }
        }
    }

    return 0;
}

bool AFCWorldNetServerModule::OnDataTableEnterPack(AFDataTable* pTable, AFMsg::EntityDataTableBase* pTableBase)
{
    if(!pTable || !pTableBase)
    {
        return false;
    }

    for(size_t i = 0; i < pTable->GetRowCount(); i ++)
    {
        //不管public还是private都要加上，不然public广播了那不是private就广播不了了
        AFMsg::DataTableAddRow* pAddRowStruct = pTableBase->add_row();
        pAddRowStruct->set_row(i);

        AFCDataList valueList;
        pTable->QueryRow(i, valueList);

        for(size_t j = 0; j < valueList.GetCount(); j++)
        {
            AFMsg::PBCellData* pCell = pAddRowStruct->add_cell_list();
            AFINetServerModule::TableCellToPBCell(valueList, i, j, *pCell);
        }
    }

    return true;
}

int AFCWorldNetServerModule::OnDataNodeEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self)
{
    if(argVar.GetCount() <= 0 || self.IsNULL())
    {
        return 0;
    }

    if(argVar.GetCount() != argGameID.GetCount())
    {
        return 1;
    }

    //分为自己和外人
    //1.public发送给所有人
    //2.如果自己在列表中，再次发送private数据
    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);
    if(nullptr == pEntity)
    {
        return 0;
    }

    AFMsg::MultiEntityDataNodeList xPublicMsg;
    AFMsg::MultiEntityDataNodeList xPrivateMsg;
    AFMsg::EntityDataNodeList* pPublicData = xPublicMsg.add_multi_entity_data_node_list();
    AFMsg::EntityDataNodeList* pPrivateData = xPrivateMsg.add_multi_entity_data_node_list();

    *(pPublicData->mutable_entity_id()) = AFINetServerModule::GUIDToPB(self);
    *(pPrivateData->mutable_entity_id()) = AFINetServerModule::GUIDToPB(self);

    ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager = pEntity->GetNodeManager();

    for(size_t i = 0; i < pNodeManager->GetNodeCount(); i++)
    {
        AFDataNode* pNode = pNodeManager->GetNodeByIndex(i);
        if(nullptr == pNode)
        {
            continue;
        }
        if(pNode->Changed())
        {
            if(pNode->IsPublic())
            {
                AFMsg::PBNodeData* pData = pPublicData->add_data_node_list();
                AFINetServerModule::DataNodeToPBNode(pNode->GetValue(), pNode->GetName().c_str(), *pData);
            }

            if(pNode->IsPrivate())
            {
                AFMsg::PBNodeData* pData = pPrivateData->add_data_node_list();
                AFINetServerModule::DataNodeToPBNode(pNode->GetValue(), pNode->GetName().c_str(), *pData);
            }
        }
    }

    for(size_t i = 0; i < argVar.GetCount(); i++)
    {
        const AFGUID& identOther = argVar.Object(i);
        const int64_t nGameID = argGameID.Int(i);
        if(self == identOther)
        {
            //找到他所在网关的FD
            SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, xPrivateMsg, identOther);
        }
        else
        {
            SendMsgToGame(nGameID, AFMsg::EGMI_ACK_ENTITY_DATA_NODE_ENTER, xPublicMsg, identOther);
        }
    }

    return 0;
}

ARK_SHARE_PTR<ServerData> AFCWorldNetServerModule::GetSuitProxyForEnter()
{
    return mProxyMap.First();
}

AFINetServerModule* AFCWorldNetServerModule::GetNetModule()
{
    return m_pNetModule;
}

int AFCWorldNetServerModule::GetPlayerGameID(const AFGUID self)
{
    //to do
    return -1;
}