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

#include "base/AFBus.hpp"
#include "net/include/AFCTCPClient.hpp"

namespace ark {

AFCTCPClient::AFCTCPClient(const brynet::net::TcpService::Ptr& service /* = nullptr*/,
    const brynet::net::AsyncConnector::Ptr& connector /* = nullptr*/)
{
    using namespace brynet::net;
    base::InitSocket();

    tcp_service_ = (service != nullptr ? service : TcpService::Create());
    connector_ = (connector != nullptr ? connector : AsyncConnector::Create());
}

AFCTCPClient::~AFCTCPClient()
{
    using namespace brynet::net;

    Shutdown();
    base::DestroySocket();
}

void AFCTCPClient::Update()
{
    UpdateNetSession();
}

bool AFCTCPClient::StartClient(
    AFHeadLength head_len, const int dst_busid, const std::string& ip, const int port, bool ip_v6 /* = false*/)
{
    using namespace brynet::net;

    this->dst_bus_id_ = dst_busid;

    tcp_service_->startWorkerThread(1);
    connector_->startWorkerThread();

    auto OnEnterCallback = [this, head_len](const TcpConnectionPtr& session) {
        // now session_id
        int64_t cur_session_id = this->trust_session_id_++;

        // set session ud
        session->setUD(cur_session_id);

        // create net event
        AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
        net_connect_event->SetId(cur_session_id);
        net_connect_event->SetType(AFNetEventType::CONNECTED);
        net_connect_event->SetBusId(this->dst_bus_id_);
        net_connect_event->SetIP(session->getIP());

        // create session and operate data
        // scope lock
        {
            AFScopeWLock guard(this->rw_lock_);

            AFTCPSessionPtr session_ptr = ARK_NEW AFTCPSession(head_len, cur_session_id, session);
            this->client_session_ptr_.reset(session_ptr);
            session_ptr->AddNetEvent(net_connect_event);
        }

        // data cb
        session->setDataCallback([this, session](const char* buffer, size_t len) {
            const auto ud = cast<int64_t>(session->getUD());
            int64_t session_id = *ud;

            // scope lock
            {
                AFScopeRLock guard(this->rw_lock_);
                this->client_session_ptr_->AddBuffer(buffer, len);
                this->client_session_ptr_->ParseBufferToMsg();
            }

            return len;
        });

        // disconnect cb
        session->setDisConnectCallback([this](const brynet::net::TcpConnectionPtr& session) {
            const auto ud = cast<int64_t>(session->getUD());
            int64_t session_id = *ud;

            // create net event
            AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
            net_disconnect_event->SetId(session_id);
            net_disconnect_event->SetType(AFNetEventType::DISCONNECTED);
            net_disconnect_event->SetBusId(this->dst_bus_id_);
            net_disconnect_event->SetIP(session->getIP());

            // scope lock
            {
                AFScopeWLock guard(this->rw_lock_);
                this->client_session_ptr_->AddNetEvent(net_disconnect_event);
                this->client_session_ptr_->SetNeedRemove(true);
            }
        });
    };

    auto failedCallback = []() { CONSOLE_ERROR_LOG << "connect failed" << std::endl; };

    connection_builder_.configureService(tcp_service_)
        .configureConnector(connector_)
        .configureConnectionOptions({TcpService::AddSocketOption::AddEnterCallback(OnEnterCallback),
            TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_TCP_RECV_BUFFER_SIZE)})
        .configureConnectOptions({AsyncConnector::ConnectOptions::WithAddr(ip, port),
            AsyncConnector::ConnectOptions::WithTimeout(ARK_CONNECT_TIMEOUT),
            AsyncConnector::ConnectOptions::WithFailedCallback(failedCallback),
            AsyncConnector::ConnectOptions::AddProcessTcpSocketCallback(
                [](TcpSocket& socket) { socket.setNodelay(); })})
        .asyncConnect();

    SetWorking(true);
    return true;
}

bool AFCTCPClient::Shutdown()
{
    CloseAllSession();
    SetWorking(false);
    return true;
}

bool AFCTCPClient::CloseAllSession()
{
    if (client_session_ptr_ != nullptr)
    {
        client_session_ptr_->GetSession()->postShutdown();
    }

    return true;
}

bool AFCTCPClient::SendMsg(const char* msg, const size_t msg_len, const AFGUID& session_id /* = 0*/)
{
    if (nullptr != client_session_ptr_ && client_session_ptr_->GetSession())
    {
        client_session_ptr_->GetSession()->send(msg, msg_len);
    }

    return true;
}

bool AFCTCPClient::CloseSession(const AFGUID& session_id)
{
    client_session_ptr_->GetSession()->postDisConnect();
    return true;
}

void AFCTCPClient::UpdateNetSession()
{
    if (client_session_ptr_ == nullptr)
    {
        return;
    }

    {
        AFScopeRLock guard(rw_lock_);
        UpdateNetEvent(client_session_ptr_.get());
        UpdateNetMsg(client_session_ptr_.get());
    }

    if (client_session_ptr_->NeedRemove())
    {
        AFScopeWLock guard(rw_lock_);
        CloseSession(client_session_ptr_->GetSessionId());
        client_session_ptr_.release();
    }
}

void AFCTCPClient::UpdateNetEvent(AFTCPSessionPtr session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetEvent* event(nullptr);
    while (session->PopNetEvent(event))
    {
        net_event_cb_(event);
        AFNetEvent::Release(event);
    }
}

void AFCTCPClient::UpdateNetMsg(AFTCPSessionPtr session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetMsg* msg(nullptr);
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

bool AFCTCPClient::SendMsg(AFMsgHead* head, const char* msg_data, const int64_t session_id)
{
    ARK_ASSERT_RET_VAL(head != nullptr && msg_data != nullptr, false);
    ARK_ASSERT_RET_VAL(client_session_ptr_ != nullptr, false);

    AFHeadLength head_length = static_cast<AFHeadLength>(client_session_ptr_->GetHeadLen());
    ARK_ASSERT_RET_VAL(
        (head_length == AFHeadLength::CS_HEAD_LENGTH) || (head_length == AFHeadLength::SS_HEAD_LENGTH), false);

    std::string buffer;
    buffer.append(reinterpret_cast<char*>(head), client_session_ptr_->GetHeadLen());

    if (buffer.empty())
    {
        return false;
    }

    buffer.append(msg_data, head->length_);
    client_session_ptr_->GetSession()->send(buffer.c_str(), buffer.length());
    return true;
}

} // namespace ark
