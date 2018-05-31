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

#include "AFCGameServerToWorldModule.h"
#include "AFGameNetClientPlugin.h"
#include "SDK/Proto/AFProtoCPP.hpp"
#include "SDK/Proto/ARKDataDefine.hpp"
#include "Server/Interface/AFINetClientModule.hpp"

bool AFCGameServerToWorldModule::Init()
{
    m_pNetClientModule = ARK_NEW AFINetClientModule(pPluginManager);

    m_pNetClientModule->Init();

    return true;
}

bool AFCGameServerToWorldModule::Shut()
{
    return true;
}


bool AFCGameServerToWorldModule::Update()
{
    return m_pNetClientModule->Update();
}

void AFCGameServerToWorldModule::Register(const int nSeverID)
{
    //成功就注册
    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr == xLogicClass)
    {
        return;
    }

    AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
    std::string strConfigName;
    for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
    {
        const int nServerType = m_pElementModule->GetNodeInt(strConfigName, "Type");
        const int nSelfServerID = m_pElementModule->GetNodeInt(strConfigName, "ServerID");
        if(nServerType == ARK_SERVER_TYPE::ARK_ST_GAME && pPluginManager->AppID() == nSelfServerID)
        {
            const int nPort = m_pElementModule->GetNodeInt(strConfigName, "Port");
            const int nMaxConnect = m_pElementModule->GetNodeInt(strConfigName, "MaxOnline");
            const int nCpus = m_pElementModule->GetNodeInt(strConfigName, "CpuCount");
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

            ARK_SHARE_PTR<ConnectData> pServerData = m_pNetClientModule->GetServerNetInfo(nSeverID);
            if(pServerData)
            {
                int nTargetID = pServerData->nGameID;
                m_pNetClientModule->SendToServerByPB(nTargetID, AFMsg::EGameMsgID::EGMI_GTW_GAME_REGISTERED, xMsg, 0);

                ARK_LOG_INFO("Register, server_id  = {} server_name = {}", pData->server_id(), pData->server_name().c_str());
            }
        }
    }
}

void AFCGameServerToWorldModule::RefreshWorldInfo()
{
    //     _tagPT_KEY_BASE_MSG baseMsg;
    //     AFMsg::ServerInfoReport xMsg;
    //
    //     xMsg.set_server_id(m_pGameLogicModule->GetGameID());
    //     xMsg.set_server_port(m_pGameServerModule->GetServerPort());
    //     xMsg.set_server_cur_count(0);
    //     xMsg.set_server_max_online(50000);
    //
    //     if (xMsg.SerializeToString(&baseMsg.strSyncInfo))
    //     {
    //         baseMsg._unMsgID = GTW_GAME_REFRESH;
    //         RakNet::BitStream oBitStream;
    //         baseMsg.EnCode(&oBitStream);
    //         SendBitStream(&oBitStream);
    //     }
}

bool AFCGameServerToWorldModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pGameServerNet_ServerModule = pPluginManager->FindModule<AFIGameNetServerModule>();

    m_pNetClientModule->AddReceiveCallBack(this, &AFCGameServerToWorldModule::TransPBToProxy);
    m_pNetClientModule->AddEventCallBack(this, &AFCGameServerToWorldModule::OnSocketWSEvent);

    m_pKernelModule->AddClassCallBack(ARK::Player::ThisName(), this, &AFCGameServerToWorldModule::OnObjectClassEvent);

    // 连接world server
    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if (nullptr == xLogicClass)
    {
        return false;
    }

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

    return true;
}

void AFCGameServerToWorldModule::OnSocketWSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if(eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString().c_str());
        Register(nServerID);
    }
}

int AFCGameServerToWorldModule::OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var)
{
    if(strClassName == ARK::Player::ThisName())
    {
        if(ARK_ENTITY_EVENT::ENTITY_EVT_DESTROY == eClassEvent)
        {
            SendOffline(self);
        }
        else if(ARK_ENTITY_EVENT::ENTITY_EVT_ALL_FINISHED == eClassEvent)
        {
            SendOnline(self);
        }
    }

    return 0;
}

void AFCGameServerToWorldModule::SendOnline(const AFGUID& self)
{
    AFMsg::RoleOnlineNotify xMsg;

    const AFGUID& xGuild = m_pKernelModule->GetNodeObject(self, "GuildID");
    *xMsg.mutable_guild() = AFINetModule::GUIDToPB(xGuild);

    m_pNetClientModule->SendSuitByPB(xGuild.nLow, AFMsg::EGMI_ACK_ONLINE_NOTIFY, xMsg, self);

}

void AFCGameServerToWorldModule::SendOffline(const AFGUID& self)
{
    AFMsg::RoleOfflineNotify xMsg;

    const AFGUID& xGuild = m_pKernelModule->GetNodeObject(self, "GuildID");
    *xMsg.mutable_guild() = AFINetModule::GUIDToPB(xGuild);

    m_pNetClientModule->SendSuitByPB(xGuild.nLow, AFMsg::EGMI_ACK_OFFLINE_NOTIFY, xMsg, self);

}

void AFCGameServerToWorldModule::TransPBToProxy(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    AFGUID nPlayerID;
    std::string strData;
    if(!AFINetServerModule::ReceivePB(xHead, nMsgID, msg, nLen, strData, nPlayerID))
    {
        return;
    }

    m_pGameServerNet_ServerModule->SendMsgPBToGate(nMsgID, strData, nPlayerID);

    return;
}

void AFCGameServerToWorldModule::SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen)
{
    m_pNetClientModule->SendBySuit(nHashKey, nMsgID, msg, nLen);
}

AFINetClientModule* AFCGameServerToWorldModule::GetClusterClientModule()
{
    return m_pNetClientModule;
}