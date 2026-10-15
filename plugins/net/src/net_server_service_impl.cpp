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

#include "ark/net/net_server_service_impl.hpp"
#include "ark/net/rudp_server.hpp"
#include "ark/net/tcp_server.hpp"

namespace ark {

net_server_service_impl::net_server_service_impl(plugin_manager* p)
    : m_pPluginManager(p) {
    m_pNetServiceManagerModule = m_pPluginManager->find_module<net_service_manager_module>();
    m_pMsgModule = m_pPluginManager->find_module<msg_module>();
}

//AFCNetServerService::~AFCNetServerService()
//{
//    ARK_DELETE(m_pNet); // shutdown will be called in destructor
//}

bool net_server_service_impl::start(std::shared_ptr<const msg_header> head, bus_id_t bus_id, const endpoint& ep,
    uint8_t thread_count, uint32_t max_connection, const size_t silent_timeout) {
    bool ret = false;
    if (ep.proto() == proto_type::tcp)
    {
        m_pNet = std::make_shared<tcp_server>(
            this, &net_server_service_impl::on_net_msg, &net_server_service_impl::on_net_event);
        ret = m_pNet->start_server(
            head, bus_id, ep.get_ip(), ep.get_port(), thread_count, max_connection, silent_timeout);

        //AFINetServerService::RegMsgCallback(
        //    AFMsg::E_SS_MSG_ID_SERVER_REPORT, this, &AFCNetServerService::OnClientRegister);
    }
    else if (ep.proto() == proto_type::udp)
    {
        m_pNet = std::make_shared<rudp_server>(
            this, &net_server_service_impl::on_net_msg, &net_server_service_impl::on_net_event);
        ret = m_pNet->start_server(
            head, bus_id, ep.get_ip(), ep.get_port(), thread_count, max_connection, silent_timeout);
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

bool net_server_service_impl::update() {
    ARK_ASSERT_RET_VAL(m_pNet != nullptr, false);
    m_pNet->update();

    //ProcessConnection();
    process_client_connection();
    return true;
}

std::shared_ptr<net> net_server_service_impl::get_net() {
    return m_pNet;
}

bool net_server_service_impl::reg_msg_callback(msg_id_t msg_id, net_msg_functor&& cb) {
    if (net_msg_callbacks_.find(msg_id) != net_msg_callbacks_.end())
    {
        return false;
    }
    else
    {
        net_msg_callbacks_.insert(std::make_pair(msg_id, std::forward<net_msg_functor>(cb)));
        return true;
    }
}

bool net_server_service_impl::reg_forward_msg_callback(net_msg_functor&& cb) {
    net_forward_msg_callbacks_.push_back(std::forward<net_msg_functor>(cb));
    return true;
}

bool net_server_service_impl::reg_net_event_callback(net_event_functor&& cb) {
    net_event_callbacks_.push_back(std::forward<net_event_functor>(cb));
    return true;
}

bool net_server_service_impl::reg_reg_server_call_back(net_msg_session_functor&& cb) {
    reg_server_call_back_ = std::forward<net_msg_session_functor>(cb);
    return true;
}

void net_server_service_impl::set_timeout_interval(const int32_t interval) {
    if (interval <= 0)
    {
        return;
    }

    client_timeout_interval_ = interval;
}

void net_server_service_impl::on_net_msg(const net_msg* msg, conv_id_t session_id) {
    ARK_LOG_DEBUG("[msg][recv] msg id = {} session id = {}", msg->get_msg_id(), session_id);

    auto msg_id = msg->get_msg_id();
    if (msg_id == AFMsg::E_SS_MSG_ID_REG_TO_SERVER)
    {
        reg_server_call_back_(msg, session_id);

        m_pNetServiceManagerModule->remove_net_connection_bus(msg->get_src_bus());
        m_pNetServiceManagerModule->add_net_connection_bus(msg->get_src_bus(), get_net());

        remove_connection(msg->get_src_bus());
        add_connection(msg->get_src_bus(), session_id);
        return;
    }
    else if (msg_id == AFMsg::E_SS_MSG_ID_HEART_BEAT)
    {
        update_connection(msg->get_src_bus());
        return;
    }

    if (msg->get_src_bus() == 0)
    {
        update_client_connection(session_id);
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

void net_server_service_impl::on_net_event(const net_event* event) {
    switch (event->get_type())
    {
    case net_event_type::CONNECT:
        ARK_LOG_INFO("Connected server = {} succenssfully, ip = {}, session_id = {}",
            bus_addr(event->get_bus_id()).to_string(), event->get_ip(), event->get_id());
        // TODO: NickYang, add bus relation of AFINet
        break;
    case net_event_type::DISCONNECT:
        ARK_LOG_INFO("Disconnected server = {} succenssfully, ip = {}, session_id = {}",
            bus_addr(event->get_bus_id()).to_string(), event->get_ip(), event->get_id());
        //m_pNetServiceManagerModule->RemoveSessionID(event->GetId());
        //m_pNetServiceManagerModule->RemoveNetConnectionBus(event->GetBusId());
        remove_client_connection(event->get_id());
        break;
    default:
        break;
    }

    for (const auto& it : net_event_callbacks_)
    {
        (it)(event);
    }
}

void net_server_service_impl::on_disconnect_net_event(const net_event* event) {
    ARK_LOG_INFO("TimeOut Disconnected server = {} succenssfully, ip = {}, session_id = {}",
        bus_addr(event->get_bus_id()).to_string(), event->get_ip(), event->get_id());

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

void net_server_service_impl::process_client_connection() {
    std::list<conv_id_t> close_list;

    ///check connection
    auto now = date_time::get_now_time();
    for (auto& iter : client_connection_list_)
    {
        auto& connection = iter.second;
        if (connection.last_active_time_ + client_timeout_interval_ < now)
        {
            /// close session
            close_list.push_back(iter.first);
            m_pNet->close_session(iter.first);

            net_event event;
            event.set_type(net_event_type::DISCONNECT);
            event.set_bus_id(connection.bus_id_);
            event.set_id(connection.session_id_);
            on_disconnect_net_event(&event);
        }
    }

    for (auto iter : close_list)
    {
        remove_client_connection(iter);
    }
}

void net_server_service_impl::add_connection(bus_id_t bus_id, conv_id_t session_id) {
    //if (connection_list_.find_value(bus_id))
    //{
    //    return;
    //}
    remove_connection(bus_id);

    auto connection = std::make_shared<ark::connection>(
        bus_id, session_id, date_time::get_now_time(), ark::connection::e_connect_state::CONNECTED);
    connection_list_.insert(bus_id, connection);
}

void net_server_service_impl::update_connection(bus_id_t bus_id) {
    auto connection = connection_list_.find_value(bus_id);
    if (!connection)
    {
        return;
    }

    auto now = date_time::get_now_time();
    connection->last_active_time_ = now;
}

void net_server_service_impl::remove_connection(bus_id_t bus_id) {
    connection_list_.erase(bus_id);
}

void net_server_service_impl::update_client_connection(conv_id_t session_id) {
    auto iter = client_connection_list_.find(session_id);
    if (iter == client_connection_list_.end())
    {
        auto connection =
            ark::connection(0, session_id, date_time::get_now_time(), ark::connection::e_connect_state::CONNECTED);
        client_connection_list_.insert(std::make_pair(session_id, connection));
    }
    else
    {
        iter->second.last_active_time_ = date_time::get_now_time();
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
