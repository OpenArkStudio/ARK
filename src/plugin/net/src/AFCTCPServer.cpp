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

#include "net/include/AFCTCPServer.h"

#if ARK_PLATFORM == PLATFORM_WIN
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")
#elif ARK_PLATFORM == PLATFORM_APPLE
#include <arpa/inet.h>
#endif

namespace ark
{

    AFCTCPServer::AFCTCPServer()
    {
        brynet::net::base::InitSocket();
    }

    AFCTCPServer::~AFCTCPServer()
    {
        CloseAllSession();
        Shutdown();
        brynet::net::base::DestroySocket();
    }

    void AFCTCPServer::Update()
    {
        UpdateNetSession();
    }

    bool AFCTCPServer::StartServer(AFHeadLength head_len, const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6/* = false*/)
    {
        this->bus_id_ = busid;

        tcp_service_ptr_ = brynet::net::TcpService::Create();
        tcp_service_ptr_->startWorkerThread(thread_num);

        auto OnEnterCallback = [this, head_len](const brynet::net::TcpConnectionPtr & session)
        {
            int64_t cur_session_id = this->trusted_session_id_++;

            session->setUD(cur_session_id);

            AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
            net_connect_event->id_ = cur_session_id;
            net_connect_event->type_ = AFNetEventType::CONNECTED;
            net_connect_event->bus_id_ = this->bus_id_;
            net_connect_event->ip_ = session->getIP();

            //scope lock
            {
                AFScopeWLock guard(this->rw_lock_);
                AFTCPSessionPtr session_ptr = ARK_NEW AFTCPSession(head_len, cur_session_id, session);
                if (this->AddNetSession(session_ptr))
                {
                    session_ptr->AddNetEvent(net_connect_event);
                }
            }

            session->setDataCallback([this, session](const char* buffer, size_t len)
            {
                auto pUD = brynet::net::cast<int64_t>(session->getUD());
                if (pUD != nullptr)
                {
                    const AFTCPSessionPtr session_ptr = this->GetNetSession(*pUD);
                    session_ptr->AddBuffer(buffer, len);
                    session_ptr->ParseBufferToMsg();
                }

                return len;
            });

            session->setDisConnectCallback([this](const brynet::net::TcpConnectionPtr & session)
            {
                auto pUD = brynet::net::cast<int64_t>(session->getUD());
                if (pUD == nullptr)
                {
                    return;
                }

                int64_t session_id = *pUD;

                AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
                net_disconnect_event->id_ = session_id;
                net_disconnect_event->type_ = AFNetEventType::DISCONNECTED;
                net_disconnect_event->bus_id_ = this->bus_id_;
                net_disconnect_event->ip_ = session->getIP();

                const AFTCPSessionPtr session_ptr = this->GetNetSession(session_id);
                if (session_ptr == nullptr)
                {
                    return;
                }

                session_ptr->AddNetEvent(net_disconnect_event);
                session_ptr->SetNeedRemove(true);
            });
        };

        //Chain expression
        brynet::net::wrapper::ListenerBuilder listener;
        listener.configureService(tcp_service_ptr_)
        .configureSocketOptions(
        {
            [](brynet::net::TcpSocket & socket)
            {
                socket.setNodelay();
            }
        })
        .configureConnectionOptions(
        {
            brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_TCP_RECV_BUFFER_SIZE),
            brynet::net::TcpService::AddSocketOption::AddEnterCallback(OnEnterCallback)
        })
        .configureListen([ = ](brynet::net::wrapper::BuildListenConfig config)
        {
            config.setAddr(ip_v6, ip, port);
        })
        .asyncRun();


