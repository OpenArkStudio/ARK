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

#include "base/AFLogger.hpp"
#include "net/include/AFCTCPServer.hpp"

namespace ark {

AFCTCPServer::~AFCTCPServer()
{
    Shutdown();
}

void AFCTCPServer::Update()
{
    UpdateNetSession();
}

bool AFCTCPServer::StartServer(std::shared_ptr<const AFIMsgHeader> head, bus_id_t bus_id, const std::string& host,
    uint16_t port, uint8_t thread_num, uint32_t max_client, const size_t silent_timeout)
{
    using namespace zephyr;
    using namespace zephyr::detail;

    server_ =
        std::make_shared<zephyr::tcp_server>(silent_timeout, tcp_frame_size, ARK_TCP_RECV_BUFFER_SIZE, thread_num);
    this->bus_id_ = bus_id;
    std::ignore = max_client; // will use to limit connection number.

    server_
        ->bind_start([self = shared_from_this()](asio::error_code ec) {
            if (ec)
            {
                ARK_LOG_INFO("Server(TCP) start failed, error={}, errno={}", ec.message(), ec.value());
            }
            else
            {
                ARK_LOG_INFO("Server(TCP) start success, listen_addr={} listen_port={}",
                    self->server_->listen_address(), self->server_->listen_port());
            }
        })
        .bind_stop(
            [](asio::error_code ec) { ARK_LOG_INFO("Server(TCP) stop, error={} errno={}", ec.message(), ec.value()); })
        .bind_connect([self = shared_from_this(), head](std::shared_ptr<zephyr::tcp_session>& session_ptr) {
            ARK_LOG_INFO("Client(TCP) connected, client_addr={} client_port={} local_addr={} local_port={}",
                session_ptr->remote_address(), session_ptr->remote_port(), session_ptr->local_address(),
                session_ptr->local_port());

            session_ptr->no_delay(true);
            auto cur_session_id = self->trusted_session_id_++;
            session_ptr->user_data(cur_session_id);

            AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
            net_connect_event->SetId(cur_session_id);
            net_connect_event->SetType(AFNetEventType::CONNECT);
            net_connect_event->SetBusId(self->bus_id_);
            net_connect_event->SetIP(session_ptr->remote_address());
            // TODO: will add port

            // add session scope
            {
                AFScopeWLock guard(self->rw_lock_);
                AFTCPSessionPtr session = std::make_shared<AFTCPSession>(head, cur_session_id, session_ptr);
                if (self->AddNetSession(session))
                {
                    session->AddNetEvent(net_connect_event);
                }
            }
        })
        .bind_disconnect([self = shared_from_this()](std::shared_ptr<zephyr::tcp_session>& session_ptr) {
            ARK_LOG_INFO("Client(TCP) disconnected, client_addr={} client_port={} error={}, errno={}",
                session_ptr->remote_address(), session_ptr->remote_port(), zephyr::last_error_msg(),
                zephyr::last_error_val());

            auto session_id = session_ptr->user_data<conv_id_t>();

            AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
            net_disconnect_event->SetId(session_id);
            net_disconnect_event->SetType(AFNetEventType::DISCONNECT);
            net_disconnect_event->SetBusId(self->bus_id_);
            net_disconnect_event->SetIP(session_ptr->remote_address());
            // TODO: will add port

            // remove session scope
            {
                AFScopeWLock xGuard(self->rw_lock_);
                AFTCPSessionPtr session = self->GetNetSession(session_id);
                if (session == nullptr)
                {
                    return;
                }

                session->AddNetEvent(net_disconnect_event);
                session->NeedRemove(true);
            }
        })
        .bind_recv([self = shared_from_this()](std::shared_ptr<zephyr::tcp_session>& session_ptr, std::string_view s) {
            auto session_id = session_ptr->user_data<conv_id_t>();

            // process msg data scope
            {
                AFScopeRLock guard(self->rw_lock_);
                const AFTCPSessionPtr session = self->GetNetSession(session_id);
                if (session == nullptr)
                {
                    return size_t(0);
                }

                session->AddBuffer(s.data(), s.length());
                session->ParseBufferToMsg();
            }

            return s.length();
        });

    bool ret = server_->start(host, port);
    SetWorking(ret);
    return ret;
}

void AFCTCPServer::UpdateNetSession()
{
    std::vector<AFTCPSessionPtr> remove_sessions;

    {
        AFScopeRLock guard(rw_lock_);
        for (auto& iter : sessions_)
        {
            auto& session = iter.second;
            ARK_ASSERT_CONTINUE(session != nullptr);

            UpdateNetEvent(session);
            UpdateNetMsg(session);

            if (!session->NeedRemove())
            {
                continue;
            }

            remove_sessions.emplace_back(session);
        }
    }

    for (auto& iter : remove_sessions)
    {
        auto& session = iter;
        if (session == nullptr)
        {
            continue;
        }

        AFScopeWLock guard(rw_lock_);
        CloseSession(session);
    }
}

void AFCTCPServer::UpdateNetEvent(const AFTCPSessionPtr& session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetEvent* event{nullptr};
    while (session->PopNetEvent(event))
    {
        net_event_cb_(event);
        AFNetEvent::Release(event);
    }
}

void AFCTCPServer::UpdateNetMsg(const AFTCPSessionPtr& session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetMsg* msg{nullptr};
    int msg_count = 0;
    while (session->PopNetMsg(msg))
    {
        net_msg_cb_(msg, session->GetSessionId());
        AFNetMsg::Release(msg);

        ++msg_count;
        if (msg_count > ARK_PROCESS_NET_MSG_COUNT_ONCE)
        {
            break;
        }
    }
}

void AFCTCPServer::Shutdown()
{
    CloseAllSession();

    server_->stop();
    SetWorking(false);
}

std::string AFCTCPServer::GetIP(const conv_id_t session_id)
{
    auto session = GetNetSession(session_id);
    if (session == nullptr)
    {
        return NULL_STR;
    }

    return session->GetSession()->remote_address();
}

bool AFCTCPServer::AddNetSession(const AFTCPSessionPtr& session)
{
    return sessions_.insert(std::make_pair(session->GetSessionId(), session)).second;
}

void AFCTCPServer::CloseSession(AFTCPSessionPtr& session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    auto session_id = session->GetSessionId();
    sessions_.erase(session_id);
}

void AFCTCPServer::CloseSession(conv_id_t session_id)
{
    AFTCPSessionPtr session = GetNetSession(session_id);
    if (session == nullptr)
    {
        return;
    }

    session->NeedRemove(true);
}

void AFCTCPServer::CloseAllSession()
{
    sessions_.clear();
}

AFTCPSessionPtr AFCTCPServer::GetNetSession(conv_id_t session_id)
{
    auto iter = sessions_.find(session_id);
    return (iter != sessions_.end() ? iter->second : nullptr);
}

bool AFCTCPServer::SendMsg(const char* msg, size_t msg_len, conv_id_t session_id)
{
    AFScopeRLock xGuard(rw_lock_);
    auto session = GetNetSession(session_id);
    ARK_ASSERT_RET_VAL(session != nullptr, false);

    return session->GetSession()->send(msg, msg_len);
}

void AFCTCPServer::SendMsgToAllClient(const char* msg, size_t msg_len)
{
    AFScopeRLock guard(rw_lock_);
    for (const auto& it : sessions_)
    {
        auto& session = it.second;
        if (session != nullptr && !session->NeedRemove())
        {
            session->GetSession()->send(msg, msg_len);
        }
    }
}

bool AFCTCPServer::SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id)
{
    ARK_ASSERT_RET_VAL(head != nullptr && msg_data != nullptr, false);

    AFScopeRLock guard(rw_lock_);
    auto session = GetNetSession(session_id);
    ARK_ASSERT_RET_VAL(session != nullptr, false);

    std::string buffer;
    head->SerializeToString(buffer);
    if (buffer.empty())
    {
        return false;
    }

    buffer.append(msg_data, head->MessageLength());
    return session->GetSession()->send(buffer.c_str(), buffer.length());
}

void AFCTCPServer::BroadcastMsg(AFIMsgHeader* head, const char* msg_data)
{
    ARK_ASSERT_RET_NONE(head != nullptr && msg_data != nullptr);

    if (sessions_.empty())
    {
        return;
    }

    auto it = sessions_.begin();
    auto first_session = it->second;
    if (first_session == nullptr)
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

    for (const auto& iter : sessions_)
    {
        auto& session = iter.second;
        if (session != nullptr && !session->NeedRemove())
        {
            session->GetSession()->send(buffer.c_str(), buffer.length());
        }
    }
}

} // namespace ark
