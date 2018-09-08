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

#include "AFCLoginToMasterModule.h"
#include "Common/AFProtoCPP.hpp"

bool AFCLoginToMasterModule::Init()
{
    m_pNetClientManagerModule = pPluginManager->FindModule<AFINetClientManagerModule>();
    m_pNetClientModule = m_pNetClientManagerModule->CreateClusterClientModule(ARK_APP_TYPE::ARK_APP_MASTER);
    ARK_ASSERT_RET_VAL(nullptr != m_pNetClientModule, false);

    return true;
}

bool AFCLoginToMasterModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pLoginLogicModule = pPluginManager->FindModule<AFILoginLogicModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
    m_pLoginNet_ServerModule = pPluginManager->FindModule<AFILoginNetServerModule>();

    m_pNetClientModule->AddRecvCallback(AFMsg::EGMI_ACK_CONNECT_WORLD, this, &AFCLoginToMasterModule::OnSelectServerResultProcess);
    m_pNetClientModule->AddRecvCallback(AFMsg::EGMI_STS_NET_INFO, this, &AFCLoginToMasterModule::OnWorldInfoProcess);

    m_pNetClientModule->AddEventCallBack(this, &AFCLoginToMasterModule::OnSocketMSEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");

    if (nullptr != xLogicClass)
    {
        AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;

        for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pConfigModule->GetNodeInt(strConfigName, "Type");
            const int nServerID = m_pConfigModule->GetNodeInt(strConfigName, "ServerID");

            if (nServerType == ARK_APP_TYPE::ARK_APP_MASTER)
            {
                const int nPort = m_pConfigModule->GetNodeInt(strConfigName, "Port");
                const std::string strServerName(m_pConfigModule->GetNodeString(strConfigName, "Name"));
                const std::string strIP(m_pConfigModule->GetNodeString(strConfigName, "IP"));

                ConnectData xServerData;

                xServerData.nGameID = nServerID;
                xServerData.eServerType = nServerType;
                xServerData.strIP = strIP;
                xServerData.nPort = nPort;
                xServerData.strName = strServerName;

                m_pNetClientModule->AddServer(xServerData);
            }
        }
    }

    return true;
}

void AFCLoginToMasterModule::Register(const int nServerID)
{
    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");

    if (nullptr != xLogicClass)
    {
        AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;

        for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pConfigModule->GetNodeInt(strConfigName, "Type");
            const int nSelfServerID = m_pConfigModule->GetNodeInt(strConfigName, "ServerID");

            if (nServerType == ARK_APP_TYPE::ARK_APP_LOGIN && pPluginManager->BusID() == nSelfServerID)
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
                    m_pNetClientModule->SendToServerByPB(nTargetID, AFMsg::EGameMsgID::EGMI_LTM_LOGIN_REGISTERED, xMsg, 0);

                    ARK_LOG_INFO("Register, server_id = {} server_name = {}", pData->server_id(), pData->server_name());
                }
            }
        }
    }
}

void AFCLoginToMasterModule::OnSelectServerResultProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::AckConnectWorldResult);
    m_pLoginNet_ServerModule->OnSelectWorldResultsProcess(xMsg.world_id(), AFIMsgModule::PBToGUID(xMsg.sender()), xMsg.login_id(), xMsg.account(), xMsg.world_ip(), xMsg.world_port(), xMsg.world_key());
}

void AFCLoginToMasterModule::OnSocketMSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if (eEvent == DISCONNECTED)
    {
        ARK_LOG_INFO("Connection closed, id = {}", xClientID.ToString());
    }
    else  if (eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString());
        Register(nServerID);
    }
}

void AFCLoginToMasterModule::OnWorldInfoProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        ARK_SHARE_PTR<AFMsg::ServerInfoReport> pServerData = mWorldMap.GetElement(xData.server_id());

        if (nullptr == pServerData)
        {
            pServerData = std::make_shared<AFMsg::ServerInfoReport>();
            *pServerData = xData;

            mWorldMap.AddElement(xData.server_id(), pServerData);
        }

    }

    ARK_LOG_INFO("WorldInfo size = {}", xMsg.server_list_size());
}

AFINetClientModule* AFCLoginToMasterModule::GetClusterModule()
{
    return m_pNetClientModule;
}

AFMapEx<int, AFMsg::ServerInfoReport>& AFCLoginToMasterModule::GetWorldMap()
{
    return mWorldMap;
}
