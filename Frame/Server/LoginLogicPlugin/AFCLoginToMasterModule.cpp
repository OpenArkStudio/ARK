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

AFCLoginToMasterModule::AFCLoginToMasterModule(AFIPluginManager* p)
{
    pPluginManager = p;
}

bool AFCLoginToMasterModule::Init()
{
    m_pLoginLogicModule         = FindModule<AFILoginLogicModule>();
    m_pLogModule                = FindModule<AFILogModule>();
    m_pLoginNetServerModule     = FindModule<AFILoginNetServerModule>();
    m_pNetClientManagerModule   = FindModule<AFINetClientManagerModule>();
    m_pBusModule                = FindModule<AFIBusModule>();
    m_pMsgModule                = FindModule<AFIMsgModule>();

    return true;
}

bool AFCLoginToMasterModule::PostInit()
{
    int ret = StartClient();
    return (ret == 0);
}

int AFCLoginToMasterModule::StartClient()
{
    //创建所有与对端链接的client
    int ret = m_pNetClientManagerModule->CreateClusterClients();
    if (ret != 0)
    {
        ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
        ARK_ASSERT_NO_EFFECT(0);
        return ret;
    }

    //if need to add a member
    AFINetClientService* pNetClient = m_pNetClientManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_MASTER);
    if (pNetClient == nullptr)
    {
        return -1;
    }

    pNetClient->AddEventCallBack(this, &AFCLoginToMasterModule::OnSocketMSEvent);

    return 0;
}

void AFCLoginToMasterModule::OnSocketMSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
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

void AFCLoginToMasterModule::Register(const int bus_id)
{
    AFINetClientService* pNetClient = m_pNetClientManagerModule->GetNetClientServiceByBusID(bus_id);
    if (pNetClient == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return;
    }

    const AFServerConfig* server_config = m_pBusModule->GetAppServerInfo();
    if (server_config == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return;
    }

    AFMsg::ServerInfoReportList xMsg;
    AFMsg::ServerInfoReport* pData = xMsg.add_server_list();

    pData->set_bus_id(server_config->self_id);
    pData->set_cur_online(0);
    pData->set_url(server_config->public_url);
    pData->set_max_online(server_config->max_connection);
    pData->set_logic_status(AFMsg::EST_NARMAL);

    m_pMsgModule->SendParticularSSMsg(bus_id, AFMsg::EGameMsgID::EGMI_GTW_GAME_REGISTERED, xMsg);
    ARK_LOG_INFO("Register self server_id = {}", pData->bus_id());
}

void AFCLoginToMasterModule::OnSelectServerResultProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::AckConnectWorldResult);
    m_pLoginNetServerModule->OnSelectWorldResultsProcess(xMsg.world_id(), AFIMsgModule::PBToGUID(xMsg.sender()), xMsg.login_id(), xMsg.account(), xMsg.world_url(), xMsg.world_key());
}

void AFCLoginToMasterModule::OnWorldInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, AFMsg::ServerInfoReportList);

    for (int i = 0; i < xMsg.server_list_size(); ++i)
    {
        const AFMsg::ServerInfoReport& xData = xMsg.server_list(i);

        ARK_SHARE_PTR<AFMsg::ServerInfoReport> pServerData = mWorldMap.GetElement(xData.bus_id());

        if (nullptr == pServerData)
        {
            pServerData = std::make_shared<AFMsg::ServerInfoReport>();
            *pServerData = xData;

            mWorldMap.AddElement(xData.bus_id(), pServerData);
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
