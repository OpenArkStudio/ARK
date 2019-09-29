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

#include "net/include/AFCWebSocketServer.hpp"

namespace ark {

AFCWebSocketServer::AFCWebSocketServer()
{
    brynet::net::base::InitSocket();
}

AFCWebSocketServer::~AFCWebSocketServer()
{
    Shutdown();
    brynet::net::base::DestroySocket();
}

void AFCWebSocketServer::Update()
{
    UpdateNetSession();
}

bool AFCWebSocketServer::StartServer(AFHeadLength head_len, const int busid, const std::string& ip, const int port,
    const int thread_num, const unsigned int max_client, bool ip_v6 /* = false*/)
{
    using namespace brynet::net;
    using namespace brynet::net::http;

    this->bus_id_ = busid;

    tcp_service_ptr_ = TcpService::Create();
    tcp_service_ptr_->startWorkerThread(thread_num);

    auto OnEnterCallback = [this, &head_len](const TcpConnectionPtr& session) {
        const std::string& session_ip = session->getIP();
        HttpService::setup(session, [this, &head_len, &session_ip](const HttpSession::Ptr& httpSession) {
            int64_t cur_session_id = this->trusted_session_id_++;
            AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
            net_connect_event->SetId(cur_session_id);
            net_connect_event->SetType(AFNetEventType::CONNECTED);
            net_connect_event->SetBusId(this->bus_id_);
            net_connect_event->SetIP(session_ip);

            // Scope lock
            {
                AFScopeWLock guard(this->rw_lock_);
                AFHttpSessionPtr session_ptr = ARK_NEW AFHttpSession(head_len, cur_session_id, httpSession);
                if (this->AddNetSession(session_ptr))
                {
                    session_ptr->AddNetEvent(net_connect_event);
                }
            }

            httpSession->setWSCallback([this](const HttpSession::Ptr& httpSession,
                                           WebSocketFormat::WebSocketFrameType opcode, const std::string& payload) {
                switch (opcode)
                {
                    case WebSocketFormat::WebSocketFrameType::ERROR_FRAME:
                    case WebSocketFormat::WebSocketFrameType::CONTINUATION_FRAME:
                    case WebSocketFormat::WebSocketFrameType::TEXT_FRAME:
                    case WebSocketFormat::WebSocketFrameType::BINARY_FRAME:
                    case WebSocketFormat::WebSocketFrameType::CLOSE_FRAME:
                    case WebSocketFormat::WebSocketFrameType::PING_FRAME:
                    {
                        auto frame = std::make_shared<std::string>();
                        WebSocketFormat::wsFrameBuild(payload.c_str(), payload.size(), *frame,
                            WebSocketFormat::WebSocketFrameType::PONG_FRAME, true, false);
                        httpSession->send(frame);
                        return;
                    }
                    break;
                    case WebSocketFormat::WebSocketFrameType::PONG_FRAME:
                        break;
                    default:
                        break;
                }

                const auto ud = cast<int64_t>(httpSession->getUD());
                int64_t session_id = *ud;

                // Scope lock
                {
                    AFScopeRLock guard(this->rw_lock_);
                    auto session_ptr = this->GetNetSession(session_id);
                    if (session_ptr == nullptr)
                    {
                        return;
                    }

                    session_ptr->AddBuffer(payload.c_str(), payload.size());
                    session_ptr->ParseBufferToMsg();
                }
            });

            httpSession->setHttpCallback([](const HTTPParser& httpParser, const HttpSession::Ptr& session) {
                HttpResponse response;
                response.setBody("<html>Hello ARK.</html>");
                std::string result = response.getResult();
                session->send(result.c_str(), result.size(), [session]() { session->postShutdown(); });
            });

            httpSession->setClosedCallback([this, &session_ip](const HttpSession::Ptr& httpSession) {
                const auto ud = cast<int64_t>(httpSession->getUD());
                int64_t session_id = *ud;

                AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
                net_disconnect_event->SetId(session_id);
                net_disconnect_event->SetType(AFNetEventType::DISCONNECTED);
                net_disconnect_event->SetBusId(this->bus_id_);
                net_disconnect_event->SetIP(session_ip);

                AFScopeWLock guard(this->rw_lock_);
                const AFHttpSessionPtr session_ptr = this->GetNetSession(session_id);
                if (session_ptr == nullptr)
                {
                    return;
                }

                session_ptr->AddNetEvent(net_disconnect_event);
                session_ptr->SetNeedRemove(true);
            });
        });
    };

    listen_builder_.configureService(tcp_service_ptr_)
        .configureSocketOptions({[](TcpSocket& socket) { socket.setNodelay(); }})
        .configureConnectionOptions({TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE),
            TcpService::AddSocketOption::AddEnterCallback(OnEnterCallback)})
        .configureListen([=](wrapper::BuildListenConfig config) { config.setAddr(ip_v6, ip, port); })
        .asyncRun();

