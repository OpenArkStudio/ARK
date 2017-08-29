/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file      AFCProxyServerNet_ServerModule.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCProxyServerNet_ServerModule                                                  *
*****************************************************************************/
#include "AFCProxyServerNet_ServerModule.h"
#include "AFProxyNetServerPlugin.h"
#include "SDK/Interface/AFIKernelModule.h"

bool AFCProxyServerNet_ServerModule::Init()
{
    m_pNetModule = ARK_NEW AFINetServerModule(pPluginManager);
    return true;
}

bool AFCProxyServerNet_ServerModule::AfterInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pProxyToWorldModule = pPluginManager->FindModule<AFIProxyServerToWorldModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pUUIDModule = pPluginManager->FindModule<AFIGUIDModule>();
    m_pProxyServerToGameModule = pPluginManager->FindModule<AFIProxyServerToGameModule>();

    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_CONNECT_KEY, this, &AFCProxyServerNet_ServerModule::OnConnectKeyProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_WORLD_LIST, this, &AFCProxyServerNet_ServerModule::OnReqServerListProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_SELECT_SERVER, this, &AFCProxyServerNet_ServerModule::OnSelectServerProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_ROLE_LIST, this, &AFCProxyServerNet_ServerModule::OnReqRoleListProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_CREATE_ROLE, this, &AFCProxyServerNet_ServerModule::OnReqCreateRoleProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_DELETE_ROLE, this, &AFCProxyServerNet_ServerModule::OnReqDelRoleProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_ENTER_GAME, this, &AFCProxyServerNet_ServerModule::OnReqEnterGameServer);
    m_pNetModule->AddReceiveCallBack(this, &AFCProxyServerNet_ServerModule::OnOtherMessage);

    m_pNetModule->AddEventCallBack(this, &AFCProxyServerNet_ServerModule::OnSocketClientEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetPropertyInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetPropertyInt(strConfigName, "ServerID");
            if(nServerType == NF_SERVER_TYPES::NF_ST_PROXY && pPluginManager->AppID() == nServerID)
            {
                const int nPort = m_pElementModule->GetPropertyInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetPropertyInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetPropertyInt(strConfigName, "CpuCount");
                const std::string strName(m_pElementModule->GetPropertyString(strConfigName, "Name"));
                const std::string strIP(m_pElementModule->GetPropertyString(strConfigName, "IP"));

                m_pUUIDModule->SetWorkerAndDatacenter(nServerID, nServerID);

                int nRet = m_pNetModule->Initialization(nMaxConnect, strIP, nPort, nCpus, nServerID);
                if(nRet < 0)
                {
                    std::ostringstream strLog;
                    strLog << "Cannot init server net, Port = " << nPort;
                    m_pLogModule->LogError(NULL_GUID, strLog, __FUNCTION__, __LINE__);
                    ARK_ASSERT(nRet, "Cannot init server net", __FILE__, __FUNCTION__);
                    exit(0);
                }
            }
        }
    }

    return true;
}

bool AFCProxyServerNet_ServerModule::Shut()
{

    return true;
}

bool AFCProxyServerNet_ServerModule::Execute()
{
    return m_pNetModule->Execute();
}

int AFCProxyServerNet_ServerModule::HB_OnConnectCheckTime(const AFGUID& self, const std::string& strHeartBeat, const float fTime, const int nCount, const AFIDataList& var)
{
    m_pKernelModule->DestroyObject(self);

    return 0;
}

