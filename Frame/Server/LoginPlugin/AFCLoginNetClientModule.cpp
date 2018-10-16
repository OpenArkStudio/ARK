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

#include "AFCLoginNetClientModule.h"

namespace ark
{

    bool AFCLoginNetClientModule::Init()
    {
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pMsgModule = pPluginManager->FindModule<AFIMsgModule>();
        m_pLoginNetServerModule = pPluginManager->FindModule<AFILoginNetServerModule>();
        m_pNetClientManagerModule = pPluginManager->FindModule<AFINetClientManagerModule>();

        return true;
    }

    bool AFCLoginNetClientModule::PreUpdate()
    {
        int ret = StartClient();
        return (ret == 0);
    }

    int AFCLoginNetClientModule::StartClient()
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

        pNetClient->AddEventCallBack(this, &AFCLoginNetClientModule::OnSocketEvent);

        return 0;
    }

    void AFCLoginNetClientModule::OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id)
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

    void AFCLoginNetClientModule::Register(const int bus_id)
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

    void AFCLoginNetClientModule::OnSelectServerResultProcess(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id)
    {
        ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::AckConnectWorldResult);
        m_pLoginNetServerModule->OnSelectWorldResultsProcess(x_msg.world_id(), AFIMsgModule::PBToGUID(x_msg.sender()), x_msg.login_id(), x_msg.account(), x_msg.world_url(), x_msg.world_key());
    }

    void AFCLoginNetClientModule::OnWorldInfoProcess(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id)
    {
        ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::ServerInfoReportList);

        for (int i = 0; i < x_msg.server_list_size(); ++i)
        {
            const AFMsg::ServerInfoReport& xData = x_msg.server_list(i);

            ARK_SHARE_PTR<AFMsg::ServerInfoReport> pServerData = mWorldMap.GetElement(xData.bus_id());

            if (nullptr == pServerData)
            {
                pServerData = std::make_shared<AFMsg::ServerInfoReport>();
                *pServerData = xData;

                mWorldMap.AddElement(xData.bus_id(), pServerData);
            }

        }

        ARK_LOG_INFO("WorldInfo size = {}", x_msg.server_list_size());
    }

    AFMapEx<int, AFMsg::ServerInfoReport>& AFCLoginNetClientModule::GetWorldMap()
    {
        return mWorldMap;
    }

}