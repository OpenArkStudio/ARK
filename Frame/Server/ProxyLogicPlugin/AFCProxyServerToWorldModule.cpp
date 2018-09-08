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

#include "AFCProxyServerToWorldModule.h"
#include "SDK/Core/AFIHeartBeatManager.h"
#include "SDK/Core/AFCHeartBeatManager.h"
#include "SDK/Interface/AFIClassModule.h"

bool AFCProxyServerToWorldModule::Init()
{
    m_pNetClientManagerModule = pPluginManager->FindModule<AFINetClientManagerModule>();
    m_pNetClientModule = m_pNetClientManagerModule->CreateClusterClientModule(ARK_APP_TYPE::ARK_APP_WORLD);
    ARK_ASSERT_RET_VAL(nullptr != m_pNetClientModule, false);

    return true;
}

void AFCProxyServerToWorldModule::OnServerInfoProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        ConnectData xServerData;
        xServerData.nGameID = xData.server_id();
        xServerData.strIP = xData.server_ip();
        xServerData.nPort = xData.server_port();
        xServerData.strName = xData.server_name();
        xServerData.eServerType = (ARK_APP_TYPE)xData.server_type();

        switch (xServerData.eServerType)
        {
        case ARK_APP_TYPE::ARK_APP_GAME:
            m_pProxyServerToGameModule->GetClusterModule()->AddServer(xServerData);
            break;

        case ARK_APP_TYPE::ARK_APP_WORLD:
            m_pNetClientModule->AddServer(xServerData);
            break;

        default:
            break;
        }
    }
}

void AFCProxyServerToWorldModule::OnSocketWSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
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

void AFCProxyServerToWorldModule::Register(const int nServerID)
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

            if (nServerType == ARK_APP_TYPE::ARK_APP_PROXY && pPluginManager->BusID() == nSelfServerID)
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

                ARK_SHARE_PTR<ConnectData> pServerData = GetClusterModule()->GetServerNetInfo(nServerID);

                if (pServerData)
                {
                    int nTargetID = pServerData->nGameID;
                    GetClusterModule()->SendToServerByPB(nTargetID, AFMsg::EGameMsgID::EGMI_PTWG_PROXY_REGISTERED, xMsg, 0);

                    ARK_LOG_INFO("Register, server_id  = {} server_name = {}", pData->server_id(), pData->server_name());
                }
            }
        }
    }
}

bool AFCProxyServerToWorldModule::PostInit()
{
    m_pProxyLogicModule = pPluginManager->FindModule<AFIProxyLogicModule>();
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pProxyServerNet_ServerModule = pPluginManager->FindModule<AFIProxyNetServerModule>();
    m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pProxyServerToGameModule = pPluginManager->FindModule<AFIProxyServerToGameModule>();

    m_pNetClientModule->AddRecvCallback(AFMsg::EGMI_ACK_CONNECT_WORLD, this, &AFCProxyServerToWorldModule::OnSelectServerResultProcess);
    m_pNetClientModule->AddRecvCallback(AFMsg::EGMI_STS_NET_INFO, this, &AFCProxyServerToWorldModule::OnServerInfoProcess);
    m_pNetClientModule->AddRecvCallback(AFMsg::EGMI_GTG_BROCASTMSG, this, &AFCProxyServerToWorldModule::OnBrocastmsg);
    m_pNetClientModule->AddRecvCallback(this, &AFCProxyServerToWorldModule::OnOtherMessage);

    m_pNetClientModule->AddEventCallBack(this, &AFCProxyServerToWorldModule::OnSocketWSEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");

    if (nullptr != xLogicClass)
    {
        AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;

        for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pConfigModule->GetNodeInt(strConfigName, "Type");
            const int nServerID = m_pConfigModule->GetNodeInt(strConfigName, "ServerID");

            if (nServerType == ARK_APP_TYPE::ARK_APP_WORLD)
            {
                const int nPort = m_pConfigModule->GetNodeInt(strConfigName, "Port");
                const std::string strServerName(m_pConfigModule->GetNodeString(strConfigName, "Name"));
                const std::string strIP(m_pConfigModule->GetNodeString(strConfigName, "IP"));

                ConnectData xServerData;
                xServerData.nGameID = nServerID;
                xServerData.eServerType = (ARK_APP_TYPE)nServerType;
                xServerData.strIP = strIP;
                xServerData.nPort = nPort;
                xServerData.strName = strServerName;

                m_pNetClientModule->AddServer(xServerData);
            }
        }
    }

    return true;
}


void AFCProxyServerToWorldModule::OnSelectServerResultProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::AckConnectWorldResult);
    ARK_SHARE_PTR<ClientConnectData> pConnectData = mxWantToConnectMap.GetElement(xMsg.account());

    if (pConnectData != nullptr)
    {
        pConnectData->strConnectKey = xMsg.world_key();
        return;
    }

    pConnectData = std::make_shared<ClientConnectData>();
    pConnectData->strAccount = xMsg.account();
    pConnectData->strConnectKey = xMsg.world_key();
    mxWantToConnectMap.AddElement(pConnectData->strAccount, pConnectData);
}

AFINetClientModule* AFCProxyServerToWorldModule::GetClusterModule()
{
    return m_pNetClientModule;
}

bool AFCProxyServerToWorldModule::VerifyConnectData(const std::string& strAccount, const std::string& strKey)
{
    ARK_SHARE_PTR<ClientConnectData> pConnectData = mxWantToConnectMap.GetElement(strAccount);

    if (pConnectData != nullptr && strKey == pConnectData->strConnectKey)
    {
        mxWantToConnectMap.RemoveElement(strAccount);
        return true;
    }

    return false;
}

void AFCProxyServerToWorldModule::LogServerInfo(const std::string& strServerInfo)
{
    ARK_LOG_INFO("{}", strServerInfo);
}

void AFCProxyServerToWorldModule::OnOtherMessage(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    m_pProxyServerNet_ServerModule->Transpond(xHead, nMsgID, msg, nLen);
}

void AFCProxyServerToWorldModule::OnBrocastmsg(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::BrocastMsg);

    for (int i = 0; i < xMsg.target_entity_list_size(); i++)
    {
        const AFMsg::PBGUID& xPlayerClientID = xMsg.target_entity_list(i);
        m_pProxyServerNet_ServerModule->SendToPlayerClient(xMsg.msg_id(), xMsg.msg_data().c_str(), xMsg.msg_data().size(), AFIMsgModule::PBToGUID(xPlayerClientID), nPlayerID);
    }
}