void AFCProxyServerNet_ServerModule::OnOtherMessage(const int nMsgID, const char * msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFMsg::MsgBase xMsg;
    if(!xMsg.ParseFromArray(msg, nLen))
    {
        char szData[MAX_PATH] = { 0 };
        sprintf(szData, "Parse Message Failed from Packet to MsgBase, MessageID: %d\n", nMsgID);

        return;
    }

    ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(!pSessionData || pSessionData->mnLogicState <= 0 || pSessionData->mnGameID <= 0)
    {
        //state error
        return;
    }
    if(xMsg.has_hash_ident())
    {
        //special for distributed
        if(!pSessionData->mnHashIdentID.IsNull())
        {
            AFCMachineNode xNode;
            if(mxConsistentHash.GetSuitNode(pSessionData->mnHashIdentID.ToString(), xNode))
            {
                m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(xNode.GetDataID(), nMsgID, msg, nLen);
            }
        }
        else
        {
            AFGUID xHashIdent = AFINetServerModule::PBToNF(xMsg.hash_ident());

            AFCMachineNode xNode;
            if(mxConsistentHash.GetSuitNode(xHashIdent.ToString(), xNode))
            {
                m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(xNode.GetDataID(), nMsgID, msg, nLen);
            }
        }
    }
    else
    {
        m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, nMsgID, msg, nLen);
    }
}

void AFCProxyServerNet_ServerModule::OnConnectKeyProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqAccountLogin xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    bool bRet = m_pProxyToWorldModule->VerifyConnectData(xMsg.account(), xMsg.security_code());
    if(bRet)
    {
        //可以进入,设置标志，选单服,心跳延迟,进入gs创建角色和删除角色,这里只是转发
        ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData)
        {
            pSessionData->mnLogicState = 1;
            pSessionData->mstrAccout  = xMsg.account();

            AFMsg::AckEventResult xSendMsg;
            xSendMsg.set_event_code(AFMsg::EGEC_VERIFY_KEY_SUCCESS);
            *xSendMsg.mutable_event_client() = AFINetServerModule::NFToPB(pSessionData->mnClientID);//让前端记得自己的fd，后面有一些验证
            *xSendMsg.mutable_event_object() = AFINetServerModule::NFToPB(nPlayerID);

            m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_CONNECT_KEY, xSendMsg, xClientID, nPlayerID);
        }
    }
    else
    {
        m_pNetModule->GetNet()->CloseNetObject(xClientID);
    }
}

void AFCProxyServerNet_ServerModule::OnSocketClientEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nSeverID)
{
    if(eEvent == DISCONNECTED)
    {
        m_pLogModule->LogInfo(xClientID, "NF_NET_EVENT_EOF", "Connection closed", __FUNCTION__, __LINE__);
        OnClientDisconnect(xClientID);
    }
    else  if(eEvent == CONNECTED)
    {
        m_pLogModule->LogInfo(xClientID, "NF_NET_EVENT_CONNECTED", "connectioned success", __FUNCTION__, __LINE__);
        OnClientConnected(xClientID);
    }
}

void AFCProxyServerNet_ServerModule::OnClientDisconnect(const AFGUID& xClientID)
{
    ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(pSessionData)
    {
        if(pSessionData->mnGameID > 0)
        {
            if(!pSessionData->mnUserID.IsNull())
            {
                //掉线
                AFMsg::ReqLeaveGameServer xData;

                AFMsg::MsgBase xMsg;
                //playerid主要是网关转发消息的时候做识别使用，其他使用不使用
                *xMsg.mutable_player_id() = AFINetServerModule::NFToPB(pSessionData->mnUserID);

                if(!xData.SerializeToString(xMsg.mutable_msg_data()))
                {
                    return;
                }

                std::string strMsg;
                if(!xMsg.SerializeToString(&strMsg))
                {
                    return;
                }

                m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, AFMsg::EGameMsgID::EGMI_REQ_LEAVE_GAME, strMsg);
            }
        }

        mmSessionData.RemoveElement(xClientID);
    }
}

