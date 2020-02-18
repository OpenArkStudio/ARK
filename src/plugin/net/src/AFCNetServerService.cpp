/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "net/include/AFCTCPServer.hpp"
#include "net/include/AFCNetServerService.hpp"

namespace ark {

AFCNetServerService::AFCNetServerService(AFPluginManager* p)
    : m_pPluginManager(p)
{
    m_pNetServiceManagerModule = m_pPluginManager->FindModule<AFINetServiceManagerModule>();
    m_pLogModule = m_pPluginManager->FindModule<AFILogModule>();
    m_pMsgModule = m_pPluginManager->FindModule<AFIMsgModule>();

    ARK_ASSERT_NO_EFFECT(m_pNetServiceManagerModule != nullptr && m_pLogModule != nullptr && m_pMsgModule != nullptr);
}

//AFCNetServerService::~AFCNetServerService()
//{
//    ARK_DELETE(m_pNet); // shutdown will be called in destructor
//}

bool AFCNetServerService::Start(const AFHeadLength len, const int bus_id, const AFEndpoint& ep,
    const uint8_t thread_count, const uint32_t max_connection)
{
    bool ret = false;
    if (ep.proto() == proto_type::tcp)
    {
        m_pNet = std::make_shared<AFCTCPServer>(this, &AFCNetServerService::OnNetMsg, &AFCNetServerService::OnNetEvent);
        ret = m_pNet->StartServer(len, bus_id, ep.GetIP(), ep.GetPort(), thread_count, max_connection, ep.IsV6());

        //AFINetServerService::RegMsgCallback(
        //    AFMsg::E_SS_MSG_ID_SERVER_REPORT, this, &AFCNetServerService::OnClientRegister);
    }
    else if (ep.proto() == proto_type::udp)
    {
        // for now, do not support udp server
    }
    else if (ep.proto() == proto_type::ws)
    {
        // will add web-socket server
    }
    else
    {
        // other protocol will be supported
        return false;
    }

    return ret;
}

bool AFCNetServerService::Update()
{
    ARK_ASSERT_RET_VAL(m_pNet != nullptr, false);
    m_pNet->Update();
    return true;
}

std::shared_ptr<AFINet> AFCNetServerService::GetNet()
{
    return m_pNet;
}

bool AFCNetServerService::RegMsgCallback(const int msg_id, NET_MSG_FUNCTOR&& cb)
{
    if (net_msg_callbacks_.find(msg_id) != net_msg_callbacks_.end())
    {
        return false;
    }
    else
    {
        net_msg_callbacks_.insert(std::make_pair(msg_id, std::forward<NET_MSG_FUNCTOR>(cb)));
        return true;
    }
}

bool AFCNetServerService::RegForwardMsgCallback(NET_MSG_FUNCTOR&& cb)
{
    net_forward_msg_callbacks_.push_back(std::forward<NET_MSG_FUNCTOR>(cb));
    return true;
}

bool AFCNetServerService::RegNetEventCallback(NET_EVENT_FUNCTOR&& cb)
{
    net_event_callbacks_.push_back(std::forward<NET_EVENT_FUNCTOR>(cb));
    return true;
}

bool AFCNetServerService::RegRegServerCallBack(NET_MSG_SESSION_FUNCTOR&& cb)
{
    reg_server_call_back_ = std::forward<NET_MSG_SESSION_FUNCTOR>(cb);
    return true;
}

void AFCNetServerService::OnNetMsg(const AFNetMsg* msg, const AFGUID& session_id)
{
    if (msg->GetMsgId() == AFMsg::e_ss_msg_id::E_SS_MSG_ID_REG_TO_SERVER)
    {
        reg_server_call_back_(msg, session_id);
        return;
    }

    auto it = net_msg_callbacks_.find(msg->GetMsgId());
    if (it != net_msg_callbacks_.end())
    {
        (it->second)(msg);
    }
    else
    {
        // TODO:forward to other server process

        ARK_LOG_ERROR("Invalid message, id = {}", msg->GetMsgId());

        // for (const auto& iter : mxCallBackList)
        //{
        //    (iter)(head, msg_id, msg, msg_len, session_id);
        //}
    }
}

void AFCNetServerService::OnNetEvent(const AFNetEvent* event)
{
    switch (event->GetType())
    {
        case AFNetEventType::CONNECTED:
            ARK_LOG_INFO("Connected server = {} succenssfully, ip = {}, session_id = {}",
                AFBusAddr(event->GetBusId()).ToString(), event->GetIP(), event->GetId());
            // TODO: NickYang, add bus relation of AFINet
            break;
        case AFNetEventType::DISCONNECTED:
            ARK_LOG_ERROR("Disconnected server = {} succenssfully, ip = {}, session_id = {}",
                AFBusAddr(event->GetBusId()).ToString(), event->GetIP(), event->GetId());
            m_pNetServiceManagerModule->RemoveNetConnectionBus(event->GetBusId());
            break;
        default:
            break;
    }

    for (const auto& it : net_event_callbacks_)
    {
        (it)(event);
    }
}

//void AFCNetServerService::OnClientRegister(const AFNetMsg* msg, const int64_t session_id)
//{
//    ARK_PROCESS_MSG(msg, AFMsg::msg_ss_server_report);
//
//    // Add server_bus_id -> client_bus_id relationship with net
//    m_pNetServiceManagerModule->AddNetConnectionBus(pb_msg.bus_id(), m_pNet);
//    //////////////////////////////////////////////////////////////////////////
//    std::shared_ptr<AFServerData> server_data_ptr = reg_clients_.find_value(pb_msg.bus_id());
//    if (nullptr == server_data_ptr)
//    {
//        server_data_ptr = std::make_shared<AFServerData>();
//        reg_clients_.insert(pb_msg.bus_id(), server_data_ptr);
//    }
//
//    server_data_ptr->Init(session_id, pb_msg);
//    //////////////////////////////////////////////////////////////////////////
//
//    SyncToAllClient(pb_msg.bus_id(), session_id);
//}

//void AFCNetServerService::SyncToAllClient(const int bus_id, const AFGUID& session_id)
//{
//    AFMsg::msg_ss_server_notify msg;
//
//    for (auto iter : reg_clients_)
//    {
//        auto server_data = iter.second;
//        AFMsg::msg_ss_server_report* report = msg.add_server_list();
//        *report = server_data->server_info_;
//        m_pMsgModule->SendSSMsg(bus_id, AFMsg::E_SS_MSG_ID_SERVER_NOTIFY, msg, session_id);
//    }
//}

} // namespace ark
