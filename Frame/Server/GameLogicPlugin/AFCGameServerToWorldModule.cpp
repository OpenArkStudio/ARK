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
#include "SDK/Proto/AFProtoCPP.hpp"
#include "SDK/Proto/ARKDataDefine.hpp"
#include "Server/Interface/AFINetClientModule.hpp"

bool AFCGameServerToWorldModule::Init()
{
    m_pNetClientManagerModule = pPluginManager->FindModule<AFINetClientManagerModule>();
    m_pNetClientModule = m_pNetClientManagerModule->CreateClusterClientModule(ARK_PROCESS_TYPE::ARK_PROC_WORLD);
    ARK_ASSERT_RET_VAL(nullptr != m_pNetClientModule, false);

    return true;
}

void AFCGameServerToWorldModule::Register(const int nSeverID)
{
    //成功就注册
    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");

    if (nullptr == xLogicClass)
    {
        return;
    }

    AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
    std::string strConfigName;

    for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
    {
        const int nServerType = m_pConfigModule->GetNodeInt(strConfigName, "Type");
        const int nSelfServerID = m_pConfigModule->GetNodeInt(strConfigName, "ServerID");

        if (nServerType == ARK_PROCESS_TYPE::ARK_PROC_GAME && pPluginManager->BusID() == nSelfServerID)
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

            ARK_SHARE_PTR<ConnectData> pServerData = m_pNetClientModule->GetServerNetInfo(nSeverID);

            if (pServerData)
            {
                int nTargetID = pServerData->nGameID;
                m_pNetClientModule->SendToServerByPB(nTargetID, AFMsg::EGameMsgID::EGMI_GTW_GAME_REGISTERED, xMsg, 0);

                ARK_LOG_INFO("Register, server_id  = {} server_name = {}", pData->server_id(), pData->server_name());
            }
        }
    }
}

void AFCGameServerToWorldModule::RefreshWorldInfo()
{
    //do nothing for now
}

bool AFCGameServerToWorldModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pGameServerNet_ServerModule = pPluginManager->FindModule<AFIGameNetServerModule>();

    m_pNetClientModule->AddReceiveCallBack(this, &AFCGameServerToWorldModule::TransPBToProxy);
    m_pNetClientModule->AddEventCallBack(this, &AFCGameServerToWorldModule::OnSocketWSEvent);

    m_pKernelModule->AddClassCallBack(ARK::Player::ThisName(), this, &AFCGameServerToWorldModule::OnObjectClassEvent);

    //////////////////////////////////////////////////////////////////////////
    //TODO:连接上层服务器
    //if (!GetTargetBusRelation())
    //{
    //    return false;
    //}

    //AddServer
    //////////////////////////////////////////////////////////////////////////

    // 连接world server
    ARK_SHARE_PTR<AFIClass> xLogicClass = m_pClassModule->GetElement("Server");

    if (nullptr == xLogicClass)
    {
        return false;
    }

    AFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
    std::string strConfigName;

    for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
    {
        const int nServerType = m_pConfigModule->GetNodeInt(strConfigName, "Type");
        const int nServerID = m_pConfigModule->GetNodeInt(strConfigName, "ServerID");

        if (nServerType == ARK_PROCESS_TYPE::ARK_PROC_WORLD)
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

void AFCGameServerToWorldModule::OnSocketWSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if (eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString());
        Register(nServerID);
    }
}

int AFCGameServerToWorldModule::OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var)
{
    if (strClassName == ARK::Player::ThisName())
    {
        if (ARK_ENTITY_EVENT::ENTITY_EVT_DESTROY == eClassEvent)
        {
            SendOffline(self);
        }
        else if (ARK_ENTITY_EVENT::ENTITY_EVT_ALL_FINISHED == eClassEvent)
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
    ARK_MSG_PROCESS_NO_OBJECT_STRING(xHead, msg, nLen);
    m_pGameServerNet_ServerModule->SendMsgPBToGate(nMsgID, strMsg, nPlayerID);
}

void AFCGameServerToWorldModule::SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen)
{
    m_pNetClientModule->SendBySuit(nHashKey, nMsgID, msg, nLen);
}

AFINetClientModule* AFCGameServerToWorldModule::GetClusterClientModule()
{
    return m_pNetClientModule;
}