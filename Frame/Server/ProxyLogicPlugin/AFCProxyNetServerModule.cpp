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
#include "SDK/Interface/AFIKernelModule.h"

bool AFCProxyNetServerModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pProxyToWorldModule = pPluginManager->FindModule<AFIProxyServerToWorldModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pProxyServerToGameModule = pPluginManager->FindModule<AFIProxyServerToGameModule>();
    m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
    m_pNetServerManagerModule = pPluginManager->FindModule<AFINetServerManagerModule>();

    int ret = StartServer();
    if (ret != 0)
    {
        exit(0);
        return false;
    }

    return true;
}

int AFCProxyNetServerModule::StartServer()
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

    m_pNetServer->AddRecvCallback(AFMsg::EGMI_REQ_CONNECT_KEY, this, &AFCProxyNetServerModule::OnConnectKeyProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_REQ_WORLD_LIST, this, &AFCProxyNetServerModule::OnReqServerListProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_REQ_SELECT_SERVER, this, &AFCProxyNetServerModule::OnSelectServerProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_REQ_ROLE_LIST, this, &AFCProxyNetServerModule::OnReqRoleListProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_REQ_CREATE_ROLE, this, &AFCProxyNetServerModule::OnReqCreateRoleProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_REQ_DELETE_ROLE, this, &AFCProxyNetServerModule::OnReqDelRoleProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_REQ_ENTER_GAME, this, &AFCProxyNetServerModule::OnReqEnterGameServer);
    m_pNetServer->AddRecvCallback(this, &AFCProxyNetServerModule::OnOtherMessage);

    m_pNetServer->AddEventCallBack(this, &AFCProxyNetServerModule::OnSocketClientEvent);

    return 0;
}

bool AFCProxyNetServerModule::Update()
{
    return true;
}

int AFCProxyNetServerModule::HB_OnConnectCheckTime(const AFGUID& self, const std::string& strHeartBeat, const float fTime, const int nCount, const AFIDataList& var)
{
    m_pKernelModule->DestroyEntity(self);

    return 0;
}

void AFCProxyNetServerModule::OnOtherMessage(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

    if (!pSessionData || pSessionData->mnLogicState <= 0 || pSessionData->mnGameID <= 0)
    {
        //state error
        return;
    }

    if (pSessionData->mnUserID != xHead.GetPlayerID())
    {
        //when after entergame xHead.GetPlayerID() is really palyerID
        ARK_LOG_ERROR("xHead.GetPlayerID() is not really palyerID, id = {}", xHead.GetPlayerID().ToString());
        return;
    }

    m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, nMsgID, msg, nLen, xHead.GetPlayerID());
}

void AFCProxyNetServerModule::OnConnectKeyProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqAccountLogin);
    bool bRet = m_pProxyToWorldModule->VerifyConnectData(xMsg.account(), xMsg.security_code());

    if (bRet)
    {
        //可以进入,设置标志，选单服,心跳延迟,进入gs创建角色和删除角色,这里只是转发
        ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

        if (pSessionData)
        {
            pSessionData->mnLogicState = 1;
            pSessionData->mstrAccout  = xMsg.account();

            AFMsg::AckEventResult xSendMsg;
            xSendMsg.set_event_code(AFMsg::EGEC_VERIFY_KEY_SUCCESS);
            *xSendMsg.mutable_event_client() = AFIMsgModule::GUIDToPB(pSessionData->mnClientID);//让前端记得自己的fd，后面有一些验证
            *xSendMsg.mutable_event_object() = AFIMsgModule::GUIDToPB(nPlayerID);

            m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_CONNECT_KEY, xSendMsg, xClientID, nPlayerID);
        }
    }
    else
    {
        m_pNetServer->GetNet()->CloseNetEntity(xClientID);
    }
}

void AFCProxyNetServerModule::OnSocketClientEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nSeverID)
{
    if (eEvent == DISCONNECTED)
    {
        ARK_LOG_INFO("Connection closed, id = {}", xClientID.ToString());
        OnClientDisconnect(xClientID);
    }
    else if (eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString());
        OnClientConnected(xClientID);
    }
}

void AFCProxyNetServerModule::OnClientDisconnect(const AFGUID& xClientID)
{
    ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

    if (pSessionData)
    {
        if (pSessionData->mnGameID > 0 && !pSessionData->mnUserID.IsNULL())
        {
            AFMsg::ReqLeaveGameServer xData;
            std::string strMsg;

            if (!xData.SerializeToString(&strMsg))
            {
                return;
            }

            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, AFMsg::EGameMsgID::EGMI_REQ_LEAVE_GAME, strMsg, pSessionData->mnUserID);
        }

        mmSessionData.RemoveElement(xClientID);
    }
}

