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

#include "AFCRouterNetClientModule.h"

namespace ark
{

    bool AFCRouterNetClientModule::Init()
    {
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pMsgModule = pPluginManager->FindModule<AFIMsgModule>();
        m_pNetClientManagerModule = pPluginManager->FindModule<AFINetClientManagerModule>();

        return true;
    }

    bool AFCRouterNetClientModule::PreUpdate()
    {
        int ret = StartClient();
        return (ret == 0);
    }

    int AFCRouterNetClientModule::StartClient()
    {
        //创建所有与对端链接的client
        int ret = m_pNetClientManagerModule->CreateClusterClients();
        if (ret != 0)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            ARK_ASSERT_NO_EFFECT(0);
            return ret;
        }

        AFINetClientService* pNetClientWorld = m_pNetClientManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_MASTER);
        if (pNetClientWorld == nullptr)
        {
            return 0;
        }

        pNetClientWorld->AddRecvCallback(this, &AFCRouterNetClientModule::InvalidMessage);
        pNetClientWorld->AddEventCallBack(this, &AFCRouterNetClientModule::OnSocketEvent);

        return 0;
    }

    void AFCRouterNetClientModule::Register(const int bus_id)
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
        pData->set_url(server_config->public_ep_.to_string());
        pData->set_max_online(server_config->max_connection);
        pData->set_logic_status(AFMsg::EST_NARMAL);

        m_pMsgModule->SendParticularSSMsg(bus_id, AFMsg::EGameMsgID::EGMI_GTW_GAME_REGISTERED, xMsg);
        ARK_LOG_INFO("Register self server_id = {}", pData->bus_id());
    }

    void AFCRouterNetClientModule::InvalidMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        ARK_LOG_ERROR("invalid msg id = {}", nMsgID);
    }

    void AFCRouterNetClientModule::OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id)
    {
        switch (event)
        {
        case CONNECTED:
            {
                ARK_LOG_INFO("Connected success, id = {}", conn_id.ToString());
                Register(bus_id);
            }
            break;
        case DISCONNECTED:
            ARK_LOG_INFO("Connection closed, id = {}", conn_id.ToString());
            break;
        default:
            break;
        }
    }

    void AFCRouterNetClientModule::OnClientDisconnect(const AFGUID& xClientID)
    {
        //do something
    }

    void AFCRouterNetClientModule::OnClientConnected(const AFGUID& xClientID)
    {
        //do something
    }

}