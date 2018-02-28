/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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
#include "AFProxyNetClientPlugin.h"
#include "SDK/Core/AFIHeartBeatManager.h"
#include "SDK/Core/AFCHeartBeatManager.h"
#include "SDK/Interface/AFIClassModule.h"

bool AFCProxyServerToWorldModule::Init()
{
    m_pNetClientModule = ARK_NEW AFINetClientModule(pPluginManager);

    m_pNetClientModule->Init();

    return true;
}

bool AFCProxyServerToWorldModule::Shut()
{
    //Final();
    //Clear();
    return true;
}

void AFCProxyServerToWorldModule::Update()
{
    m_pNetClientModule->Update();
}

void AFCProxyServerToWorldModule::OnServerInfoProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ServerInfoReportList xMsg;
    if(!AFINetServerModule::ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        //type
        ConnectData xServerData;

        xServerData.nGameID = xData.server_id();
        xServerData.strIP = xData.server_ip();
        xServerData.nPort = xData.server_port();
        xServerData.strName = xData.server_name();
        //xServerData.eState = pData->server_state();
        xServerData.eServerType = (ARK_SERVER_TYPE)xData.server_type();

        switch(xServerData.eServerType)
        {
        case ARK_SERVER_TYPE::ARK_ST_GAME:
            {
                m_pProxyServerToGameModule->GetClusterModule()->AddServer(xServerData);
            }
            break;
        case ARK_SERVER_TYPE::ARK_ST_WORLD:
            {
                m_pNetClientModule->AddServer(xServerData);
            }
            break;
        default:
            break;
        }

    }
}

void AFCProxyServerToWorldModule::OnSocketWSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if(eEvent == DISCONNECTED)
    {
        m_pLogModule->LogInfo(xClientID, "NF_NET_EVENT_EOF", "Connection closed", __FUNCTION__, __LINE__);
    }
    else  if(eEvent == CONNECTED)
    {
        m_pLogModule->LogInfo(xClientID, "NF_NET_EVENT_CONNECTED", "connectioned success", __FUNCTION__, __LINE__);
        Register(nServerID);
    }
}

void AFCProxyServerToWorldModule::Register(const int nServerID)
{
    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetNodeInt(strConfigName, "Type");
            const int nSelfServerID = m_pElementModule->GetNodeInt(strConfigName, "ServerID");
            if(nServerType == ARK_SERVER_TYPE::ARK_ST_PROXY && pPluginManager->AppID() == nSelfServerID)
            {
                const int nPort = m_pElementModule->GetNodeInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetNodeInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetNodeInt(strConfigName, "CpuCount");
                const std::string strName(m_pElementModule->GetNodeString(strConfigName, "Name"));
                const std::string strIP(m_pElementModule->GetNodeString(strConfigName, "IP"));

                AFMsg::ServerInfoReportList xMsg;
                AFMsg::ServerInfoReport* pData = xMsg.add_server_list();

                pData->set_server_id(nSelfServerID);
                pData->set_server_name(strName);
                pData->set_server_cur_count(0);
                pData->set_server_ip(strIP);
                pData->set_server_port(nPort);
                pData->set_server_max_online(nMaxConnect);
                pData->set_server_state(AFMsg::EST_NARMAL);
                pData->set_server_type(nServerType);

                ARK_SHARE_PTR<ConnectData> pServerData = GetClusterModule()->GetServerNetInfo(nServerID);
                if(pServerData)
                {
                    int nTargetID = pServerData->nGameID;
                    GetClusterModule()->SendToServerByPB(nTargetID, AFMsg::EGameMsgID::EGMI_PTWG_PROXY_REGISTERED, xMsg, 0);

                    m_pLogModule->LogInfo(AFGUID(0, pData->server_id()), pData->server_name(), "Register");
                }
            }
        }
    }
}

bool AFCProxyServerToWorldModule::AfterInit()
{
    m_pProxyLogicModule = pPluginManager->FindModule<AFIProxyLogicModule>();
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pProxyServerNet_ServerModule = pPluginManager->FindModule<AFIProxyNetServerModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pProxyServerToGameModule = pPluginManager->FindModule<AFIProxyServerToGameModule>();

    m_pNetClientModule->AddReceiveCallBack(AFMsg::EGMI_ACK_CONNECT_WORLD, this, &AFCProxyServerToWorldModule::OnSelectServerResultProcess);
    m_pNetClientModule->AddReceiveCallBack(AFMsg::EGMI_STS_NET_INFO, this, &AFCProxyServerToWorldModule::OnServerInfoProcess);
    m_pNetClientModule->AddReceiveCallBack(AFMsg::EGMI_GTG_BROCASTMSG, this, &AFCProxyServerToWorldModule::OnBrocastmsg);
    m_pNetClientModule->AddReceiveCallBack(this, &AFCProxyServerToWorldModule::OnOtherMessage);

    m_pNetClientModule->AddEventCallBack(this, &AFCProxyServerToWorldModule::OnSocketWSEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetNodeInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetNodeInt(strConfigName, "ServerID");
            if(nServerType == ARK_SERVER_TYPE::ARK_ST_WORLD)
            {
                const int nPort = m_pElementModule->GetNodeInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetNodeInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetNodeInt(strConfigName, "CpuCount");
                const std::string strName(m_pElementModule->GetNodeString(strConfigName, "Name"));
                const std::string strIP(m_pElementModule->GetNodeString(strConfigName, "IP"));

                ConnectData xServerData;

                xServerData.nGameID = nServerID;
                xServerData.eServerType = (ARK_SERVER_TYPE)nServerType;
                xServerData.strIP = strIP;
                xServerData.nPort = nPort;
                xServerData.strName = strName;

                m_pNetClientModule->AddServer(xServerData);
            }
        }
    }

    return true;
}


void AFCProxyServerToWorldModule::OnSelectServerResultProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //保持记录,直到下线,或者1分钟不上线即可删除
    AFGUID nPlayerID;
    AFMsg::AckConnectWorldResult xMsg;
    if(!AFINetServerModule::ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    ARK_SHARE_PTR<ClientConnectData> pConnectData = mxWantToConnectMap.GetElement(xMsg.account());
    if(pConnectData != nullptr)
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
    if(pConnectData != nullptr && strKey == pConnectData->strConnectKey)
    {
        mxWantToConnectMap.RemoveElement(strAccount);
        return true;
    }

    return false;
}

void AFCProxyServerToWorldModule::LogServerInfo(const std::string& strServerInfo)
{
    m_pLogModule->LogInfo(AFGUID(), strServerInfo, "");
}

void AFCProxyServerToWorldModule::OnOtherMessage(const AFIMsgHead& xHead, const int nMsgID, const char * msg, const uint32_t nLen, const AFGUID& xClientID)
{
    m_pProxyServerNet_ServerModule->Transpond(xHead, nMsgID, msg, nLen);
}

void AFCProxyServerToWorldModule::OnBrocastmsg(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //保持记录,直到下线,或者1分钟不上线即可删除
    AFGUID nPlayerID;
    AFMsg::BrocastMsg xMsg;
    if(!AFINetServerModule::ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    for(int i = 0; i < xMsg.player_client_list_size(); i++)
    {
        const AFMsg::Ident& xClientID = xMsg.player_client_list(i);
        m_pProxyServerNet_ServerModule->SendToPlayerClient(xMsg.nmsgid(), xMsg.msg_data().c_str(), xMsg.msg_data().size(), AFINetServerModule::PBToGUID(xClientID), nPlayerID);
    }
}