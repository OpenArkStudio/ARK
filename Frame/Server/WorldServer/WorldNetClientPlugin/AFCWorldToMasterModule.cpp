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

#include "AFCWorldToMasterModule.h"
#include "AFWorldNetClientPlugin.h"
#include "SDK/Core/Base/AFCDataList.h"
#include "SDK/Proto/AFProtoCPP.hpp"
#include "Server/Interface/AFINetClientModule.hpp"

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

bool AFCWorldToMasterModule::PostInit()
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

    if (nullptr != xLogicClass)
    {
        AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;

        for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetNodeInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetNodeInt(strConfigName, "ServerID");

            if (nServerType == ARK_SERVER_TYPE::ARK_ST_MASTER)
            {
                const int nPort = m_pElementModule->GetNodeInt(strConfigName, "Port");
                const std::string strServerName(m_pElementModule->GetNodeString(strConfigName, "Name"));
                const std::string strIP(m_pElementModule->GetNodeString(strConfigName, "IP"));

                ConnectData xServerData;

                xServerData.nGameID = nServerID;
                xServerData.eServerType = (ARK_SERVER_TYPE)nServerType;
                xServerData.strIP = strIP;
                xServerData.nPort = nPort;
                xServerData.strName = strServerName;

                m_pNetClientModule->AddServer(xServerData);
            }
        }
    }

    return true;
}


bool AFCWorldToMasterModule::Update()
{
    return m_pNetClientModule->Update();
}

void AFCWorldToMasterModule::Register(const int nServerID)
{
    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");

    if (nullptr != xLogicClass)
    {
        AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;

        for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetNodeInt(strConfigName, "Type");
            const int nSelfServerID = m_pElementModule->GetNodeInt(strConfigName, "ServerID");

            if (nServerType == ARK_SERVER_TYPE::ARK_ST_WORLD && pPluginManager->AppID() == nSelfServerID)
            {
                const int nPort = m_pElementModule->GetNodeInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetNodeInt(strConfigName, "MaxOnline");
                const std::string strServerName(m_pElementModule->GetNodeString(strConfigName, "Name"));
                const std::string strIP(m_pElementModule->GetNodeString(strConfigName, "IP"));

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
                    m_pNetClientModule->SendToServerByPB(nTargetID, AFMsg::EGameMsgID::EGMI_MTL_WORLD_REGISTERED, xMsg, 0);

                    ARK_LOG_INFO("Register, server_id = {} server_name = {}", pData->server_id(), pData->server_name());
                }
            }
        }
    }
}

void AFCWorldToMasterModule::RefreshWorldInfo()
{
    //Will add
}

void AFCWorldToMasterModule::OnSelectServerProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqConnectWorld);

    ARK_SHARE_PTR<ServerData> xServerData = m_pWorldNet_ServerModule->GetSuitProxyForEnter();

    if (xServerData)
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
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ReqKickFromWorld);
}

void AFCWorldToMasterModule::InvalidMessage(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    printf("NFNet || 非法消息:unMsgID=%d\n", nMsgID);
}

void AFCWorldToMasterModule::OnSocketMSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
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

void AFCWorldToMasterModule::OnClientDisconnect(const AFGUID& xClientID)
{
    //do something
}

void AFCWorldToMasterModule::OnClientConnected(const AFGUID& xClientID)
{
    //do something
}

void AFCWorldToMasterModule::LogServerInfo(const std::string& strServerInfo)
{
    ARK_LOG_INFO("{}", strServerInfo);
}

