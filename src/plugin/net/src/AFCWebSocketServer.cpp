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

#include <algorithm>
#include <chrono>
#include <utility>
#include <vector>

#include "base/AFLogger.hpp"
#include "net/include/AFCWebSocketServer.hpp"
#include "net/include/AFNetEndpoint.hpp"

namespace ark {

AFCWebSocketServer::~AFCWebSocketServer()
{
    Shutdown();
}

void AFCWebSocketServer::Update()
{
    UpdateNetSession();
}

bool AFCWebSocketServer::StartServer(std::shared_ptr<const AFIMsgHeader> head, bus_id_t bus_id, const std::string& host,
    uint16_t port, uint8_t thread_num, uint32_t max_client, const size_t silent_timeout)
{
    ARK_ASSERT_RET_VAL(head != nullptr, false);
    if (server_ != nullptr)
    {
        Shutdown();
    }
    server_ = detail::MakeNetEndpoint<arknet::ws_server>(
        arknet::detail::tcp_frame_size, ARK_TCP_RECV_BUFFER_SIZE, std::max<size_t>(thread_num, 1));
    this->bus_id_ = bus_id;
    const auto weak = weak_from_this();

    server_
        ->bind_start([weak] {
            const auto ec = arknet::get_last_error();
            const auto self = weak.lock();
            if (self == nullptr)
            {
                return;
            }
            if (ec)
            {
                ARK_LOG_INFO("Server(WS) start failed, error={} errno={}", ec.message(), ec.value());
            }
            else
            {
                ARK_LOG_INFO("Server(WS) start success, listen_addr={} listen_port={}", self->server_->listen_address(),
                    self->server_->listen_port());
            }
        })
        .bind_stop([] {
            const auto ec = arknet::get_last_error();
            ARK_LOG_INFO("Server(WS) stop, error={} errno={}", ec.message(), ec.value());
        })
        .bind_accept([silent_timeout](std::shared_ptr<arknet::ws_session>& session_ptr) {
            session_ptr->set_silence_timeout(std::chrono::milliseconds(silent_timeout));
            session_ptr->set_no_delay(true);
        })
        .bind_connect([weak, head, max_client](std::shared_ptr<arknet::ws_session>& session_ptr) {
            const auto self = weak.lock();
            if (self == nullptr)
            {
                session_ptr->stop();
                return;
            }
            ARK_LOG_INFO("Client(WS) connected, remote_addr={} remote_port={}, local_addr={} local_port={}",
                session_ptr->remote_address(), session_ptr->remote_port(), session_ptr->local_address(),
                session_ptr->local_port());

            session_ptr->ws_stream().binary(true);
            auto cur_session_id = self->trusted_session_id_++;
            bool admitted = false;
            {
                AFScopeWLock guard(self->rw_lock_);
                if (max_client == 0 || self->active_session_count_ < max_client)
                {
                    session_ptr->set_user_data(cur_session_id);
                    AFWSSessionPtr session = std::make_shared<AFWSSession>(head, cur_session_id, session_ptr);
                    admitted = self->AddNetSession(session);
                    if (admitted)
                    {
                        ++self->active_session_count_;
                        AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
                        net_connect_event->SetId(cur_session_id);
                        net_connect_event->SetType(AFNetEventType::CONNECT);
                        net_connect_event->SetBusId(self->bus_id_);
                        net_connect_event->SetIP(session_ptr->remote_address());
                        if (!session->AddNetEvent(net_connect_event))
                        {
                            AFNetEvent::Release(net_connect_event);
                        }
                    }
                }
            }
            if (!admitted)
            {
                session_ptr->stop();
            }
        })
        .bind_disconnect([weak](std::shared_ptr<arknet::ws_session>& session_ptr) {
            const auto ec = arknet::get_last_error();
            const auto self = weak.lock();
            if (self == nullptr)
            {
                return;
            }
            ARK_LOG_INFO("Client(WS) disconnected, client_addr={} client_port={} error={}",
                session_ptr->remote_address(), session_ptr->remote_port(), ec.message());

            auto session_id = session_ptr->get_user_data<conv_id_t>();

            AFScopeWLock guard(self->rw_lock_);
            AFWSSessionPtr session = self->GetNetSession(session_id);
            if (session == nullptr || session->NeedRemove())
            {
                return;
            }

            AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
            net_disconnect_event->SetId(session_id);
            net_disconnect_event->SetType(AFNetEventType::DISCONNECT);
            net_disconnect_event->SetBusId(self->bus_id_);
            net_disconnect_event->SetIP(session_ptr->remote_address());
            session->NeedRemove(true);
            --self->active_session_count_;
            if (!session->AddNetEvent(net_disconnect_event))
            {
                AFNetEvent::Release(net_disconnect_event);
            }
        })
        .bind_upgrade([](std::shared_ptr<arknet::ws_session>& session_ptr) {
            const auto ec = arknet::get_last_error();
            ARK_LOG_INFO("Client(WS) upgrade, client_addr={} client_port={} error={}", session_ptr->remote_address(),
                session_ptr->remote_port(), ec.message());
        })
        .bind_recv([weak](std::shared_ptr<arknet::ws_session>& session_ptr, std::string_view s) {
            const auto self = weak.lock();
            if (self == nullptr)
            {
                return;
            }
            auto session_id = session_ptr->get_user_data<conv_id_t>();

            // process msg data scope
            bool parsed = false;
            {
                AFScopeRLock guard(self->rw_lock_);
                const AFWSSessionPtr session = self->GetNetSession(session_id);
                if (session == nullptr || session->NeedRemove())
                {
                    return;
                }

                session->AddBuffer(s.data(), s.length());
                parsed = session->ParseBufferToMsg();
            }
            if (!parsed)
            {
                session_ptr->stop();
            }
        });

    bool ret = server_->start(host, port);
    SetWorking(ret);
    return ret;
}

void AFCWebSocketServer::UpdateNetSession()
{
    std::vector<AFWSSessionPtr> sessions;
    {
        AFScopeRLock guard(rw_lock_);
        sessions.reserve(sessions_.size());
        for (const auto& iter : sessions_)
        {
            sessions.push_back(iter.second);
        }
    }
    for (const auto& session : sessions)
    {
        ARK_ASSERT_CONTINUE(session != nullptr);
        const bool disconnected = UpdateNetEvent(session);
        UpdateNetMsg(session);
        if (disconnected)
        {
            AFScopeWLock guard(rw_lock_);
            CloseSession(session);
        }
    }
}

bool AFCWebSocketServer::UpdateNetEvent(const AFWSSessionPtr& session)
{
    ARK_ASSERT_RET_VAL(session != nullptr, false);

    bool disconnected = false;
    AFNetEvent* event{nullptr};
    while (session->PopNetEvent(event))
    {
        disconnected = disconnected || event->GetType() == AFNetEventType::DISCONNECT;
        net_event_cb_(event);
        AFNetEvent::Release(event);
    }
    return disconnected;
}

void AFCWebSocketServer::UpdateNetMsg(const AFWSSessionPtr& session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetMsg* msg{nullptr};
    int msg_count = 0;
    while (session->PopNetMsg(msg))
    {
        net_msg_cb_(msg, session->GetSessionId());
        AFNetMsg::Release(msg);

        ++msg_count;
        if (msg_count >= ARK_PROCESS_NET_MSG_COUNT_ONCE && !session->NeedRemove())
        {
            break;
        }
    }
}

void AFCWebSocketServer::Shutdown()
{
    if (server_ != nullptr)
    {
        server_->stop();
    }
    SetWorking(false);
}

bool AFCWebSocketServer::AddNetSession(const AFWSSessionPtr& session)
{
    return sessions_.insert(std::make_pair(session->GetSessionId(), session)).second;
}

void AFCWebSocketServer::CloseSession(const AFWSSessionPtr& session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    auto session_id = session->GetSessionId();
    sessions_.erase(session_id);
}

void AFCWebSocketServer::CloseSession(conv_id_t session_id)
{
    AFWSSessionPtr session;
    {
        AFScopeRLock guard(rw_lock_);
        session = GetNetSession(session_id);
    }
    if (session != nullptr)
    {
        session->GetSession()->stop();
    }
}

void AFCWebSocketServer::CloseAllSession()
{
    std::vector<AFWSSessionPtr> sessions;
    {
        AFScopeRLock guard(rw_lock_);
        for (const auto& entry : sessions_)
        {
            sessions.push_back(entry.second);
        }
    }
    for (const auto& session : sessions)
    {
        session->GetSession()->stop();
    }
}

AFWSSessionPtr AFCWebSocketServer::GetNetSession(conv_id_t session_id)
{
    auto it = sessions_.find(session_id);
    return (it != sessions_.end() ? it->second : nullptr);
}

bool AFCWebSocketServer::SendMsg(const char* msg, const size_t msg_len, conv_id_t session_id)
{
    AFScopeRLock xGuard(rw_lock_);
    auto session = GetNetSession(session_id);
    if (session == nullptr || session->NeedRemove())
    {
        return false;
    }

    return session->GetSession()->async_send(msg, msg_len);
}

bool AFCWebSocketServer::SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id)
{
    ARK_ASSERT_RET_VAL(head != nullptr && msg_data != nullptr, false);
    if (head->MessageLength() > ARK_MSG_MAX_LENGTH)
    {
        return false;
    }

    AFScopeRLock guard(rw_lock_);
    auto session = GetNetSession(session_id);
    if (session == nullptr || session->NeedRemove())
    {
        return false;
    }

    std::string buffer;
    head->SerializeToString(buffer);
    if (buffer.empty())
    {
        return false;
    }

    buffer.append(msg_data, head->MessageLength());
    return session->GetSession()->async_send(std::move(buffer));
}

void AFCWebSocketServer::SendMsgToAllClient(const char* msg, size_t msg_len)
{
    AFScopeRLock guard(rw_lock_);
    for (const auto& it : sessions_)
    {
        auto& session = it.second;
        if (session != nullptr && !session->NeedRemove())
        {
            session->GetSession()->async_send(msg, msg_len);
        }
    }
}

void AFCWebSocketServer::BroadcastMsg(AFIMsgHeader* head, const char* msg_data)
{
    ARK_ASSERT_RET_NONE(head != nullptr && msg_data != nullptr);
    if (head->MessageLength() > ARK_MSG_MAX_LENGTH)
    {
        return;
    }

    std::string buffer;
    head->SerializeToString(buffer);
    if (buffer.empty())
    {
        return;
    }
    buffer.append(msg_data, head->MessageLength());

    SendMsgToAllClient(buffer.data(), buffer.size());
}

} // namespace ark
