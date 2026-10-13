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
#include <vector>

#include "base/AFLogger.hpp"
#include "net/include/AFCRDPClient.hpp"

namespace ark {

AFCRDPClient::~AFCRDPClient()
{
    Shutdown();
}

bool AFCRDPClient::StartClient(
    std::shared_ptr<const AFIMsgHeader> head, bus_id_t dst_bus_id, const std::string& host, uint16_t port)
{
    if (!head)
    {
        return false;
    }
    Shutdown();
    client_ = std::make_shared<arknet::rudp_client>(65536, ARK_TCP_RECV_BUFFER_SIZE);
    this->dst_bus_id_ = dst_bus_id;
    arknet::arq::options transport_options;
    transport_options.mtu = 1400;
    transport_options.congestion_control = false;
    client_->set_rudp_options(transport_options);
    client_->set_auto_reconnect(true, ARK_RECONNECT_TIME);
    client_->set_connect_timeout(ARK_CONNECT_TIMEOUT);
    auto weak = weak_from_this();
    client_
        ->bind_connect([weak, head] {
            const auto ec = arknet::get_last_error();
            auto self = weak.lock();
            if (!self)
            {
                return;
            }
            if (ec)
            {
                ARK_LOG_INFO("Client(RUDP) connect server failed, error={} errno={}, server_addr={} "
                             "server_port={},local_addr={} local_port={}",
                    ec.message(), ec.value(), self->client_->remote_address(), self->client_->remote_port(),
                    self->client_->local_address(), self->client_->local_port());
                return;
            }
            else
            {
                ARK_LOG_INFO(
                    "Client(RUDP) connect server success, server_addr={} server_port={} local_addr={} local_port={}",
                    self->client_->remote_address(), self->client_->remote_port(), self->client_->local_address(),
                    self->client_->local_port());
            }

            auto cur_session_id = self->trust_session_id_++;
            self->client_->set_user_data(cur_session_id);

            AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
            net_connect_event->SetId(cur_session_id);
            net_connect_event->SetType(AFNetEventType::CONNECT);
            net_connect_event->SetBusId(self->dst_bus_id_);
            net_connect_event->SetIP(self->client_->remote_address());
            {
                AFScopeWLock guard(self->rw_lock_);
                self->session_ = std::make_shared<AFRDPClientSession>(head, cur_session_id, nullptr);
                self->sessions_.push_back(self->session_);
                if (!self->session_->AddNetEvent(net_connect_event))
                {
                    AFNetEvent::Release(net_connect_event);
                }
            }
        })
        .bind_disconnect([weak] {
            const auto ec = arknet::get_last_error();
            auto self = weak.lock();
            if (!self)
            {
                return;
            }
            ARK_LOG_INFO("Client(RUDP) disconnect server, error={} errno={} remote_addr={} remote_port={} "
                         "local_addr={} local_port={}",
                ec.message(), ec.value(), self->client_->remote_address(), self->client_->remote_port(),
                self->client_->local_address(), self->client_->local_port());

            auto session_id = self->client_->get_user_data<conv_id_t>();

            AFScopeWLock guard(self->rw_lock_);
            if (!self->session_ || self->session_->GetSessionId() != session_id || self->session_->NeedRemove())
            {
                return;
            }
            AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
            net_disconnect_event->SetId(session_id);
            net_disconnect_event->SetType(AFNetEventType::DISCONNECT);
            net_disconnect_event->SetBusId(self->dst_bus_id_);
            net_disconnect_event->SetIP(self->client_->remote_address());
            if (!self->session_->AddNetEvent(net_disconnect_event))
            {
                AFNetEvent::Release(net_disconnect_event);
            }
            self->session_->NeedRemove(true);
        })
        .bind_recv([weak](std::string_view s) {
            auto self = weak.lock();
            if (!self)
            {
                return;
            }
            AFRDPClientSessionPtr session;
            {
                AFScopeRLock guard(self->rw_lock_);
                session = self->session_;
            }
            if (session && !session->NeedRemove())
            {
                session->AddBuffer(s.data(), s.length());
                if (!session->ParseBufferToMsg())
                {
                    self->client_->stop();
                }
            }
        });

    bool ret = client_->start(host, port);
    SetWorking(ret);
    return ret;
}

void AFCRDPClient::Update()
{
    std::vector<AFRDPClientSessionPtr> sessions;
    {
        AFScopeRLock guard(rw_lock_);
        sessions.assign(sessions_.begin(), sessions_.end());
    }
    for (const auto& session : sessions)
    {
        const auto disconnected = session->NeedRemove();
        UpdateNetEvent(session);
        const auto drained = UpdateNetMsg(session);
        if (disconnected && drained)
        {
            AFScopeWLock guard(rw_lock_);
            sessions_.erase(std::remove(sessions_.begin(), sessions_.end(), session), sessions_.end());
            if (session_ == session)
            {
                session_.reset();
            }
        }
    }
}

void AFCRDPClient::UpdateNetEvent(const AFRDPClientSessionPtr& session)
{
    AFNetEvent* event{nullptr};
    while (session->PopNetEvent(event))
    {
        net_event_cb_(event);
        AFNetEvent::Release(event);
    }
}

bool AFCRDPClient::UpdateNetMsg(const AFRDPClientSessionPtr& session)
{
    AFNetMsg* msg{nullptr};
    int msg_count = 0;
    while (session->PopNetMsg(msg))
    {
        net_msg_cb_(msg, session->GetSessionId());
        AFNetMsg::Release(msg);

        ++msg_count;
        if (msg_count >= ARK_PROCESS_NET_MSG_COUNT_ONCE)
        {
            return false;
        }
    }
    return true;
}

void AFCRDPClient::Shutdown()
{
    if (client_)
    {
        client_->stop();
    }
    SetWorking(false);
}

bool AFCRDPClient::SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id)
{
    if (!head || !msg_data || head->MessageLength() > ARK_MSG_MAX_LENGTH || !client_)
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
    AFScopeRLock guard(rw_lock_);
    const auto session = session_;
    if (!session || session->NeedRemove() || (session_id != 0 && session_id != session->GetSessionId()))
    {
        return false;
    }

    return client_->async_send(std::move(buffer), [weak = std::weak_ptr<arknet::rudp_client>(client_),
                                                      id = session->GetSessionId()](arknet::error_code ec, size_t) {
        if (!ec)
        {
            return;
        }
        ARK_LOG_ERROR("Client(RUDP) send failed, session={} error={} errno={}", id, ec.message(), ec.value());
        if (auto client = weak.lock(); client && client->is_started())
        {
            // Check the session on its executor so an old send cannot stop a reconnected client.
            client->post([weak, id] {
                if (auto current = weak.lock(); current && current->get_user_data<conv_id_t>() == id)
                {
                    current->stop();
                }
            });
        }
    });
}

void AFCRDPClient::CloseSession(conv_id_t session_id)
{
    if (!client_)
    {
        return;
    }
    client_->post([weak = std::weak_ptr<arknet::rudp_client>(client_), session_id] {
        if (auto client = weak.lock(); client && (session_id == 0 || client->get_user_data<conv_id_t>() == session_id))
        {
            client->stop();
        }
    });
}

} // namespace ark
