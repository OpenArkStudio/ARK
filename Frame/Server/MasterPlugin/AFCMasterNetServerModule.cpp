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

#include "AFCMasterNetServerModule.h"

bool AFCMasterNetServerModule::Init()
{
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
    m_pNetServerManagerModule = pPluginManager->FindModule<AFINetServerManagerModule>();

    return true;
}

bool AFCMasterNetServerModule::PostInit()
{
    int ret = StartServer();
    if (ret != 0)
    {
        exit(0);
        return false;
    }

    return true;
}

int AFCMasterNetServerModule::StartServer()
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

    m_pNetServer->AddRecvCallback(AFMsg::EGMI_STS_HEART_BEAT, this, &AFCMasterNetServerModule::OnHeartBeat);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_MTL_WORLD_REGISTERED, this, &AFCMasterNetServerModule::OnWorldRegisteredProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_MTL_WORLD_UNREGISTERED, this, &AFCMasterNetServerModule::OnWorldUnRegisteredProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_MTL_WORLD_REFRESH, this, &AFCMasterNetServerModule::OnRefreshWorldInfoProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_LTM_LOGIN_REGISTERED, this, &AFCMasterNetServerModule::OnLoginRegisteredProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_LTM_LOGIN_UNREGISTERED, this, &AFCMasterNetServerModule::OnLoginUnRegisteredProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_LTM_LOGIN_REFRESH, this, &AFCMasterNetServerModule::OnRefreshLoginInfoProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_REQ_CONNECT_WORLD, this, &AFCMasterNetServerModule::OnSelectWorldProcess);
    m_pNetServer->AddRecvCallback(AFMsg::EGMI_ACK_CONNECT_WORLD, this, &AFCMasterNetServerModule::OnSelectServerResultProcess);
    m_pNetServer->AddRecvCallback(this, &AFCMasterNetServerModule::InvalidMessage);

    m_pNetServer->AddEventCallBack(this, &AFCMasterNetServerModule::OnSocketEvent);

    return 0;
}

bool AFCMasterNetServerModule::Update()
{
    LogGameServer();

    return true;
}

void AFCMasterNetServerModule::OnWorldRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<AFServerData> pServerData =  mWorldMap.GetElement(xData.bus_id());

        if (nullptr == pServerData)
        {
            pServerData = std::make_shared<AFServerData>();
            mWorldMap.AddElement(xData.bus_id(), pServerData);
        }

        pServerData->Init(xClientID, xData);

        ARK_LOG_INFO("WorldRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
    }

    SynWorldToLogin();
}

void AFCMasterNetServerModule::OnWorldUnRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        mWorldMap.RemoveElement(xData.bus_id());

        ARK_LOG_INFO("WorldUnRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
    }

    SynWorldToLogin();
}

void AFCMasterNetServerModule::OnRefreshWorldInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<AFServerData> pServerData =  mWorldMap.GetElement(xData.bus_id());

        if (nullptr == pServerData)
        {
            pServerData = std::make_shared<AFServerData>();
            mWorldMap.AddElement(xData.bus_id(), pServerData);
        }

        pServerData->Init(xClientID, xData);

        ARK_LOG_INFO("RefreshWorldInfo, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
    }

    SynWorldToLogin();
}

void AFCMasterNetServerModule::OnLoginRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<AFServerData> pServerData =  mLoginMap.GetElement(xData.bus_id());

        if (nullptr == pServerData)
        {
            pServerData = std::make_shared<AFServerData>();
            mLoginMap.AddElement(xData.bus_id(), pServerData);
        }

        pServerData->Init(xClientID, xData);

        ARK_LOG_INFO("LoginRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
    }

    SynWorldToLogin();
}

void AFCMasterNetServerModule::OnLoginUnRegisteredProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        mLoginMap.RemoveElement(xData.bus_id());

        ARK_LOG_INFO("LoginUnRegistered, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
    }
}

void AFCMasterNetServerModule::OnRefreshLoginInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<AFServerData> pServerData =  mLoginMap.GetElement(xData.bus_id());

        if (nullptr == pServerData)
        {
            pServerData = std::make_shared<AFServerData>();
            mLoginMap.AddElement(xData.bus_id(), pServerData);
        }

        pServerData->Init(xClientID, xData);

        ARK_LOG_INFO("RefreshLoginInfo, server_id[{}] server_url[{}]", xData.bus_id(), xData.url());
    }
}

