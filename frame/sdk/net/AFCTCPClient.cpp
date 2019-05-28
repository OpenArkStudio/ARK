/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

#include "base/AFBaseStruct.hpp"
#include "AFCTCPClient.h"

namespace ark
{

    AFCTCPClient::AFCTCPClient(const brynet::net::TcpService::Ptr& service/* = nullptr*/, const brynet::net::AsyncConnector::Ptr& connector/* = nullptr*/)
    {
        brynet::net::base::InitSocket();

        tcp_service_ptr_ = (service != nullptr ? service : brynet::net::TcpService::Create());
        connector_ptr_ = (connector != nullptr ? connector : brynet::net::AsyncConnector::Create());
    }

    AFCTCPClient::~AFCTCPClient()
    {
        CloseAllSession();
        Shutdown();
        brynet::net::base::DestroySocket();
    }

    void AFCTCPClient::Update()
    {
        UpdateNetSession();
    }

    bool AFCTCPClient::StartClient(AFHeadLength head_len, const int dst_busid, const std::string& ip, const int port, bool ip_v6/* = false*/)
    {
        this->dst_bus_id_ = dst_busid;

        tcp_service_ptr_->startWorkerThread(1);
        connector_ptr_->startWorkerThread();

        auto socketEnterCallback = [&, head_len](brynet::net::TcpSocket::Ptr socket)
        {
            socket->setNodelay();
            AFCTCPClient* this_ptr = this;

            auto OnEnterCallback = [&, this_ptr, head_len](const brynet::net::TcpConnectionPtr & session)
            {
                //now session_id
                int64_t cur_session_id = this_ptr->trust_session_id_++;

                //set session ud
                session->setUD(cur_session_id);

                //create net event
                AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
                net_connect_event->id_ = cur_session_id;
                net_connect_event->type_ = AFNetEventType::CONNECTED;
                net_connect_event->bus_id_ = this_ptr->dst_bus_id_;
                net_connect_event->ip_ = session->getIP();

                //create session and operate data
                do
                {
                    AFScopeWLock guard(this_ptr->rw_lock_);

                    AFTCPSessionPtr session_ptr = ARK_NEW AFTCPSession(head_len, cur_session_id, session);
                    this_ptr->client_session_ptr_.reset(session_ptr);
                    session_ptr->AddNetEvent(net_connect_event);
                } while (false);

                //data cb
                session->setDataCallback([this_ptr, session](const char* buffer, size_t len)
                {
                    const auto ud = brynet::net::cast<int64_t>(session->getUD());
                    int64_t session_id = *ud;

                    do
                    {
                        AFScopeRLock guard(this_ptr->rw_lock_);
                        this_ptr->client_session_ptr_->AddBuffer(buffer, len);
                        this_ptr->client_session_ptr_->ParseBufferToMsg();
                    } while (false);

                    return len;
                });

                //disconnect cb
                session->setDisConnectCallback([this_ptr](const brynet::net::TcpConnectionPtr & session)
                {
                    const auto ud = brynet::net::cast<int64_t>(session->getUD());
                    int64_t session_id = *ud;

                    //create net event
                    AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
                    net_disconnect_event->id_ = session_id;
                    net_disconnect_event->type_ = AFNetEventType::DISCONNECTED;
                    net_disconnect_event->bus_id_ = this_ptr->dst_bus_id_;
                    net_disconnect_event->ip_ = session->getIP();

                    do
                    {
                        AFScopeWLock guard(this_ptr->rw_lock_);
                        this_ptr->client_session_ptr_->AddNetEvent(net_disconnect_event);
                        this_ptr->client_session_ptr_->SetNeedRemove(true);
                    } while (false);
                });
            };


            tcp_service_ptr_->addTcpConnection(std::move(socket),
                                               brynet::net::TcpService::AddSocketOption::AddEnterCallback(OnEnterCallback),
                                               brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_TCP_RECV_BUFFER_SIZE));
        };

        auto failedCallback = []()
        {
            std::cout << "connect failed" << std::endl;
        };

        connector_ptr_->asyncConnect(
        {
            brynet::net::AsyncConnector::ConnectOptions::WithAddr(ip, port),
            brynet::net::AsyncConnector::ConnectOptions::WithTimeout(std::chrono::seconds(10)),
            brynet::net::AsyncConnector::ConnectOptions::WithCompletedCallback(socketEnterCallback),
            brynet::net::AsyncConnector::ConnectOptions::WithFailedCallback(failedCallback)
        });


        SetWorking(true);
        return true;
    }

    bool AFCTCPClient::Shutdown()
    {
        if (!CloseAllSession())
        {
            //add log
        }

        connector_ptr_->stopWorkerThread();
        tcp_service_ptr_->stopWorkerThread();
        SetWorking(false);
        return true;
    }

    bool AFCTCPClient::CloseAllSession()
    {
        if (client_session_ptr_ != nullptr)
        {
            client_session_ptr_->GetSession()->postDisConnect();
        }

        return true;
    }

    bool AFCTCPClient::SendMsg(const char* msg, const size_t msg_len, const AFGUID& session_id/* = 0*/)
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
        do
        {
            AFScopeRLock guard(rw_lock_);
            UpdateNetEvent(client_session_ptr_.get());
            UpdateNetMsg(client_session_ptr_.get());
        } while (false);

        if (client_session_ptr_ != nullptr && client_session_ptr_->NeedRemove())
        {
            AFScopeWLock guard(rw_lock_);
            CloseSession(client_session_ptr_->GetSessionId());
            client_session_ptr_.reset(nullptr);
        }
    }

    void AFCTCPClient::UpdateNetEvent(AFTCPSessionPtr session)
    {
        if (session == nullptr)
        {
            return;
        }

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

    void AFCTCPClient::UpdateNetMsg(AFTCPSessionPtr session)
    {
        if (session == nullptr)
        {
            return;
        }

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

    bool AFCTCPClient::SendMsg(AFMsgHead* head, const char* msg_data, const int64_t session_id)
    {
        if (head == nullptr || msg_data == nullptr)
        {
            return false;
        }

        if (client_session_ptr_ == nullptr)
        {
            return false;
        }

        std::string buffer;
        switch (client_session_ptr_->GetHeadLen())
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
        client_session_ptr_->GetSession()->send(buffer.c_str(), buffer.length());
        return true;
    }

}