void AFCProxyServerNet_ServerModule::OnSelectServerProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqSelectServer xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    ARK_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xMsg.world_id());
    if(pServerData && ConnectDataState::NORMAL == pServerData->eState)
    {
        ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData)
        {
            //now this client bind a game server, after this time, all message will be sent to this game server who bind with client
            pSessionData->mnGameID = xMsg.world_id();

            AFMsg::AckEventResult xMsg;
            xMsg.set_event_code(AFMsg::EGameEventCode::EGEC_SELECTSERVER_SUCCESS);
            m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xMsg, xClientID, nPlayerID);
            return;
        }
    }

    AFMsg::AckEventResult xSendMsg;
    xSendMsg.set_event_code(AFMsg::EGameEventCode::EGEC_SELECTSERVER_FAIL);
    m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xMsg, xClientID, nPlayerID);
}

void AFCProxyServerNet_ServerModule::OnReqServerListProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqServerList xMsg;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    if(xMsg.type() != AFMsg::RSLT_GAMES_ERVER)
    {
        return;
    }

    ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(pSessionData && pSessionData->mnLogicState > 0)
    {
        //ack all gameserver data
        AFMsg::AckServerList xData;
        xData.set_type(AFMsg::RSLT_GAMES_ERVER);

        AFMapEx<int, ConnectData>& xServerList = m_pProxyServerToGameModule->GetClusterModule()->GetServerList();
        ConnectData* pGameData = xServerList.FirstNude();
        while(NULL != pGameData)
        {
            if(ConnectDataState::NORMAL == pGameData->eState)
            {
                AFMsg::ServerInfo* pServerInfo = xData.add_info();

                pServerInfo->set_name(pGameData->strName);
                pServerInfo->set_status(AFMsg::EServerState::EST_NARMAL);
                pServerInfo->set_server_id(pGameData->nGameID);
                pServerInfo->set_wait_count(0);
            }

            pGameData = xServerList.NextNude();
        }

        m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_WORLD_LIST, xData, xClientID, nPlayerID);
    }
}

int AFCProxyServerNet_ServerModule::Transpond(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFMsg::MsgBase xMsg;
    if(!xMsg.ParseFromArray(msg, nLen))
    {
        char szData[MAX_PATH] = { 0 };
        sprintf(szData, "Parse Message Failed from Packet to MsgBase, MessageID: %d\n", nMsgID);

        return false;
    }

    //broadcast many palyer
    for(int i = 0; i < xMsg.player_client_list_size(); ++i)
    {
        ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(AFINetServerModule::PBToNF(xMsg.player_client_list(i)));
        if(pSessionData)
        {
            if(xMsg.has_hash_ident())
            {
                ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
                if(pSessionData)
                {
                    pSessionData->mnHashIdentID = AFINetServerModule::PBToNF(xMsg.hash_ident());
                }
            }

            m_pNetModule->GetNet()->SendMsgWithOutHead(nMsgID, msg, nLen, pSessionData->mnClientID);
        }
    }

    //send message to one player
    if(xMsg.player_client_list_size() <= 0)
    {
        //playerid==ClientID;

        ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(AFINetServerModule::PBToNF(xMsg.player_id()));
        if(pSessionData)
        {
            if(xMsg.has_hash_ident())
            {
                ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
                if(pSessionData)
                {
                    pSessionData->mnHashIdentID = AFINetServerModule::PBToNF(xMsg.hash_ident());
                }
            }

            m_pNetModule->GetNet()->SendMsgWithOutHead(nMsgID, msg, nLen, pSessionData->mnClientID);
        }
    }

    return true;
}

void AFCProxyServerNet_ServerModule::OnClientConnected(const AFGUID& xClientID)
{
    ARK_SHARE_PTR<SessionData> pSessionData(ARK_NEW SessionData());

    pSessionData->mnClientID = xClientID;
    mmSessionData.AddElement(xClientID, pSessionData);
}

