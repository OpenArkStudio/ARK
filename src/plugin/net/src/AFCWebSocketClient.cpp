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

#include <brynet/net/http/HttpFormat.hpp>
#include "net/include/AFCWebSocketClient.hpp"

namespace ark {

AFCWebSocketClient::~AFCWebSocketClient()
{
    using namespace brynet::net;

    Shutdown();
    base::DestroySocket();
}

bool AFCWebSocketClient::StartClient(
    AFHeadLength len, const int target_busid, const std::string& ip, const int port, bool ip_v6)
{
    using namespace brynet::base;
    using namespace brynet::net;
    using namespace brynet::net::http;

    dst_bus_id_ = target_busid;

    auto shared_this = shared_from_this();
    auto OnEnterCallback = [shared_this, len, ip](const HttpSession::Ptr& httpSession) {
        brynet::net::http::HttpRequest request;
        request.setMethod(HttpRequest::HTTP_METHOD::HTTP_METHOD_GET);
        request.setUrl("/ws");
        request.addHeadValue("Host", ip);
        request.addHeadValue("Upgrade", "websocket");
        request.addHeadValue("Connection", "Upgrade");
        request.addHeadValue("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
        request.addHeadValue("Sec-WebSocket-Version", "13");

        std::string requestStr = request.getResult();
        httpSession->send(requestStr.c_str(), requestStr.size());

        httpSession->setWSConnected([shared_this, &len, &ip](const HttpSession::Ptr& httpSession, const HTTPParser&) {
            // now session_id
            //int64_t cur_session_id = this->trust_session_id_++;
            auto cur_session_id = shared_this->uid_generator_->GetUID(shared_this->dst_bus_id_);
            // create net event
            AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
            net_connect_event->SetId(cur_session_id);
            net_connect_event->SetType(AFNetEventType::CONNECTED);
            net_connect_event->SetBusId(shared_this->dst_bus_id_);
            net_connect_event->SetIP(ip);

            // set session ud
            httpSession->setUD(cur_session_id);

            // create session and net event
            {
                AFScopeWLock guard(shared_this->rw_lock_);

                AFHttpSessionPtr session_ptr = ARK_NEW AFHttpSession(len, cur_session_id, httpSession);
                shared_this->client_session_ptr_.reset(session_ptr);
                session_ptr->AddNetEvent(net_connect_event);
            }
        });

        httpSession->setWSCallback([shared_this](const HttpSession::Ptr& httpSession,
                                       WebSocketFormat::WebSocketFrameType opcode, const std::string& payload) {
            const auto ud = cast<int64_t>(httpSession->getUD());
            int64_t session_id = *ud;

            // Scope lock
            {
                AFScopeRLock xGuard(shared_this->rw_lock_);
                shared_this->client_session_ptr_->AddBuffer(payload.c_str(), payload.size());
                shared_this->client_session_ptr_->ParseBufferToMsg();
            }
        });

        httpSession->setClosedCallback([shared_this, &ip](const HttpSession::Ptr& httpSession) {
            const auto ud = cast<int64_t>(httpSession->getUD());
            int64_t session_id = *ud;

            // create net event
            AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
            net_disconnect_event->SetId(session_id);
            net_disconnect_event->SetType(AFNetEventType::DISCONNECTED);
            net_disconnect_event->SetBusId(shared_this->dst_bus_id_);
            net_disconnect_event->SetIP(ip);
            // Scope lock
            {
                AFScopeWLock xGuard(shared_this->rw_lock_);
                shared_this->client_session_ptr_->AddNetEvent(net_disconnect_event);
                shared_this->client_session_ptr_->SetNeedRemove(true);
            }
        });
    };

    tcp_service_->startWorkerThread(1);
    connector_->startWorkerThread();

    connection_builder.configureService(tcp_service_)
        .configureConnector(connector_)
        .configureConnectionOptions({AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE)})
        .configureConnectOptions({ConnectOption::WithAddr(ip, port), ConnectOption::WithTimeout(ARK_CONNECT_TIMEOUT),
            ConnectOption::AddProcessTcpSocketCallback([](TcpSocket& socket) { socket.setNodelay(); })})
        .configureEnterCallback(OnEnterCallback)
        .asyncConnect();

    return true;
}

bool AFCWebSocketClient::Shutdown()
{
    CloseSession();
    SetWorking(false);

    tcp_service_->stopWorkerThread();
    connector_->stopWorkerThread();

    return true;
}

void AFCWebSocketClient::Update()
{
    UpdateNetSession();
}

void AFCWebSocketClient::UpdateNetSession()
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
        CloseSession();
        client_session_ptr_.release();
    }
}

void AFCWebSocketClient::UpdateNetEvent(AFHttpSessionPtr session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetEvent* event{nullptr};
    while (session->PopNetEvent(event))
    {
        net_event_cb_(event);
        AFNetEvent::Release(event);
    }
}

void AFCWebSocketClient::UpdateNetMsg(AFHttpSessionPtr session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetMsg* msg{nullptr};
    int msg_count = 0;
    while (session->PopNetMsg(msg))
    {
        net_msg_cb_(msg);
        AFNetMsg::Release(msg);

        ++msg_count;
        if (msg_count > ARK_PROCESS_NET_MSG_COUNT_ONCE)
        {
            break;
        }
    }
}

bool AFCWebSocketClient::SendMsg(const char* msg, const size_t msg_len, const AFGUID& session_id /* = 0*/)
{
    using namespace brynet::net::http;

    auto frame = std::make_shared<std::string>();
    WebSocketFormat::wsFrameBuild(msg, msg_len, *frame, WebSocketFormat::WebSocketFrameType::BINARY_FRAME, true, false);

    if (client_session_ptr_->GetSession() != nullptr)
    {
        client_session_ptr_->GetSession()->send(frame);
    }

    return true;
}

bool AFCWebSocketClient::CloseSession(const AFGUID& session_id)
{
    client_session_ptr_->SetNeedRemove(true);
    return true;
}

bool AFCWebSocketClient::CloseSession()
{
    if (client_session_ptr_ != nullptr)
    {
        client_session_ptr_->GetSession()->postShutdown();
    }

    return true;
}

bool AFCWebSocketClient::SendMsg(AFMsgHead* head, const char* msg_data, const int64_t session_id)
{
    // AFCSMsgHead head;
    // head.set_msg_id(msg_id);
    // head.set_uid(actor_id);
    // head.set_body_length(msg_len);

    // std::string out_data;
    // size_t whole_len = EnCode(head, msg, msg_len, out_data);
    // if (whole_len == msg_len + GetHeadLength())
    //{
    //    return SendMsg(out_data.c_str(), out_data.length(), session_id);
    //}
    // else
    //{
    //    return false;
    //}

    return true;
}

} // namespace ark
