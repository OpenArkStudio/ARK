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

#include "AFCProxyNetServerModule.h"
#include "AFProxyNetServerPlugin.h"
#include "SDK/Interface/AFIKernelModule.h"

bool AFCProxyNetServerModule::Init()
{
    m_pNetModule = ARK_NEW AFINetServerModule(pPluginManager);
    return true;
}

bool AFCProxyNetServerModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pProxyToWorldModule = pPluginManager->FindModule<AFIProxyServerToWorldModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pUUIDModule = pPluginManager->FindModule<AFIGUIDModule>();
    m_pProxyServerToGameModule = pPluginManager->FindModule<AFIProxyServerToGameModule>();

    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_CONNECT_KEY, this, &AFCProxyNetServerModule::OnConnectKeyProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_WORLD_LIST, this, &AFCProxyNetServerModule::OnReqServerListProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_SELECT_SERVER, this, &AFCProxyNetServerModule::OnSelectServerProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_ROLE_LIST, this, &AFCProxyNetServerModule::OnReqRoleListProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_CREATE_ROLE, this, &AFCProxyNetServerModule::OnReqCreateRoleProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_DELETE_ROLE, this, &AFCProxyNetServerModule::OnReqDelRoleProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_ENTER_GAME, this, &AFCProxyNetServerModule::OnReqEnterGameServer);
    m_pNetModule->AddReceiveCallBack(this, &AFCProxyNetServerModule::OnOtherMessage);

    m_pNetModule->AddEventCallBack(this, &AFCProxyNetServerModule::OnSocketClientEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetNodeInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetNodeInt(strConfigName, "ServerID");
            if(nServerType == ARK_SERVER_TYPE::ARK_ST_PROXY && pPluginManager->AppID() == nServerID)
            {
                const int nPort = m_pElementModule->GetNodeInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetNodeInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetNodeInt(strConfigName, "CpuCount");
                const std::string strServerName(m_pElementModule->GetNodeString(strConfigName, "Name"));
                const std::string strIP(m_pElementModule->GetNodeString(strConfigName, "IP"));

                m_pUUIDModule->SetGUIDMask(nServerID);

                int nRet = m_pNetModule->Start(nMaxConnect, strIP, nPort, nCpus, nServerID);
                if(nRet < 0)
                {
                    ARK_LOG_ERROR("Cannot init server net, Port = {}", nPort);
                    ARK_ASSERT(nRet, "Cannot init server net", __FILE__, __FUNCTION__);
                    exit(0);
                }
            }
        }
    }

    return true;
}

bool AFCProxyNetServerModule::Shut()
{

    return true;
}

bool AFCProxyNetServerModule::Update()
{
    return m_pNetModule->Update();
}

int AFCProxyNetServerModule::HB_OnConnectCheckTime(const AFGUID& self, const std::string& strHeartBeat, const float fTime, const int nCount, const AFIDataList& var)
{
    m_pKernelModule->DestroyEntity(self);

    return 0;
}

void AFCProxyNetServerModule::OnOtherMessage(const AFIMsgHead& xHead, const int nMsgID, const char * msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(!pSessionData || pSessionData->mnLogicState <= 0 || pSessionData->mnGameID <= 0)
    {
        //state error
        return;
    }

    if(pSessionData->mnUserID != xHead.GetPlayerID())
    {
        //when after entergame xHead.GetPlayerID() is really palyerID
        ARK_LOG_ERROR("xHead.GetPlayerID() is not really palyerID, id = {}", xHead.GetPlayerID().ToString().c_str());
        return;
    }

    m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, nMsgID, msg, nLen, xHead.GetPlayerID());
}

void AFCProxyNetServerModule::OnConnectKeyProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqAccountLogin xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
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
            *xSendMsg.mutable_event_client() = AFINetServerModule::GUIDToPB(pSessionData->mnClientID);//让前端记得自己的fd，后面有一些验证
            *xSendMsg.mutable_event_object() = AFINetServerModule::GUIDToPB(nPlayerID);

            m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_CONNECT_KEY, xSendMsg, xClientID, nPlayerID);
        }
    }
    else
    {
        m_pNetModule->GetNet()->CloseNetEntity(xClientID);
    }
}

void AFCProxyNetServerModule::OnSocketClientEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nSeverID)
{
    if(eEvent == DISCONNECTED)
    {
        ARK_LOG_INFO("Connection closed, id = {}", xClientID.ToString().c_str());
        OnClientDisconnect(xClientID);
    }
    else if(eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString().c_str());
        OnClientConnected(xClientID);
    }
}

