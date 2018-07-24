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

#include "AFCLoginNetServerModule.h"
#include "AFLoginNetServerPlugin.h"

const std::string PROPERTY_ACCOUNT = "Account";
const std::string PROPERTY_VERIFIED = "Verified";

bool AFCLoginNetServerModule::Init()
{
    m_pNetModule = ARK_NEW AFINetServerModule(pPluginManager);
    return true;
}

bool AFCLoginNetServerModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pLoginLogicModule = pPluginManager->FindModule<AFILoginLogicModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pLoginToMasterModule = pPluginManager->FindModule<AFILoginToMasterModule>();
    m_pUUIDModule = pPluginManager->FindModule<AFIGUIDModule>();


    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_STS_HEART_BEAT, this, &AFCLoginNetServerModule::OnHeartBeat);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_LOGIN, this, &AFCLoginNetServerModule::OnLoginProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_LOGOUT, this, &AFCLoginNetServerModule::OnLogOut);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_CONNECT_WORLD, this, &AFCLoginNetServerModule::OnSelectWorldProcess);
    m_pNetModule->AddReceiveCallBack(AFMsg::EGMI_REQ_WORLD_LIST, this, &AFCLoginNetServerModule::OnViewWorldProcess);
    m_pNetModule->AddReceiveCallBack(this, &AFCLoginNetServerModule::InvalidMessage);

    m_pNetModule->AddEventCallBack(this, &AFCLoginNetServerModule::OnSocketClientEvent);

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

        if (nServerType == ARK_PROCESS_TYPE::ARK_PROC_LOGIN && pPluginManager->AppID() == nServerID)
        {
            const int nPort = m_pElementModule->GetNodeInt(strConfigName, "Port");
            const int nMaxConnect = m_pElementModule->GetNodeInt(strConfigName, "MaxOnline");
            const int nCpus = m_pElementModule->GetNodeInt(strConfigName, "CpuCount");
            const std::string strIP(m_pElementModule->GetNodeString(strConfigName, "IP"));

            m_pUUIDModule->SetGUIDMask(nServerID);

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

int AFCLoginNetServerModule::OnSelectWorldResultsProcess(const int nWorldID, const AFGUID xSenderID, const int nLoginID, const std::string& strAccount, const std::string& strWorldIP, const int nWorldPort, const std::string& strWorldKey)
{
    ARK_SHARE_PTR<SessionData> pSessionData = mmClientSessionData.GetElement(xSenderID);

    if (pSessionData)
    {
        AFMsg::AckConnectWorldResult xMsg;
        xMsg.set_world_id(nWorldID);
        xMsg.mutable_sender()->CopyFrom(AFINetModule::GUIDToPB(xSenderID));
        xMsg.set_login_id(nLoginID);
        xMsg.set_account(strAccount);
        xMsg.set_world_ip(strWorldIP);
        xMsg.set_world_port(nWorldPort);
        xMsg.set_world_key(strWorldKey);

        m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_CONNECT_WORLD, xMsg, pSessionData->mnClientID, xSenderID);
    }

    return 0;
}

bool AFCLoginNetServerModule::Update()
{
    return m_pNetModule->Update();
}

void AFCLoginNetServerModule::OnClientConnected(const AFGUID& xClientID)
{
    ARK_SHARE_PTR<SessionData> pSessionData = std::make_shared<SessionData>();

    pSessionData->mnClientID = xClientID;
    mmClientSessionData.AddElement(xClientID, pSessionData);
}

void AFCLoginNetServerModule::OnClientDisconnect(const AFGUID& xClientID)
{
    mmClientSessionData.RemoveElement(xClientID);
}

