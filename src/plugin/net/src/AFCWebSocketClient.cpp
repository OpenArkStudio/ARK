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

#include <utility>

#include "base/AFLogger.hpp"
#include "net/include/AFCWebSocketClient.hpp"

namespace ark {

AFCWebSocketClient::~AFCWebSocketClient()
{
    Shutdown();
}

bool AFCWebSocketClient::StartClient(
    std::shared_ptr<const AFIMsgHeader> head, bus_id_t dst_bus_id, const std::string& host, uint16_t port)
{
    ARK_ASSERT_RET_VAL(head != nullptr, false);
    if (client_ != nullptr)
    {
        Shutdown();
    }
    client_ = std::make_shared<arknet::ws_client>(arknet::detail::tcp_frame_size, ARK_TCP_RECV_BUFFER_SIZE);
    this->dst_bus_id_ = dst_bus_id;
    const auto weak = weak_from_this();

    client_->set_auto_reconnect(true, ARK_RECONNECT_TIME);
    client_->set_connect_timeout(ARK_CONNECT_TIMEOUT);
    client_
        ->bind_init([weak] {
            if (auto self = weak.lock())
            {
                self->client_->set_no_delay(true);
            }
        })
        .bind_connect([weak, head] {
            const auto ec = arknet::get_last_error();
            const auto self = weak.lock();
            if (self == nullptr)
            {
                return;
            }
            if (ec)
            {
                ARK_LOG_INFO("Client(WS) connect server failed, error={} errno={}, server_addr={} "
                             "server_port={},local_addr={} local_port={}",
                    ec.message(), ec.value(), self->client_->remote_address(), self->client_->remote_port(),
                    self->client_->local_address(), self->client_->local_port());
                return;
            }
            else
            {
                ARK_LOG_INFO(
                    "Client(WS) connect server success, server_addr={} server_port={} local_addr={} local_port={}",
                    self->client_->remote_address(), self->client_->remote_port(), self->client_->local_address(),
                    self->client_->local_port());
            }

            // now session_id
            auto cur_session_id = self->trust_session_id_++;
            // set session ud
            self->client_->set_user_data(cur_session_id);
            self->client_->ws_stream().binary(true);

            // create net event
            AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
            net_connect_event->SetId(cur_session_id);
            net_connect_event->SetType(AFNetEventType::CONNECT);
            net_connect_event->SetBusId(self->dst_bus_id_);
            net_connect_event->SetIP(self->client_->remote_address());
            // TODO: will add port

            // net event & session scope
            {
                AFScopeWLock guard(self->rw_lock_);
                if (self->client_session_ptr_ != nullptr)
                {
                    self->retired_sessions_.push_back(self->client_session_ptr_);
                }
                self->client_session_ptr_ = std::make_shared<AFWSClientSession>(head, cur_session_id, nullptr);
                if (!self->client_session_ptr_->AddNetEvent(net_connect_event))
                {
                    AFNetEvent::Release(net_connect_event);
                }
            }
        })
        .bind_disconnect([weak] {
            const auto ec = arknet::get_last_error();
            const auto self = weak.lock();
            if (self == nullptr)
            {
                return;
            }
            ARK_LOG_INFO("Client(WS) disconnect server, error={} errno={} remote_addr={} remote_port={} local_addr={} "
                         "local_port={}",
                ec.message(), ec.value(), self->client_->remote_address(), self->client_->remote_port(),
                self->client_->local_address(), self->client_->local_port());

            auto session_id = self->client_->get_user_data<conv_id_t>();

            AFScopeWLock guard(self->rw_lock_);
            if (self->client_session_ptr_ == nullptr || self->client_session_ptr_->GetSessionId() != session_id ||
                self->client_session_ptr_->NeedRemove())
            {
                return;
            }

            // create net event
            AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
            net_disconnect_event->SetId(session_id);
            net_disconnect_event->SetType(AFNetEventType::DISCONNECT);
            net_disconnect_event->SetBusId(self->dst_bus_id_);
            net_disconnect_event->SetIP(self->client_->remote_address());
            // TODO: will add port

            self->client_session_ptr_->NeedRemove(true);
            if (!self->client_session_ptr_->AddNetEvent(net_disconnect_event))
            {
                AFNetEvent::Release(net_disconnect_event);
            }
        })
        .bind_recv([weak](std::string_view s) {
            const auto self = weak.lock();
            if (self == nullptr)
            {
                return;
            }
            bool parsed = false;
            {
                AFScopeRLock guard(self->rw_lock_);
                if (self->client_session_ptr_ == nullptr || self->client_session_ptr_->NeedRemove())
                {
                    return;
                }
                self->client_session_ptr_->AddBuffer(s.data(), s.length());
                parsed = self->client_session_ptr_->ParseBufferToMsg();
            }
            if (!parsed)
            {
                self->client_->stop();
            }
        });

    bool ret = client_->start(host, port);

    SetWorking(ret);
    return ret;
}

void AFCWebSocketClient::Shutdown()
{
    if (client_ != nullptr)
    {
        client_->stop();
    }
    SetWorking(false);
}

void AFCWebSocketClient::Update()
{
    std::vector<AFWSClientSessionPtr> sessions;
    {
        AFScopeWLock guard(rw_lock_);
        sessions.swap(retired_sessions_);
        if (client_session_ptr_ != nullptr)
        {
            sessions.push_back(client_session_ptr_);
        }
    }
    for (const auto& session : sessions)
    {
        UpdateNetEvent(session);
        UpdateNetMsg(session);
    }
}

void AFCWebSocketClient::UpdateNetEvent(const AFWSClientSessionPtr& session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetEvent* event{nullptr};
    while (session->PopNetEvent(event))
    {
        net_event_cb_(event);
        AFNetEvent::Release(event);
    }
}

void AFCWebSocketClient::UpdateNetMsg(const AFWSClientSessionPtr& session)
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

bool AFCWebSocketClient::SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id)
{
    ARK_ASSERT_RET_VAL(head != nullptr && msg_data != nullptr, false);
    // Header serialization can run application code and reconnect the client.
    std::string buffer;
    head->SerializeToString(buffer);
    const auto message_length = head->MessageLength();
    if (buffer.empty() || message_length > ARK_MSG_MAX_LENGTH)
    {
        return false;
    }

    buffer.append(msg_data, message_length);
    AFScopeRLock guard(rw_lock_);
    if (client_ == nullptr || client_session_ptr_ == nullptr || client_session_ptr_->NeedRemove() ||
        (session_id != 0 && client_session_ptr_->GetSessionId() != session_id))
    {
        return false;
    }
    return client_->async_send(std::move(buffer));
}

bool AFCWebSocketClient::SendMsg(const char* msg, size_t msg_len)
{
    return client_ != nullptr && client_->async_send(msg, msg_len);
}

void AFCWebSocketClient::CloseSession(conv_id_t session_id)
{
    std::shared_ptr<arknet::ws_client> client;
    {
        AFScopeRLock guard(rw_lock_);
        if (client_ == nullptr || client_session_ptr_ == nullptr || client_session_ptr_->NeedRemove() ||
            client_session_ptr_->GetSessionId() != session_id)
        {
            return;
        }
        session_id = client_session_ptr_->GetSessionId();
        client = client_;
    }
    client->post([weak = std::weak_ptr(client), session_id] {
        if (const auto current = weak.lock();
            current != nullptr && current->is_started() && current->get_user_data<conv_id_t>() == session_id)
        {
            current->stop();
        }
    });
}

} // namespace ark
