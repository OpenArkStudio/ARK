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
#include "AFMasterNetServerPlugin.h"

bool AFCMasterNetServerModule::Init()
{
    m_pNetModule = ARK_NEW AFINetServerModule(pPluginManager);
    return true;
}


void AFCMasterNetServerModule::OnWorldRegisteredProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<ServerData> pServerData =  mWorldMap.GetElement(xData.server_id());

        if (nullptr == pServerData)
        {
            pServerData = std::make_shared<ServerData>();
            mWorldMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->Init(xClientID, xData);

        ARK_LOG_INFO("WorldRegistered, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name());
    }

    SynWorldToLogin();
}

void AFCMasterNetServerModule::OnWorldUnRegisteredProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        mWorldMap.RemoveElement(xData.server_id());

        ARK_LOG_INFO("WorldUnRegistered, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name());
    }

    SynWorldToLogin();
}

void AFCMasterNetServerModule::OnRefreshWorldInfoProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<ServerData> pServerData =  mWorldMap.GetElement(xData.server_id());

        if (nullptr == pServerData)
        {
            pServerData = std::make_shared<ServerData>();
            mWorldMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->Init(xClientID, xData);

        ARK_LOG_INFO("RefreshWorldInfo, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name());
    }

    SynWorldToLogin();
}

void AFCMasterNetServerModule::OnLoginRegisteredProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<ServerData> pServerData =  mLoginMap.GetElement(xData.server_id());

        if (nullptr == pServerData)
        {
            pServerData = std::make_shared<ServerData>();
            mLoginMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->Init(xClientID, xData);

        ARK_LOG_INFO("LoginRegistered, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name());
    }

    SynWorldToLogin();
}

void AFCMasterNetServerModule::OnLoginUnRegisteredProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        mLoginMap.RemoveElement(xData.server_id());

        ARK_LOG_INFO("LoginUnRegistered, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name());
    }
}

void AFCMasterNetServerModule::OnRefreshLoginInfoProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);
        ARK_SHARE_PTR<ServerData> pServerData =  mLoginMap.GetElement(xData.server_id());

        if (nullptr == pServerData)
        {
            pServerData = std::make_shared<ServerData>();
            mLoginMap.AddElement(xData.server_id(), pServerData);
        }

        pServerData->Init(xClientID, xData);

        ARK_LOG_INFO("RefreshLoginInfo, server_id[{}] server_name[{}]", xData.server_id(), xData.server_name());
    }
}

void AFCMasterNetServerModule::OnSelectWorldProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqConnectWorld);

    ARK_SHARE_PTR<ServerData> pServerData =  mWorldMap.GetElement(xMsg.world_id());

    if (nullptr == pServerData)
    {
        return;
    }

    //send to world
    m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_REQ_CONNECT_WORLD, xMsg, pServerData->xClient, nPlayerID);
}

bool AFCMasterNetServerModule::Update()
{
    LogGameServer();
    return m_pNetModule->Update();
}

void AFCMasterNetServerModule::OnSelectServerResultProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::AckConnectWorldResult);
    ARK_SHARE_PTR<ServerData> pServerData =  mLoginMap.GetElement(xMsg.login_id());

    if (nullptr == pServerData)
    {
        return;
    }

    //转发送到登录服务器
    m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_CONNECT_WORLD, xMsg, pServerData->xClient, nPlayerID);
}

