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

#include "AFCProxyServerToGameModule.h"
#include "AFProxyNetClientPlugin.h"
#include "SDK/Core/AFIHeartBeatManager.h"
#include "SDK/Core/AFCHeartBeatManager.h"
#include "SDK/Interface/AFIClassModule.h"

bool AFCProxyServerToGameModule::Init()
{
    m_pNetClientModule = ARK_NEW AFINetClientModule(pPluginManager);
    m_pNetClientModule->Init();

    return true;
}

bool AFCProxyServerToGameModule::Shut()
{
    m_pNetClientModule->Shut();
    ARK_DELETE(m_pNetClientModule);

    return true;
}

bool AFCProxyServerToGameModule::Update()
{
    return m_pNetClientModule->Update();
}

bool AFCProxyServerToGameModule::PostInit()
{
    m_pProxyLogicModule = pPluginManager->FindModule<AFIProxyLogicModule>();
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pProxyServerNet_ServerModule = pPluginManager->FindModule<AFIProxyNetServerModule>();
    m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();

    m_pNetClientModule->AddReceiveCallBack(AFMsg::EGMI_ACK_ENTER_GAME, this, &AFCProxyServerToGameModule::OnAckEnterGame);
    m_pNetClientModule->AddReceiveCallBack(AFMsg::EGMI_GTG_BROCASTMSG, this, &AFCProxyServerToGameModule::OnBrocastmsg);
    m_pNetClientModule->AddReceiveCallBack(this, &AFCProxyServerToGameModule::Transpond);

    m_pNetClientModule->AddEventCallBack(this, &AFCProxyServerToGameModule::OnSocketGSEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");

    if (nullptr == xLogicClass)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
    std::string strConfigName;

    for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
    {
        const int nServerType = m_pConfigModule->GetNodeInt(strConfigName, "Type");
        const int nServerID = m_pConfigModule->GetNodeInt(strConfigName, "ServerID");

        if (nServerType == ARK_PROCESS_TYPE::ARK_PROC_GAME)
        {
            const int nPort = m_pConfigModule->GetNodeInt(strConfigName, "Port");
            const std::string strServerName(m_pConfigModule->GetNodeString(strConfigName, "Name"));
            const std::string strIP(m_pConfigModule->GetNodeString(strConfigName, "IP"));

            ConnectData xServerData;

            xServerData.nGameID = nServerID;
            xServerData.eServerType = (ARK_PROCESS_TYPE)nServerType;
            xServerData.strIP = strIP;
            xServerData.nPort = nPort;
            xServerData.strName = strServerName;

            m_pNetClientModule->AddServer(xServerData);
        }
    }

    return true;
}

AFINetClientModule* AFCProxyServerToGameModule::GetClusterModule()
{
    return m_pNetClientModule;
}

void AFCProxyServerToGameModule::OnSocketGSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if (eEvent == DISCONNECTED)
    {
        ARK_LOG_INFO("Connection closed, id = {}", xClientID.ToString());
    }
    else if (eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString());
        Register(nServerID);
    }
}

void AFCProxyServerToGameModule::Register(const int nServerID)
{
    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");

    if (nullptr == xLogicClass)
    {
        ARK_ASSERT_NO_EFFECT(0);
    }

    AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
    std::string strConfigName;

    for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
    {
        const int nServerType = m_pConfigModule->GetNodeInt(strConfigName, "Type");
        const int nSelfServerID = m_pConfigModule->GetNodeInt(strConfigName, "ServerID");

        if (nServerType == ARK_PROCESS_TYPE::ARK_PROC_PROXY && pPluginManager->BusID() == nSelfServerID)
        {
            const int nPort = m_pConfigModule->GetNodeInt(strConfigName, "Port");
            const int nMaxConnect = m_pConfigModule->GetNodeInt(strConfigName, "MaxOnline");
            const std::string strServerName(m_pConfigModule->GetNodeString(strConfigName, "Name"));
            const std::string strIP(m_pConfigModule->GetNodeString(strConfigName, "IP"));

            AFMsg::ServerInfoReportList xMsg;
            AFMsg::ServerInfoReport* pData = xMsg.add_server_list();

            pData->set_server_id(nSelfServerID);
            pData->set_server_name(strServerName);
            pData->set_server_cur_count(0);
            pData->set_server_ip(strIP);
            pData->set_server_port(nPort);
            pData->set_server_max_online(nMaxConnect);
            pData->set_server_state(AFMsg::EST_NARMAL);
            pData->set_server_type(nServerType);

            ARK_SHARE_PTR<ConnectData> pServerData = m_pNetClientModule->GetServerNetInfo(nServerID);

            if (pServerData)
            {
                int nTargetID = pServerData->nGameID;
                m_pNetClientModule->SendToServerByPB(nTargetID, AFMsg::EGameMsgID::EGMI_PTWG_PROXY_REGISTERED, xMsg, 0);

                ARK_LOG_INFO("Register, server_id  = {} server_name = {}", pData->server_id(), pData->server_name());
            }
        }
    }
}

void AFCProxyServerToGameModule::OnAckEnterGame(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::AckEventResult);

    if (xMsg.event_code() == AFMsg::EGEC_ENTER_GAME_SUCCESS)
    {
        const AFGUID& xClient = AFINetModule::PBToGUID(xMsg.event_client());
        const AFGUID& xPlayer = AFINetModule::PBToGUID(xMsg.event_object());

        m_pProxyServerNet_ServerModule->EnterGameSuccessEvent(xClient, xPlayer);
    }
}

void AFCProxyServerToGameModule::OnBrocastmsg(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::BrocastMsg);

    for (int i = 0; i < xMsg.target_entity_list_size(); i++)
    {
        const AFMsg::PBGUID& tmpID = xMsg.target_entity_list(i);
        m_pProxyServerNet_ServerModule->SendToPlayerClient(xMsg.msg_id(), xMsg.msg_data().c_str(), xMsg.msg_data().size(), AFINetModule::PBToGUID(tmpID), nPlayerID);
    }
}

void AFCProxyServerToGameModule::LogServerInfo(const std::string& strServerInfo)
{
    ARK_LOG_INFO("{}", strServerInfo.c_str());
}

void AFCProxyServerToGameModule::Transpond(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    m_pProxyServerNet_ServerModule->Transpond(xHead, nMsgID, msg, nLen);
}