void AFCProxyServerNet_ServerModule::OnReqRoleListProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //在没有正式进入游戏之前，nPlayerID都是FD
    AFGUID nPlayerID;
    AFMsg::ReqRoleList xData;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xData, nPlayerID))
    {
        return;
    }

    ARK_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xData.game_id());
    if(pServerData && ConnectDataState::NORMAL == pServerData->eState)
    {
        //数据匹配
        ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData
                && pSessionData->mnLogicState > 0
                && pSessionData->mnGameID == xData.game_id()
                && pSessionData->mstrAccout == xData.account())
        {
            AFMsg::MsgBase xMsg;
            if(!xData.SerializeToString(xMsg.mutable_msg_data()))
            {
                return;
            }

            //playerid主要是网关转发消息的时候做识别使用，其他使用不使用
            xMsg.mutable_player_id()->CopyFrom(AFINetServerModule::NFToPB(pSessionData->mnClientID));

            std::string strMsg;
            if(!xMsg.SerializeToString(&strMsg))
            {
                return;
            }

            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, AFMsg::EGameMsgID::EGMI_REQ_ROLE_LIST, strMsg);
        }
    }
}

void AFCProxyServerNet_ServerModule::OnReqCreateRoleProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //在没有正式进入游戏之前，nPlayerID都是FD


    AFGUID nPlayerID;
    AFMsg::ReqCreateRole xData;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xData, nPlayerID))
    {
        return;
    }

    ARK_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xData.game_id());
    if(pServerData && ConnectDataState::NORMAL == pServerData->eState)
    {
        //数据匹配
        ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData
                && pSessionData->mnLogicState > 0
                && pSessionData->mnGameID == xData.game_id()
                && pSessionData->mstrAccout == xData.account())
        {
            AFMsg::MsgBase xMsg;
            if(!xData.SerializeToString(xMsg.mutable_msg_data()))
            {
                return;
            }

            //playerid主要是网关转发消息的时候做识别使用，其他使用不使用
            xMsg.mutable_player_id()->CopyFrom(AFINetServerModule::NFToPB(pSessionData->mnClientID));

            std::string strMsg;
            if(!xMsg.SerializeToString(&strMsg))
            {
                return;
            }

            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, nMsgID, strMsg);
        }
    }
}

void AFCProxyServerNet_ServerModule::OnReqDelRoleProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //在没有正式进入游戏之前，nPlayerID都是FD
    AFGUID nPlayerID;
    AFMsg::ReqDeleteRole xData;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xData, nPlayerID))
    {
        return;
    }

    ARK_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xData.game_id());
    if(pServerData && ConnectDataState::NORMAL == pServerData->eState)
    {
        //数据匹配
        ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData
                && pSessionData->mnLogicState > 0
                && pSessionData->mnGameID == xData.game_id()
                && pSessionData->mstrAccout == xData.account())
        {
            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, nMsgID, std::string(msg, nLen));
        }
    }
}

void AFCProxyServerNet_ServerModule::OnReqEnterGameServer(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //在没有正式进入游戏之前，nPlayerID都是FD
    AFGUID nPlayerID;
    AFMsg::ReqEnterGameServer xData;
    if(!m_pNetModule->ReceivePB(nMsgID, msg, nLen, xData, nPlayerID))
    {
        return;
    }

    ARK_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xData.game_id());
    if(pServerData && ConnectDataState::NORMAL == pServerData->eState)
    {
        //数据匹配
        ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData
                && pSessionData->mnLogicState > 0
                && pSessionData->mnGameID == xData.game_id()
                && pSessionData->mstrAccout == xData.account()
                && !xData.name().empty()
                && !xData.account().empty())
        {
            AFMsg::MsgBase xMsg;
            if(!xData.SerializeToString(xMsg.mutable_msg_data()))
            {
                return;
            }

            //playerid在进入游戏之前都是FD，其他时候是真实的ID
            xMsg.mutable_player_id()->CopyFrom(AFINetServerModule::NFToPB(pSessionData->mnClientID));

            std::string strMsg;
            if(!xMsg.SerializeToString(&strMsg))
            {
                return;
            }

            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, AFMsg::EGameMsgID::EGMI_REQ_ENTER_GAME, strMsg);
        }
    }
}

int AFCProxyServerNet_ServerModule::EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID)
{
    ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(pSessionData)
    {
        pSessionData->mnUserID = xPlayerID;
    }

    return 0;
}

