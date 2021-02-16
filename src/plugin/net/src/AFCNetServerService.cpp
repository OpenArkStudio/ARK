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

#include "net/include/AFCTCPServer.hpp"
#include "net/include/AFCRDPServer.hpp"
#include "net/include/AFCNetServerService.hpp"

namespace ark {

AFCNetServerService::AFCNetServerService(AFPluginManager* p)
    : m_pPluginManager(p)
{
    m_pNetServiceManagerModule = m_pPluginManager->FindModule<AFINetServiceManagerModule>();
    m_pMsgModule = m_pPluginManager->FindModule<AFIMsgModule>();
}

//AFCNetServerService::~AFCNetServerService()
//{
//    ARK_DELETE(m_pNet); // shutdown will be called in destructor
//}

bool AFCNetServerService::Start(std::shared_ptr<const AFIMsgHeader> head, bus_id_t bus_id, const AFEndpoint& ep,
    uint8_t thread_count, uint32_t max_connection, const size_t silent_timeout)
{
    bool ret = false;
    if (ep.proto() == proto_type::tcp)
    {
        m_pNet = std::make_shared<AFCTCPServer>(this, &AFCNetServerService::OnNetMsg, &AFCNetServerService::OnNetEvent);
        ret = m_pNet->StartServer(head, bus_id, ep.GetIP(), ep.GetPort(), thread_count, max_connection, silent_timeout);

        //AFINetServerService::RegMsgCallback(
        //    AFMsg::E_SS_MSG_ID_SERVER_REPORT, this, &AFCNetServerService::OnClientRegister);
    }
    else if (ep.proto() == proto_type::udp)
    {
        m_pNet = std::make_shared<AFCRDPServer>(this, &AFCNetServerService::OnNetMsg, &AFCNetServerService::OnNetEvent);
        ret = m_pNet->StartServer(head, bus_id, ep.GetIP(), ep.GetPort(), thread_count, max_connection, silent_timeout);
    }
    else if (ep.proto() == proto_type::ws)
    {
        // will add websocket server
    }
    else
    {
        // other protocols will be supported
        return false;
    }

    return ret;
}

bool AFCNetServerService::Update()
{
    ARK_ASSERT_RET_VAL(m_pNet != nullptr, false);
    m_pNet->Update();

    //ProcessConnection();
    ProcessClientConnection();
    return true;
}

std::shared_ptr<AFINet> AFCNetServerService::GetNet()
{
    return m_pNet;
}

bool AFCNetServerService::RegMsgCallback(msg_id_t msg_id, NET_MSG_FUNCTOR&& cb)
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

void AFCNetServerService::SetTimeoutInterval(const int32_t interval)
{
    if (interval <= 0)
    {
        return;
    }

    client_timeout_interval_ = interval;
}

void AFCNetServerService::OnNetMsg(const AFNetMsg* msg, conv_id_t session_id)
{
    ARK_LOG_DEBUG("[msg][recv] msg id = {} session id = {}", msg->GetMsgId(), session_id);

    auto msg_id = msg->GetMsgId();
    if (msg_id == AFMsg::E_SS_MSG_ID_REG_TO_SERVER)
    {
        reg_server_call_back_(msg, session_id);

        m_pNetServiceManagerModule->RemoveNetConnectionBus(msg->GetSrcBus());
        m_pNetServiceManagerModule->AddNetConnectionBus(msg->GetSrcBus(), GetNet());

        RemoveConnection(msg->GetSrcBus());
        AddConnection(msg->GetSrcBus(), session_id);
        return;
    }
    else if (msg_id == AFMsg::E_SS_MSG_ID_HEART_BEAT)
    {
        UpdateConnection(msg->GetSrcBus());
        return;
    }

    if (msg->GetSrcBus() == 0)
    {
        UpdateClientConnection(session_id);
    }

    auto it = net_msg_callbacks_.find(msg_id);
    if (it != net_msg_callbacks_.end())
    {
        (it->second)(msg, session_id);
    }
    else
    {
        // TODO:forward to other server process

        ARK_LOG_ERROR("Invalid message, id = {}", msg_id);

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
    case AFNetEventType::CONNECT:
        ARK_LOG_INFO("Connected server = {} succenssfully, ip = {}, session_id = {}",
            AFBusAddr(event->GetBusId()).ToString(), event->GetIP(), event->GetId());
        // TODO: NickYang, add bus relation of AFINet
        break;
    case AFNetEventType::DISCONNECT:
        ARK_LOG_INFO("Disconnected server = {} succenssfully, ip = {}, session_id = {}",
            AFBusAddr(event->GetBusId()).ToString(), event->GetIP(), event->GetId());
        //m_pNetServiceManagerModule->RemoveSessionID(event->GetId());
        //m_pNetServiceManagerModule->RemoveNetConnectionBus(event->GetBusId());
        RemoveClientConnection(event->GetId());
        break;
    default:
        break;
    }

    for (const auto& it : net_event_callbacks_)
    {
        (it)(event);
    }
}

void AFCNetServerService::OnDisconnectNetEvent(const AFNetEvent* event)
{
    ARK_LOG_INFO("TimeOut Disconnected server = {} succenssfully, ip = {}, session_id = {}",
        AFBusAddr(event->GetBusId()).ToString(), event->GetIP(), event->GetId());
	
    for (const auto& it : net_event_callbacks_)
    {
        (it)(event);
    }
}

//void AFCNetServerService::ProcessConnection()
//{
//    ///check connection
//    auto now = AFDateTime::GetNowTime();
//    for (auto& iter : connection_list_)
//    {
//        auto connection = iter.second;
//        if (!connection)
//        {
//            continue;
//        }
//
//        if (connection->connect_state_ != AFConnection::eConnectState::CONNECTED)
//        {
//            continue;
//        }
//
//        if (connection->last_active_time_ + 30 * 1000 < now)
//        {
//            connection->connect_state_ = AFConnection::eConnectState::DISCONNECT;
//
//            /// close session
//            connection_list_.erase(iter.first);
//            m_pNet->CloseSession(connection->session_id_);
//
//            /// todo
//            AFNetEvent event;
//            event.SetType(AFNetEventType::DISCONNECT);
//            event.SetBusId(connection->bus_id_);
//            event.SetId(connection->session_id_);
//            OnNetEvent(&event);
//            break;
//        }
//    }
//}

void AFCNetServerService::ProcessClientConnection()
{
    std::list<conv_id_t> close_list;
	
    ///check connection
    auto now = AFDateTime::GetNowTime();
    for (auto& iter : client_connection_list_)
    {
        auto& connection = iter.second;
        if (connection.last_active_time_ + client_timeout_interval_ < now)
        {
            /// close session
            close_list.push_back(iter.first);
            m_pNet->CloseSession(iter.first);

            AFNetEvent event;
            event.SetType(AFNetEventType::DISCONNECT);
            event.SetBusId(connection.bus_id_);
            event.SetId(connection.session_id_);
            OnDisconnectNetEvent(&event);
        }
    }

	for (auto iter : close_list)
	{
        RemoveClientConnection(iter);
	}
}

void AFCNetServerService::AddConnection(bus_id_t bus_id, conv_id_t session_id)
{
    //if (connection_list_.find_value(bus_id))
    //{
    //    return;
    //}
    RemoveConnection(bus_id);

    auto connection = std::make_shared<AFConnection>(
        bus_id, session_id, AFDateTime::GetNowTime(), AFConnection::eConnectState::CONNECTED);
    connection_list_.insert(bus_id, connection);
}

void AFCNetServerService::UpdateConnection(bus_id_t bus_id)
{
    auto connection = connection_list_.find_value(bus_id);
    if (!connection)
    {
        return;
    }

    auto now = AFDateTime::GetNowTime();
    connection->last_active_time_ = now;
}

void AFCNetServerService::RemoveConnection(bus_id_t bus_id)
{
    connection_list_.erase(bus_id);
}

void AFCNetServerService::UpdateClientConnection(conv_id_t session_id)
{
    auto iter = client_connection_list_.find(session_id);
    if (iter == client_connection_list_.end())
    {
        auto connection = AFConnection(0, session_id, AFDateTime::GetNowTime(), AFConnection::eConnectState::CONNECTED);
        client_connection_list_.insert(std::make_pair(session_id, connection));
    }
    else
    {
        iter->second.last_active_time_ = AFDateTime::GetNowTime();
    }
}

//void AFCNetServerService::OnClientRegister(const AFNetMsg* msg, conv_id_t session_id)
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

//void AFCNetServerService::SyncToAllClient(bus_id_t bus_id, conv_id_t session_id)
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
