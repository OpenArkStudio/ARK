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
#include "base/AFBus.hpp"
#include "net/include/AFCTCPClient.hpp"

namespace ark {

AFCTCPClient::~AFCTCPClient()
{
    Shutdown();
}

void AFCTCPClient::Update()
{
    UpdateNetSession();
}

bool AFCTCPClient::StartClient(std::shared_ptr<const AFIMsgHeader> head, bus_id_t dst_bus_id, const std::string& host,
    uint16_t port)
{
    using namespace zephyr;
    using namespace zephyr::detail;

    client_ = std::make_shared<zephyr::tcp_client>(tcp_frame_size, ARK_TCP_RECV_BUFFER_SIZE);
    this->dst_bus_id_ = dst_bus_id;

    client_->auto_reconnect(true, ARK_RECONNECT_TIME);
    client_->no_delay(true);
    client_
        ->bind_connect([self = shared_from_this(), head](asio::error_code ec) {
            if (ec)
            {
                ARK_LOG_INFO("Client(TCP) connect server failed, error={} errno={}, server_addr={} server_port={} "
                             "local_addr={} local_port={}",
                    ec.message(), ec.value(), self->client_->remote_address(), self->client_->remote_port(),
                    self->client_->local_address(), self->client_->local_port());
                return;
            }
            else
            {
                ARK_LOG_INFO("Client(TCP) connect success, server_addr={} server_port={} local_addr={} local_port={}",
                    self->client_->remote_address(), self->client_->remote_port(), self->client_->local_address(),
                    self->client_->local_port());
            }

            // now session_id
            auto cur_session_id = self->trust_session_id_++;
            // set session ud
            self->client_->user_data(cur_session_id);

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
                self->session_ = std::make_shared<AFTCPClientSession>(head, cur_session_id, nullptr);
                self->session_->AddNetEvent(net_connect_event);
            }
        })
        .bind_disconnect([self = shared_from_this()](asio::error_code ec) {
            ARK_LOG_INFO("Disconnect server(TCP), error={} errno={} local_addr={} local_port={}", ec.message(),
                ec.value(), self->client_->remote_address(), self->client_->remote_port(),
                self->client_->local_address(), self->client_->local_port());

            auto session_id = self->client_->user_data<conv_id_t>();
            // TODO: checking session is same.

            // create net event
            AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
            net_disconnect_event->SetId(session_id);
            net_disconnect_event->SetType(AFNetEventType::DISCONNECT);
            net_disconnect_event->SetBusId(self->dst_bus_id_);
            net_disconnect_event->SetIP(self->client_->remote_address());

            // scope lock
            {
                AFScopeWLock guard(self->rw_lock_);
                self->session_->AddNetEvent(net_disconnect_event);
                self->session_->NeedRemove(true);
            }
        })
        .bind_recv([self = shared_from_this()](std::string_view s) {
            AFScopeRLock guard(self->rw_lock_);
            self->session_->AddBuffer(s.data(), s.length());
            self->session_->ParseBufferToMsg();
        });

    bool ret = client_->start(host, port);

    SetWorking(ret);
    return ret;
}

void AFCTCPClient::Shutdown()
{
    client_->stop();
    SetWorking(false);
}

void AFCTCPClient::UpdateNetSession()
{
    if (session_ == nullptr)
    {
        return;
    }

    AFScopeRLock guard(rw_lock_);
    UpdateNetEvent();
    UpdateNetMsg();
}

void AFCTCPClient::UpdateNetEvent()
{
    ARK_ASSERT_RET_NONE(session_ != nullptr);

    AFNetEvent* event(nullptr);
    while (session_->PopNetEvent(event))
    {
        net_event_cb_(event);
        AFNetEvent::Release(event);
    }
}

void AFCTCPClient::UpdateNetMsg()
{
    ARK_ASSERT_RET_NONE(session_ != nullptr);

    AFNetMsg* msg(nullptr);
    int msg_count = 0;
    while (session_->PopNetMsg(msg))
    {
        net_msg_cb_(msg, session_->GetSessionId());
        AFNetMsg::Release(msg);

        ++msg_count;
        if (msg_count > ARK_PROCESS_NET_MSG_COUNT_ONCE)
        {
            break;
        }
    }
}

bool AFCTCPClient::SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id)
{
    std::ignore = session_id;

    ARK_ASSERT_RET_VAL(head != nullptr && msg_data != nullptr, false);
    ARK_ASSERT_RET_VAL(session_ != nullptr, false);

    std::string buffer;
    head->SerializeToString(buffer);
    if (buffer.empty())
    {
        return false;
    }

    buffer.append(msg_data, head->MessageLength());
    return client_->send(buffer.c_str(), buffer.length());
}

bool AFCTCPClient::SendMsg(const char* msg, size_t msg_len)
{
    return client_->send(msg, msg_len);
}

void AFCTCPClient::CloseSession(conv_id_t session_id)
{
    std::ignore = session_id;
}

} // namespace ark
