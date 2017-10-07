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

#include "AFCWorldToMasterModule.h"
#include "AFWorldNetClientPlugin.h"
#include "SDK/Base/AFCDataList.h"
#include "SDK/Proto/AFMsgDefine.h"
#include "SDK/Interface/AFINetClientModule.hpp"

bool AFCWorldToMasterModule::Init()
{
    m_pNetClientModule = ARK_NEW AFINetClientModule(pPluginManager);

    m_pNetClientModule->Init();

    return true;
}

bool AFCWorldToMasterModule::Shut()
{
    return true;
}

bool AFCWorldToMasterModule::AfterInit()
{
    m_pWorldLogicModule = pPluginManager->FindModule<AFIWorldLogicModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pWorldNet_ServerModule = pPluginManager->FindModule<AFIWorldNetServerModule>();

    m_pNetClientModule->AddReceiveCallBack(AFMsg::EGMI_REQ_CONNECT_WORLD, this, &AFCWorldToMasterModule::OnSelectServerProcess);
    m_pNetClientModule->AddReceiveCallBack(AFMsg::EGMI_REQ_KICK_CLIENT_INWORLD, this, &AFCWorldToMasterModule::OnKickClientProcess);
    m_pNetClientModule->AddReceiveCallBack(this, &AFCWorldToMasterModule::InvalidMessage);

    m_pNetClientModule->AddEventCallBack(this, &AFCWorldToMasterModule::OnSocketMSEvent);

    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetPropertyInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetPropertyInt(strConfigName, "ServerID");
            if(nServerType == NF_SERVER_TYPES::NF_ST_MASTER)
            {
                const int nPort = m_pElementModule->GetPropertyInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetPropertyInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetPropertyInt(strConfigName, "CpuCount");
                const std::string strName(m_pElementModule->GetPropertyString(strConfigName, "Name"));
                const std::string strIP(m_pElementModule->GetPropertyString(strConfigName, "IP"));

                ConnectData xServerData;

                xServerData.nGameID = nServerID;
                xServerData.eServerType = (NF_SERVER_TYPES)nServerType;
                xServerData.strIP = strIP;
                xServerData.nPort = nPort;
                xServerData.strName = strName;

                m_pNetClientModule->AddServer(xServerData);
            }
        }
    }

    return true;
}


bool AFCWorldToMasterModule::Execute()
{
    m_pNetClientModule->Execute();

    return true;
}

void AFCWorldToMasterModule::Register(const int nServerID)
{
    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetPropertyInt(strConfigName, "Type");
            const int nSelfServerID = m_pElementModule->GetPropertyInt(strConfigName, "ServerID");
            if(nServerType == NF_SERVER_TYPES::NF_ST_WORLD && pPluginManager->AppID() == nSelfServerID)
            {
                const int nPort = m_pElementModule->GetPropertyInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetPropertyInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetPropertyInt(strConfigName, "CpuCount");
                const std::string strName(m_pElementModule->GetPropertyString(strConfigName, "Name"));
                const std::string strIP(m_pElementModule->GetPropertyString(strConfigName, "IP"));

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

                ARK_SHARE_PTR<ConnectData> pServerData = m_pNetClientModule->GetServerNetInfo(nServerID);
                if(pServerData)
                {
                    int nTargetID = pServerData->nGameID;
                    m_pNetClientModule->SendToServerByPB(nTargetID, AFMsg::EGameMsgID::EGMI_MTL_WORLD_REGISTERED, xMsg, 0);

                    m_pLogModule->LogInfo(AFGUID(0, pData->server_id()), pData->server_name(), "Register");
                }
            }
        }
    }
}

void AFCWorldToMasterModule::RefreshWorldInfo()
{

}

void AFCWorldToMasterModule::OnSelectServerProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqConnectWorld xMsg;
    if(!AFINetServerModule::ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    ARK_SHARE_PTR<ServerData> xServerData = m_pWorldNet_ServerModule->GetSuitProxyForEnter();
    if(xServerData)
    {
        AFMsg::AckConnectWorldResult xData;

        xData.set_world_id(xMsg.world_id());
        xData.mutable_sender()->CopyFrom(xMsg.sender());
        xData.set_login_id(xMsg.login_id());
        xData.set_account(xMsg.account());

        xData.set_world_ip(xServerData->pData->server_ip());
        xData.set_world_port(xServerData->pData->server_port());
        xData.set_world_key(xMsg.account());

        m_pWorldNet_ServerModule->GetNetModule()->SendMsgPB(AFMsg::EGMI_ACK_CONNECT_WORLD, xData, xServerData->xClient, nPlayerID);

        m_pNetClientModule->SendSuitByPB(xMsg.account(), AFMsg::EGMI_ACK_CONNECT_WORLD, xData, xHead.GetPlayerID());
    }

}

void AFCWorldToMasterModule::OnKickClientProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    AFMsg::ReqKickFromWorld xMsg;
    if(!AFINetServerModule::ReceivePB(xHead, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    //T人,下线
    //     AFIDataList var;
    //     var << xMsg.world_id() << xMsg.account();
    //     m_pEventProcessModule->DoEvent(AFGUID(), AFED_ON_KICK_FROM_SERVER, var);
}

void AFCWorldToMasterModule::InvalidMessage(const AFIMsgHead& xHead, const int nMsgID, const char * msg, const uint32_t nLen, const AFGUID& xClientID)
{
    printf("NFNet || 非法消息:unMsgID=%d\n", nMsgID);
}

void AFCWorldToMasterModule::OnSocketMSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
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

void AFCWorldToMasterModule::OnClientDisconnect(const AFGUID& xClientID)
{

}

void AFCWorldToMasterModule::OnClientConnected(const AFGUID& xClientID)
{

}

bool AFCWorldToMasterModule::BeforeShut()
{
    return true;
}

void AFCWorldToMasterModule::LogServerInfo(const std::string& strServerInfo)
{
    m_pLogModule->LogInfo(AFGUID(), strServerInfo, "");
}