void AFCProxyNetServerModule::OnClientDisconnect(const AFGUID& xClientID)
{
    ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(pSessionData)
    {
        if(pSessionData->mnGameID > 0)
        {
            if(!pSessionData->mnUserID.IsNULL())
            {
                AFMsg::ReqLeaveGameServer xData;
                std::string strMsg;
                if(!xData.SerializeToString(&strMsg))
                {
                    return;
                }

                m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, AFMsg::EGameMsgID::EGMI_REQ_LEAVE_GAME, strMsg, pSessionData->mnUserID);
            }
        }

        mmSessionData.RemoveElement(xClientID);
    }
}

void AFCProxyNetServerModule::OnSelectServerProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqSelectServer xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
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

            AFMsg::AckEventResult xResultMsg;
            xResultMsg.set_event_code(AFMsg::EGameEventCode::EGEC_SELECTSERVER_SUCCESS);
            m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xResultMsg, xClientID, nPlayerID);
            return;
        }
    }

    AFMsg::AckEventResult xSendMsg;
    xSendMsg.set_event_code(AFMsg::EGameEventCode::EGEC_SELECTSERVER_FAIL);
    m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xMsg, xClientID, nPlayerID);
}

void AFCProxyNetServerModule::OnReqServerListProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqServerList xMsg;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
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
        while(pGameData != nullptr)
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

int AFCProxyNetServerModule::Transpond(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen)
{
    ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xHead.GetPlayerID());
    if(pSessionData)
    {
        m_pNetModule->GetNet()->SendMsgWithOutHead(nMsgID, msg, nLen, pSessionData->mnClientID, xHead.GetPlayerID());
    }

    return true;
}

int AFCProxyNetServerModule::SendToPlayerClient(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID&  nClientID, const AFGUID&  nPlayer)
{
    m_pNetModule->GetNet()->SendMsgWithOutHead(nMsgID, msg, nLen, nClientID, nPlayer);

    return true;
}

void AFCProxyNetServerModule::OnClientConnected(const AFGUID& xClientID)
{
    ARK_SHARE_PTR<SessionData> pSessionData = std::make_shared<SessionData>();

    pSessionData->mnClientID = xClientID;
    mmSessionData.AddElement(xClientID, pSessionData);
}

void AFCProxyNetServerModule::OnReqRoleListProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //在没有正式进入游戏之前，nPlayerID都是FD
    AFGUID nPlayerID;
    AFMsg::ReqRoleList xData;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xData, nPlayerID))
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
            std::string strMsg;
            if(!xData.SerializeToString(&strMsg))
            {
                return;
            }

            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, AFMsg::EGameMsgID::EGMI_REQ_ROLE_LIST, strMsg, xClientID);
        }
    }
}

void AFCProxyNetServerModule::OnReqCreateRoleProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //在没有正式进入游戏之前，nPlayerID都是FD
    AFGUID nPlayerID;
    AFMsg::ReqCreateRole xData;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xData, nPlayerID))
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
            std::string strMsg;
            if(!xData.SerializeToString(&strMsg))
            {
                return;
            }

            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, nMsgID, strMsg, pSessionData->mnClientID);
        }
    }
}

void AFCProxyNetServerModule::OnReqDelRoleProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //在没有正式进入游戏之前，nPlayerID都是FD
    AFGUID nPlayerID;
    AFMsg::ReqDeleteRole xData;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xData, nPlayerID))
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
            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, nMsgID, std::string(msg, nLen), xClientID);
        }
    }
}

void AFCProxyNetServerModule::OnReqEnterGameServer(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //在没有正式进入游戏之前，nPlayerID都是FD
    AFGUID nPlayerID;
    AFMsg::ReqEnterGameServer xData;
    if(!m_pNetModule->ReceivePB(xHead, nMsgID, msg, nLen, xData, nPlayerID))
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
            std::string strMsg;
            if(!xData.SerializeToString(&strMsg))
            {
                return;
            }

            //playerid在进入游戏之前都是FD，其他时候是真实的ID
            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, AFMsg::EGameMsgID::EGMI_REQ_ENTER_GAME, strMsg, pSessionData->mnClientID);
        }
    }
}

int AFCProxyNetServerModule::EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID)
{
    ARK_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(pSessionData)
    {
        pSessionData->mnUserID = xPlayerID;
    }

    return 0;
}