void AFCProxyNetServerModule::OnSelectServerProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqSelectServer);
    ARK_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xMsg.world_id());

    if (pServerData && ConnectData::CONNECTED == pServerData->eState)
    {
        ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

        if (pSessionData)
        {
            //now this client bind a game server, after this time, all message will be sent to this game server who bind with client
            pSessionData->mnGameID = xMsg.world_id();

            AFMsg::AckEventResult xResultMsg;
            xResultMsg.set_event_code(AFMsg::EGameEventCode::EGEC_SELECTSERVER_SUCCESS);
            m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xResultMsg, xClientID, nPlayerID);
            return;
        }
    }

    AFMsg::AckEventResult xSendMsg;
    xSendMsg.set_event_code(AFMsg::EGameEventCode::EGEC_SELECTSERVER_FAIL);
    m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xMsg, xClientID, nPlayerID);
}

void AFCProxyNetServerModule::OnReqServerListProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqServerList);

    if (xMsg.type() != AFMsg::RSLT_GAMES_ERVER)
    {
        return;
    }

    ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

    if (pSessionData && pSessionData->mnLogicState > 0)
    {
        //ack all gameserver data
        AFMsg::AckServerList xData;
        xData.set_type(AFMsg::RSLT_GAMES_ERVER);

        AFMapEx<int, ConnectData>& xServerList = m_pProxyServerToGameModule->GetClusterModule()->GetServerList();
        auto pGameData = xServerList.First();

        while (pGameData != nullptr)
        {
            if (ConnectData::CONNECTED == pGameData->eState)
            {
                AFMsg::ServerInfo* pServerInfo = xData.add_info();

                pServerInfo->set_name(pGameData->strName);
                pServerInfo->set_status(AFMsg::EServerState::EST_NARMAL);
                pServerInfo->set_server_id(pGameData->nGameID);
                pServerInfo->set_wait_count(0);
            }

            pGameData = xServerList.Next();
        }

        m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_WORLD_LIST, xData, xClientID, nPlayerID);
    }
}

int AFCProxyNetServerModule::Transpond(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen)
{
    ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xHead.GetPlayerID());

    if (pSessionData)
    {
        m_pNetServer->GetNet()->SendMsgWithOutHead(nMsgID, msg, nLen, pSessionData->mnClientID, xHead.GetPlayerID());
    }

    return true;
}

int AFCProxyNetServerModule::SendToPlayerClient(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID&  nClientID, const AFGUID&  nPlayer)
{
    m_pNetServer->GetNet()->SendMsgWithOutHead(nMsgID, msg, nLen, nClientID, nPlayer);

    return true;
}

void AFCProxyNetServerModule::OnClientConnected(const AFGUID& xClientID)
{
    ARK_SHARE_PTR<AFSessionData> pSessionData = std::make_shared<AFSessionData>();

    pSessionData->mnClientID = xClientID;
    mmSessionData.AddElement(xClientID, pSessionData);
}

void AFCProxyNetServerModule::OnReqRoleListProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    CheckSessionTransMsg<AFMsg::ReqRoleList>(xHead, nMsgID, msg, nLen, xClientID);
}

void AFCProxyNetServerModule::OnReqCreateRoleProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //在没有正式进入游戏之前，nPlayerID都是FD
    CheckSessionTransMsg<AFMsg::ReqCreateRole>(xHead, nMsgID, msg, nLen, xClientID);
}

void AFCProxyNetServerModule::OnReqDelRoleProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    CheckSessionTransMsg<AFMsg::ReqDeleteRole>(xHead, nMsgID, msg, nLen, xClientID);
}

void AFCProxyNetServerModule::OnReqEnterGameServer(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    CheckSessionTransMsg<AFMsg::ReqEnterGameServer>(xHead, nMsgID, msg, nLen, xClientID);
}

int AFCProxyNetServerModule::EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID)
{
    ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

    if (pSessionData)
    {
        pSessionData->mnUserID = xPlayerID;
    }

    return 0;
}

bool AFCProxyNetServerModule::CheckSessionState(const int64_t nGameID, const AFGUID& xClientID, const std::string& strAccount)
{
    ARK_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(nGameID);

    if (pServerData && ConnectData::CONNECTED == pServerData->eState)
    {
        //数据匹配
        ARK_SHARE_PTR<AFSessionData> pSessionData = mmSessionData.GetElement(xClientID);

        if (pSessionData
                && pSessionData->mnLogicState > 0
                && pSessionData->mnGameID == nGameID
                && pSessionData->mstrAccout == strAccount)
        {
            return true;
        }
    }

    return false;
}