void AFCMasterNetServerModule::OnSelectWorldProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqConnectWorld);

    ARK_SHARE_PTR<AFServerData> pServerData =  mWorldMap.GetElement(xMsg.world_id());

    if (nullptr == pServerData)
    {
        return;
    }

    //send to world
    m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_REQ_CONNECT_WORLD, xMsg, pServerData->xClient, nPlayerID);
}

void AFCMasterNetServerModule::OnSelectServerResultProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::AckConnectWorldResult);
    ARK_SHARE_PTR<AFServerData> pServerData =  mLoginMap.GetElement(xMsg.login_id());

    if (nullptr == pServerData)
    {
        return;
    }

    //转发送到登录服务器
    m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_ACK_CONNECT_WORLD, xMsg, pServerData->xClient, nPlayerID);
}

void AFCMasterNetServerModule::OnSocketEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
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

void AFCMasterNetServerModule::OnClientDisconnect(const AFGUID& xClientID)
{
    //不管是login还是world都要找出来,替他反注册
    for (ARK_SHARE_PTR<AFServerData> pServerData = mWorldMap.First(); nullptr != pServerData; pServerData = mWorldMap.Next())
    {
        if (xClientID == pServerData->xClient)
        {
            pServerData->xData.set_logic_status(AFMsg::EST_CRASH);
            pServerData->xClient = AFGUID(0);

            SynWorldToLogin();
            return;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    int nServerID = 0;
    for (ARK_SHARE_PTR<AFServerData> pServerData = mLoginMap.First(); nullptr != pServerData; pServerData = mLoginMap.Next())
    {
        if (xClientID == pServerData->xClient)
        {
            nServerID = pServerData->xData.bus_id();
            break;
        }
    }

    mLoginMap.RemoveElement(nServerID);
}

void AFCMasterNetServerModule::OnClientConnected(const AFGUID& xClientID)
{
    //连接上来啥都不做
}

void AFCMasterNetServerModule::SynWorldToLogin()
{
    AFMsg::ServerInfoReportList xData;

    for (ARK_SHARE_PTR<AFServerData> pServerData = mWorldMap.First(); nullptr != pServerData; pServerData = mWorldMap.Next())
    {
        AFMsg::ServerInfoReport* pData = xData.add_server_list();
        *pData = pServerData->xData;
    }

    //广播给所有loginserver
    for (ARK_SHARE_PTR<AFServerData> pServerData = mLoginMap.First(); nullptr != pServerData; pServerData = mLoginMap.Next())
    {
        m_pNetServer->SendPBMsg(AFMsg::EGameMsgID::EGMI_STS_NET_INFO, xData, pServerData->xClient, AFGUID(0));
    }
}

void AFCMasterNetServerModule::LogGameServer()
{
    if (mnLastLogTime + 10 * 1000 > GetPluginManager()->GetNowTime())
    {
        return;
    }

    mnLastLogTime = GetPluginManager()->GetNowTime();

    //////////////////////////////////////////////////////////////////////////
    ARK_LOG_INFO("Begin Log WorldServer Info---------------------------");
    for (ARK_SHARE_PTR<AFServerData> pGameData = mWorldMap.First(); pGameData != nullptr; pGameData = mWorldMap.Next())
    {
        ARK_LOG_INFO("ID[{}] State[{}] IP[{}] xClient[{}]",
                     pGameData->xData.bus_id(),
                     AFMsg::EServerState_Name(pGameData->xData.logic_status()),
                     pGameData->xData.url(),
                     pGameData->xClient.nLow);
    }
    ARK_LOG_INFO("End Log WorldServer Info---------------------------");
    //////////////////////////////////////////////////////////////////////////
    ARK_LOG_INFO("Begin Log LoginServer Info---------------------------");
    for (ARK_SHARE_PTR<AFServerData> pLoginData = mLoginMap.First(); pLoginData != nullptr; pLoginData = mLoginMap.Next())
    {
        ARK_LOG_INFO("ID[{}] State[{}] IP[{}] xClient[{}]",
                     pLoginData->xData.bus_id(),
                     AFMsg::EServerState_Name(pLoginData->xData.logic_status()),
                     pLoginData->xData.url(),
                     pLoginData->xClient.nLow);
    }
    ARK_LOG_INFO("End Log LoginServer Info---------------------------");
}

void AFCMasterNetServerModule::OnHeartBeat(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //do nothing
}

void AFCMasterNetServerModule::InvalidMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_LOG_ERROR("Invalid msg id = {}", nMsgID);
}