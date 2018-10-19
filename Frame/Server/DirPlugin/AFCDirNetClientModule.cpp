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

#include "AFCDirNetClientModule.h"

namespace ark
{

    bool AFCDirNetClientModule::Init()
    {
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pMsgModule = pPluginManager->FindModule<AFIMsgModule>();
        m_pDirNetServerModule = pPluginManager->FindModule<AFIDirNetServerModule>();
        m_pNetClientManagerModule = pPluginManager->FindModule<AFINetClientManagerModule>();

        return true;
    }

    bool AFCDirNetClientModule::PreUpdate()
    {
        int ret = StartClient();
        return (ret == 0);
    }

    int AFCDirNetClientModule::StartClient()
    {
        //创建所有与对端链接的client
        int ret = m_pNetClientManagerModule->CreateClusterClients();
        if (ret != 0)
        {
            ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
            ARK_ASSERT_NO_EFFECT(0);
            return ret;
        }

        AFINetClientService* pNetClientMaster = m_pNetClientManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_MASTER);
        if (pNetClientMaster == nullptr)
        {
            return 0;
        }

        //pNetClientMaster->AddRecvCallback(AFMsg::E_SS_MSG_ID_SERVER_NOTIFY, this, &AFCDirNetClientModule::OnRecvServerNotify);
        //pNetClientMaster->AddRecvCallback(this, &AFCDirNetClientModule::InvalidMessage);

        //pNetClientMaster->AddNetEventCallBack(this, &AFCDirNetClientModule::OnSocketEvent);

        return 0;
    }

    //void AFCDirNetClientModule::Register(const int bus_id)
    //{
    //    AFINetClientService* pNetClient = m_pNetClientManagerModule->GetNetClientServiceByBusID(bus_id);
    //    if (pNetClient == nullptr)
    //    {
    //        ARK_ASSERT_NO_EFFECT(0);
    //        return;
    //    }

    //    const AFServerConfig* server_config = m_pBusModule->GetAppServerInfo();
    //    if (server_config == nullptr)
    //    {
    //        ARK_ASSERT_NO_EFFECT(0);
    //        return;
    //    }

    //    AFMsg::msg_ss_server_report report_msg;
    //    report_msg.set_bus_id(server_config->self_id);
    //    report_msg.set_url(server_config->public_ep_.to_string());
    //    report_msg.set_cur_online(0);
    //    report_msg.set_max_online(server_config->max_connection);
    //    report_msg.set_logic_status(AFMsg::e_ss_server_state::E_ST_NARMAL);
    //    m_pMsgModule->SendParticularSSMsg(bus_id, AFMsg::E_SS_MSG_ID_SERVER_REPORT, report_msg);
    //}

    //void AFCDirNetClientModule::InvalidMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    //{
    //    ARK_LOG_ERROR("invalid msg id = {}", nMsgID);
    //}

    //void AFCDirNetClientModule::OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id)
    //{
    //    switch (event)
    //    {
    //    case CONNECTED:
    //        {
    //            ARK_LOG_INFO("Connected success, id = {}", conn_id.ToString());
    //            Register(bus_id);
    //        }
    //        break;
    //    case DISCONNECTED:
    //        ARK_LOG_INFO("Connection closed, id = {}", conn_id.ToString());
    //        break;
    //    default:
    //        break;
    //    }
    //}

    //void AFCDirNetClientModule::OnClientConnected(const AFGUID& xClientID)
    //{
    //    //do something
    //}

    //void AFCDirNetClientModule::OnClientDisconnect(const AFGUID& xClientID)
    //{
    //    //do something
    //}

    //void AFCDirNetClientModule::OnRecvServerNotify(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id)
    //{
    //    ARK_PROCESS_MSG(head, msg, msg_len, AFMsg::msg_ss_server_notify);
    //    for (int i = 0; i < x_msg.server_list_size(); ++i)
    //    {
    //        const AFMsg::msg_ss_server_report& report = x_msg.server_list(i);

    //        AFBusAddr bus_addr(report.bus_id());
    //        auto zone = std::make_pair(bus_addr.channel_id, bus_addr.zone_id);
    //        auto iter = proxy_server_infos_.find(zone);
    //        if (iter != proxy_server_infos_.end())
    //        {
    //            iter->second.insert(std::make_pair(report.bus_id(), report));
    //        }
    //        else
    //        {
    //            std::map<int, AFMsg::msg_ss_server_report> proxys;
    //            proxys.insert(std::make_pair(report.bus_id(), report));
    //            proxy_server_infos_.insert(std::make_pair(zone, proxys));
    //        }

    //        //同时保存小区号，以供发给客户端作为列表
    //    }
    //}

    //will add get zone list and select zone

}