bool AFCMasterNetServerModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();

    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_STS_HEART_BEAT, this, &AFCMasterNetServerModule::OnHeartBeat);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_MTL_WORLD_REGISTERED, this, &AFCMasterNetServerModule::OnWorldRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_MTL_WORLD_UNREGISTERED, this, &AFCMasterNetServerModule::OnWorldUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_MTL_WORLD_REFRESH, this, &AFCMasterNetServerModule::OnRefreshWorldInfoProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_LTM_LOGIN_REGISTERED, this, &AFCMasterNetServerModule::OnLoginRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_LTM_LOGIN_UNREGISTERED, this, &AFCMasterNetServerModule::OnLoginUnRegisteredProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_LTM_LOGIN_REFRESH, this, &AFCMasterNetServerModule::OnRefreshLoginInfoProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_CONNECT_WORLD, this, &AFCMasterNetServerModule::OnSelectWorldProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_ACK_CONNECT_WORLD, this, &AFCMasterNetServerModule::OnSelectServerResultProcess);
    m_pNetModule->AddReceiveCallBack(this, &AFCMasterNetServerModule::InvalidMessage);

    m_pNetModule->AddEventCallBack(this, &AFCMasterNetServerModule::OnSocketEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");

    if (nullptr == xLogicClass)
    {
        return false;
    }

    AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
    std::string strConfigName;

    for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
    {
        const int nServerType = m_pElementModule->GetNodeInt(strConfigName, "Type");
        const int nServerID = m_pElementModule->GetNodeInt(strConfigName, "ServerID");

        if (nServerType == ARK_PROCESS_TYPE::ARK_PROC_MASTER && pPluginManager->BusID() == nServerID)
        {
            const int nPort = m_pElementModule->GetNodeInt(strConfigName, "Port");
            const int nMaxConnect = m_pElementModule->GetNodeInt(strConfigName, "MaxOnline");
            const int nCpus = m_pElementModule->GetNodeInt(strConfigName, "CpuCount");
            const std::string strServerName(m_pElementModule->GetNodeString(strConfigName, "Name"));
            const std::string strIP(m_pElementModule->GetNodeString(strConfigName, "IP"));

            int nRet = m_pNetModule->Start(nMaxConnect, strIP, nPort, nCpus, nServerID);

            if (nRet < 0)
            {
                ARK_LOG_ERROR("Cannot init server net, Port = {}", nPort);
                ARK_ASSERT(nRet, "Cannot init server net", __FILE__, __FUNCTION__);
                exit(0);
            }
        }
    }

    return true;
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
    ARK_SHARE_PTR<ServerData> pServerData =  mWorldMap.First();

    while (nullptr != pServerData)
    {
        if (xClientID == pServerData->xClient)
        {
            pServerData->pData->set_server_state(AFMsg::EST_CRASH);
            pServerData->xClient = AFGUID(0);

            SynWorldToLogin();
            return;
        }

        pServerData = mWorldMap.Next();
    }

    //////////////////////////////////////////////////////////////////////////

    int nServerID = 0;
    pServerData =  mLoginMap.First();

    while (nullptr != pServerData)
    {
        if (xClientID == pServerData->xClient)
        {
            nServerID = pServerData->pData->server_id();
            break;
        }

        pServerData = mLoginMap.Next();
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

    ARK_SHARE_PTR<ServerData> pServerData =  mWorldMap.First();

    while (nullptr != pServerData)
    {
        AFMsg::ServerInfoReport* pData = xData.add_server_list();
        *pData = *(pServerData->pData);

        pServerData = mWorldMap.Next();
    }

    //广播给所有loginserver
    pServerData =  mLoginMap.First();

    while (nullptr != pServerData)
    {
        m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_STS_NET_INFO, xData, pServerData->xClient, AFGUID(0));
        pServerData = mLoginMap.Next();
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

    for (ARK_SHARE_PTR<ServerData> pGameData = mWorldMap.First(); pGameData != nullptr; pGameData = mWorldMap.Next())
    {
        ARK_LOG_INFO("Type[{}] ID[{}] State[{}] IP[{}] xClient[{}]",
                     pGameData->pData->server_type(),
                     pGameData->pData->server_id(),
                     AFMsg::EServerState_Name(pGameData->pData->server_state()),
                     pGameData->pData->server_ip(),
                     pGameData->xClient.nLow);
    }

    ARK_LOG_INFO("End Log WorldServer Info---------------------------");
    //////////////////////////////////////////////////////////////////////////
    ARK_LOG_INFO("Begin Log LoginServer Info---------------------------");

    for (ARK_SHARE_PTR<ServerData> pGameData = mLoginMap.First(); pGameData != nullptr; pGameData = mLoginMap.Next())
    {
        ARK_LOG_INFO("Type[{}] ID[{}] State[{}] IP[{}] xClient[{}]",
                     pGameData->pData->server_type(),
                     pGameData->pData->server_id(),
                     AFMsg::EServerState_Name(pGameData->pData->server_state()).c_str(),
                     pGameData->pData->server_ip().c_str(),
                     pGameData->xClient.nLow);
    }

    ARK_LOG_INFO("End Log LoginServer Info---------------------------");
}

void AFCMasterNetServerModule::OnHeartBeat(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //do nothing
}

void AFCMasterNetServerModule::InvalidMessage(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_LOG_ERROR("Invalid msg id = {}", nMsgID);
}