/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "master/include/AFCMasterNetModule.hpp"

namespace ark {

bool AFCMasterNetModule::Init()
{
    m_pBusModule = FindModule<AFIBusModule>();
    m_pNetServiceManagerModule = FindModule<AFINetServiceManagerModule>();
    m_pTimerModule = FindModule<AFITimerModule>();
    m_pMsgModule = FindModule<AFIMsgModule>();

    // log timer
    // m_pTimerModule->AddForeverTimer("master-sub-server-timer", BusID(), 30 * 1000, this,
    // &AFCMasterNetServerModule::OnTimerLogServer);

    return true;
}

bool AFCMasterNetModule::PostInit()
{
    int ret = StartServer();
    if (ret != 0)
    {
        exit(0);
    }

    return true;
}

int AFCMasterNetModule::StartServer()
{
    auto ret = m_pNetServiceManagerModule->CreateServer();
    if (!ret)
    {
        ARK_LOG_ERROR("Cannot start server net, busid = {}", m_pBusModule->GetSelfBusName());
        ARK_ASSERT_NO_EFFECT(0);
        exit(0);
    }

    m_pNetServer = m_pNetServiceManagerModule->GetInterNetServer();
    if (m_pNetServer == nullptr)
    {
        ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->GetSelfBusName());
        exit(0);
    }

    return 0;
}

// void AFCMasterNetServerModule::OnSocketEvent(const NetEventType event, const guid_t& conn_id, const std::string& ip,
// bus_id_t bus_id)
//{
//    switch (event)
//    {
//    case CONNECTED:
//        {
//            ARK_LOG_INFO("Connected success, id = {}", conn_id.ToString());
//            OnClientConnected(conn_id);
//        }
//        break;
//    case DISCONNECTED:
//        {
//            ARK_LOG_INFO("Connection closed, id = {}", conn_id.ToString());
//            OnClientDisconnect(bus_id, conn_id);
//        }
//        break;
//    default:
//        break;
//    }
//}

// void AFCMasterNetServerModule::OnClientConnected(const guid_t& conn_id)
//{
//
//}

// void AFCMasterNetServerModule::OnClientDisconnect(bus_id_t bus_id, const guid_t& conn_id)
//{
//    reg_servers_.RemoveElement(bus_id);
//}

// void AFCMasterNetServerModule::OnHeartBeat(const ARK_PKG_BASE_HEAD& head, msg_id_t msg_id, const char* msg, const
// uint32_t msg_len, const guid_t& conn_id)
//{
//    //do nothing
//}

//////////////////////////////////////////////////////////////////////////
// void AFCMasterNetServerModule::OnTimerLogServer(const std::string& name, const guid_t& id)
//{
//    ARK_LOG_INFO("---------------------------Start to log all registered server---------------------------");
//    for (bool ret = reg_servers_.Begin(); ret; ret = reg_servers_.Increase())
//    {
//        auto& server_data = reg_servers_.GetCurrentData();
//        ARK_LOG_INFO("bus[{}] state[{}] url[{}] conn_id[{}]",
//                     server_data->server_info_.bus_id(),
//                     AFMsg::e_ss_server_state_Name(server_data->server_info_.logic_status()),
//                     server_data->conn_id_.nLow);
//    }
//    ARK_LOG_INFO("---------------------------End to log all registered server---------------------------");
//}

} // namespace ark