        SetWorking(true);
        return true;
    }

    void AFCTCPServer::UpdateNetSession()
    {
        std::list<int64_t> remove_sessions;

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

                remove_sessions.emplace_back(session->GetSessionId());
            }
        }

        for (auto& iter : remove_sessions)
        {
            auto& session_id = iter;
            AFScopeWLock guard(rw_lock_);
            CloseSession(session_id);
        }
    }

    void AFCTCPServer::UpdateNetEvent(AFTCPSessionPtr session)
    {
        AFNetEvent* event(nullptr);
        if (!session->PopNetEvent(event))
        {
            return;
        }

        while (event != nullptr)
        {
            net_event_cb_(event);
            AFNetEvent::Release(event);

            session->PopNetEvent(event);
        }
    }

    void AFCTCPServer::UpdateNetMsg(AFTCPSessionPtr session)
    {
        AFNetMsg* msg(nullptr);
        if (!session->PopNetMsg(msg))
        {
            return;
        }

        int msg_count = 0;
        while (msg != nullptr)
        {
            net_msg_cb_(msg, session->GetSessionId());
            AFNetMsg::Release(msg);

            ++msg_count;
            if (msg_count > ARK_PROCESS_NET_MSG_COUNT_ONCE)
            {
                break;
            }

            session->PopNetMsg(msg);
        }
    }

    bool AFCTCPServer::Shutdown()
    {
        CloseAllSession();

        listen_thread_ptr_->stopListen();
        tcp_service_ptr_->stopWorkerThread();

        SetWorking(false);
        return true;
    }

    bool AFCTCPServer::SendMsgToAllClient(const char* msg, const size_t msg_len)
    {
        for (auto it : sessions_)
        {
            auto& session = it.second;
            if (session != nullptr && !session->NeedRemove())
            {
                session->GetSession()->send(msg, msg_len);
            }
        }

        return true;
    }

    bool AFCTCPServer::SendMsg(const char* msg, const size_t msg_len, const int64_t& session_id)
    {
        AFScopeRLock xGuard(rw_lock_);
        auto session = GetNetSession(session_id);
        if (session == nullptr)
        {
            return false;
        }
        else
        {
            session->GetSession()->send(msg, msg_len);
            return true;
        }
    }

    bool AFCTCPServer::AddNetSession(AFTCPSessionPtr session)
    {
        return sessions_.insert(std::make_pair(session->GetSessionId(), session)).second;
    }

    bool AFCTCPServer::CloseSession(AFTCPSessionPtr& session)
    {
        if (session != nullptr)
        {
            auto session_id = session->GetSessionId();
            session->GetSession()->postDisConnect();
            ARK_DELETE(session);
            sessions_.erase(session_id);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool AFCTCPServer::CloseSession(const int64_t& session_id)
    {
        AFTCPSessionPtr session = GetNetSession(session_id);
        return CloseSession(session);
    }

    bool AFCTCPServer::CloseAllSession()
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

    AFTCPSessionPtr AFCTCPServer::GetNetSession(const int64_t& session_id)
    {
        auto iter = sessions_.find(session_id);
        return (iter != sessions_.end() ? iter->second : nullptr);
    }

    //bool AFCTCPServer::SendMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& session_id, const AFGUID& actor_id)
    //{
    //    //AFTCPMsg msg;

    //    //AFCSMsgHead head;
    //    //head.set_msg_id(msg_id);
    //    //head.set_uid(actor_id);
    //    //head.set_body_length(msg_len);

    //    //std::string out_data;
    //    //size_t whole_len = EnCode(head, msg, msg_len, out_data);
    //    //if (whole_len == msg_len + GetHeadLength())
    //    //{
    //    //    return SendMsg(out_data.c_str(), out_data.length(), session_id);
    //    //}
    //    //else
    //    //{
    //    //    return false;
    //    //}

    //    return true;
    //}

    bool AFCTCPServer::SendMsg(AFMsgHead* head, const char* msg_data, const int64_t session_id)
    {
        if (head == nullptr || msg_data == nullptr || session_id <= 0)
        {
            return false;
        }

        auto session = GetNetSession(session_id);
        if (session == nullptr)
        {
            return false;
        }

        std::string buffer;
        switch (session->GetHeadLen())
        {
        case AFHeadLength::CS_HEAD_LENGTH:
            buffer.append(reinterpret_cast<char*>(head), AFHeadLength::CS_HEAD_LENGTH);
            break;
        case AFHeadLength::SS_HEAD_LENGTH:
            buffer.append(reinterpret_cast<char*>(head), AFHeadLength::SS_HEAD_LENGTH);
            break;
        default:
            return false;
            break;
        }

        if (buffer.empty())
        {
            return false;
        }

        buffer.append(msg_data, head->length_);
        session->GetSession()->send(buffer.c_str(), buffer.length());
        return true;
    }

    bool AFCTCPServer::BroadcastMsg(AFMsgHead* head, const char* msg_data)
    {
        if (head == nullptr || msg_data == nullptr)
        {
            return false;
        }

        if (sessions_.empty())
        {
            return false;
        }

        auto iter = sessions_.begin();
        auto first_session = iter->second;
        if (first_session == nullptr)
        {
            return false;
        }

        char* new_buffer = reinterpret_cast<char*>(head);

        uint32_t new_buffer_len = first_session->GetHeadLen() + head->length_;
        memcpy(new_buffer + first_session->GetHeadLen(), msg_data, head->length_);

        for (auto& session : sessions_)
        {
            session.second->GetSession()->send(new_buffer, new_buffer_len);
        }

        return true;
    }

}