    return true;
}

void AFCWebSocketServer::UpdateNetSession()
{
    std::list<AFGUID> remove_sessions;

    {
        AFScopeRLock guard(rw_lock_);
        for (auto& iter : sessions_)
        {
            auto& session = iter.second;
            if (session == nullptr)
            {
                continue;
            }

            UpdateNetEvent(session);
            UpdateNetMsg(session);

            if (!session->NeedRemove())
            {
                continue;
            }

            remove_sessions.push_back(session->GetSessionId());
        }
    }

    for (auto& iter : remove_sessions)
    {
        auto& session_id = iter;
        auto session = GetNetSession(session_id);
        if (session == nullptr)
        {
            continue;
        }

        AFScopeWLock guard(rw_lock_);
        CloseSession(session);
    }

    remove_sessions.clear();
}

void AFCWebSocketServer::UpdateNetEvent(AFHttpSessionPtr session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetEvent* event{nullptr};
    while (session->PopNetEvent(event))
    {
        net_event_cb_(event);
        AFNetEvent::Release(event);
    }
}

void AFCWebSocketServer::UpdateNetMsg(AFHttpSessionPtr session)
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

bool AFCWebSocketServer::Shutdown()
{
    CloseAllSession();
    SetWorking(false);
    return true;
}

bool AFCWebSocketServer::SendMsgToAllClient(const char* msg, const size_t nLen)
{
    // auto frame = std::make_shared<std::string>();
    // brynet::net::WebSocketFormat::wsFrameBuild(msg,
    //        nLen,
    //        *frame,
    //        brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME,
    //        true,
    //        false);

    // for (auto& it : sessions_)
    //{
    //    AFHttpEntityPtr pNetObject = it.second;

    //    if (pNetObject && !pNetObject->NeedRemove())
    //    {
    //        pNetObject->GetSession()->send(frame);
    //    }
    //}

    return true;
}

bool AFCWebSocketServer::SendMsg(const char* msg, const size_t msg_len, const AFGUID& session_id)
{
    // AFScopeRdLock xGuard(rw_lock_);

    // AFHttpEntity* pNetObject = GetNetSession(session_id);

    // if (pNetObject == nullptr)
    //{
    //    return false;
    //}

    // auto frame = std::make_shared<std::string>();
    // brynet::net::WebSocketFormat::wsFrameBuild(msg,
    //        msg_len,
    //        *frame,
    //        brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME,
    //        true,
    //        false);

    // pNetObject->GetSession()->send(frame);
    return true;
}

bool AFCWebSocketServer::AddNetSession(AFHttpSessionPtr session)
{
    return sessions_.insert(std::make_pair(session->GetSessionId(), session)).second;
}

bool AFCWebSocketServer::CloseSession(AFHttpSessionPtr session)
{
    ARK_ASSERT_RET_VAL(session != nullptr, false);

    auto session_id = session->GetSessionId();
    session->GetSession()->postShutdown();
    ARK_DELETE(session);
    sessions_.erase(session_id);
    return true;
}

bool AFCWebSocketServer::CloseSession(const AFGUID& session_id)
{
    auto session = GetNetSession(session_id);
    if (session == nullptr)
    {
        return false;
    }

    session->SetNeedRemove(true);
    return true;
}

bool AFCWebSocketServer::CloseAllSession()
{
    for (auto& iter : sessions_)
    {
        auto& session = iter.second;
        session->GetSession()->postShutdown();
        ARK_DELETE(session);
    }

    sessions_.clear();
    return true;
}

AFHttpSessionPtr AFCWebSocketServer::GetNetSession(const AFGUID& session_id)
{
    auto it = sessions_.find(session_id);
    return (it != sessions_.end() ? it->second : nullptr);
}

bool AFCWebSocketServer::SendMsg(AFMsgHead* head, const char* msg_data, const int64_t session_id)
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

bool AFCWebSocketServer::BroadcastMsg(AFMsgHead* head, const char* msg_data)
{
    // AFCSMsgHead head;
    // head.set_msg_id(msg_id);
    // head.set_uid(actor_id);
    // head.set_body_length(msg_len);

    // std::string out_data;
    // size_t whole_len = EnCode(head, msg, msg_len, out_data);
    // if (whole_len == msg_len + GetHeadLength())
    //{
    //    return SendMsgToAllClient(out_data.c_str(), out_data.length());
    //}
    // else
    //{
    //    return false;
    //}

    return true;
}

} // namespace ark