void AFCLoginNetServerModule::OnLoginProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqAccountLogin);
    ARK_SHARE_PTR<SessionData> pSession = mmClientSessionData.GetElement(xClientID);

    if (pSession == nullptr)
    {
        return;
    }

    if (pSession->mnLogicState == 0)
    {
        int nState = m_pLoginLogicModule->OnLoginProcess(pSession->mnClientID, xMsg.account(), xMsg.password());

        if (0 != nState)
        {
            ARK_LOG_ERROR("Check password failed, id = {} account = {} password = {}", xClientID.ToString().c_str(), xMsg.account().c_str(), xMsg.password().c_str());
            AFMsg::AckEventResult xMsg;
            xMsg.set_event_code(AFMsg::EGEC_ACCOUNTPWD_INVALID);

            m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_LOGIN, xMsg, xClientID, nPlayerID);
            return;
        }

        pSession->mnLogicState = 1;
        pSession->mstrAccout = xMsg.account();

        AFMsg::AckEventResult xData;
        xData.set_event_code(AFMsg::EGEC_ACCOUNT_SUCCESS);
        xData.set_parame1(xMsg.account());
        xData.set_parame2(xMsg.password());
        xData.set_parame3(xMsg.security_code());

        m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_LOGIN, xData, xClientID, nPlayerID);
        ARK_LOG_INFO("In same scene and group but it not a clone scene, id = {} account = {}", xClientID.ToString().c_str(), xMsg.account().c_str());
    }
}

void AFCLoginNetServerModule::OnSelectWorldProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqConnectWorld);
    ARK_SHARE_PTR<SessionData> pSession = mmClientSessionData.GetElement(xClientID);

    if (!pSession)
    {
        return;
    }

    //没登录过
    if (pSession->mnLogicState <= 0)
    {
        return;
    }

    AFMsg::ReqConnectWorld xData;
    xData.set_world_id(xMsg.world_id());
    xData.set_login_id(pPluginManager->AppID());
    xData.mutable_sender()->CopyFrom(AFINetServerModule::GUIDToPB(pSession->mnClientID));
    xData.set_account(pSession->mstrAccout);

    m_pLoginToMasterModule->GetClusterModule()->SendSuitByPB(pSession->mstrAccout, AFMsg::EGameMsgID::EGMI_REQ_CONNECT_WORLD, xData, xHead.GetPlayerID());//here has a problem to be solve
}

void AFCLoginNetServerModule::OnSocketClientEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if (eEvent == DISCONNECTED)
    {
        ARK_LOG_INFO("Connection closed, id = {}", xClientID.ToString().c_str());
        OnClientDisconnect(xClientID);
    }
    else  if (eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString().c_str());
        OnClientConnected(xClientID);
    }
}

void AFCLoginNetServerModule::SynWorldToClient(const AFGUID& xClientID)
{
    AFMsg::AckServerList xData;
    xData.set_type(AFMsg::RSLT_WORLD_SERVER);

    AFMapEx<int, AFMsg::ServerInfoReport>& xWorldMap = m_pLoginToMasterModule->GetWorldMap();
    auto pWorldData = xWorldMap.First();

    while (pWorldData)
    {
        AFMsg::ServerInfo* pServerInfo = xData.add_info();

        pServerInfo->set_name(pWorldData->server_name());
        pServerInfo->set_status(pWorldData->server_state());
        pServerInfo->set_server_id(pWorldData->server_id());
        pServerInfo->set_wait_count(0);

        pWorldData = xWorldMap.Next();
    }


    m_pNetModule->SendMsgPB(AFMsg::EGameMsgID::EGMI_ACK_WORLD_LIST, xData, xClientID, AFGUID(0));
}

void AFCLoginNetServerModule::OnViewWorldProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqServerList);

    if (xMsg.type() == AFMsg::RSLT_WORLD_SERVER)
    {
        SynWorldToClient(xClientID);
    }
}

void AFCLoginNetServerModule::OnHeartBeat(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //do nothing
}

void AFCLoginNetServerModule::OnLogOut(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //do nothing
}

void AFCLoginNetServerModule::InvalidMessage(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_LOG_ERROR("Invalid msg id = {}", nMsgID);
}



