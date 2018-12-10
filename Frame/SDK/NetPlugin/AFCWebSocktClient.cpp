/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include <brynet/net/SyncConnector.h>
#include <brynet/net/http/HttpFormat.h>
#include "AFCWebSocktClient.h"

namespace ark
{

    AFCWebSocktClient::AFCWebSocktClient(brynet::net::TcpService::PTR service /*= nullptr*/, brynet::net::AsyncConnector::PTR connector /*= nullptr*/)
    {
        brynet::net::base::InitSocket();
        tcp_service_ptr_ = (service != nullptr ? service : brynet::net::TcpService::Create());
    }

    AFCWebSocktClient::~AFCWebSocktClient()
    {
        Shutdown();
        brynet::net::base::DestroySocket();
    }

    void AFCWebSocktClient::Update()
    {
        UpdateNetSession();
    }

    bool AFCWebSocktClient::StartClient(AFHeadLength len, const int target_busid, const std::string& ip, const int port, bool ip_v6)
    {
        dst_bus_id_ = target_busid;
        tcp_service_ptr_->startWorkerThread(1);

        sock fd = brynet::net::base::Connect(ip_v6, ip, port);
        auto socket = brynet::net::TcpSocket::Create(fd, false);
        socket->SocketNodelay();

        CONSOLE_LOG_NO_FILE << "connect success" << std::endl;
        auto OnEnterCallback = [ = ](const brynet::net::DataSocket::PTR & session)
        {
            AFCWebSocktClient* this_ptr = this;
            brynet::net::HttpService::setup(session, [this_ptr, len, ip](const brynet::net::HttpSession::PTR & httpSession)
            {
                brynet::net::HttpRequest request;
                request.setMethod(brynet::net::HttpRequest::HTTP_METHOD::HTTP_METHOD_GET);
                request.setUrl("/ws");
                request.addHeadValue("Host", ip);
                request.addHeadValue("Upgrade", "websocket");
                request.addHeadValue("Connection", "Upgrade");
                request.addHeadValue("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
                request.addHeadValue("Sec-WebSocket-Version", "13");

                std::string requestStr = request.getResult();
                httpSession->send(requestStr.c_str(), requestStr.size());

                httpSession->setWSConnected([this_ptr, len](const brynet::net::HttpSession::PTR & httpSession, const brynet::net::HTTPParser&)
                {
                    //now conn_id
                    int cur_session_id = this_ptr->trust_session_id_++;

                    //create net event
                    AFNetEvent* net_event_ptr = AFNetEvent::AllocEvent();
                    net_event_ptr->id_ = cur_session_id;
                    net_event_ptr->type_ = AFNetEventType::CONNECTED;
                    net_event_ptr->bus_id_ = this_ptr->dst_bus_id_;
                    //net_event_ptr->ip_ = httpSession->();

                    //set session ud
                    httpSession->setUD(cur_session_id);

                    //create session and operate data
                    do
                    {
                        AFScopeWLock guard(this_ptr->rw_lock_);

                        AFHttpSessionPtr session_ptr = ARK_NEW AFHttpSession(len, cur_session_id, httpSession);
                        this_ptr->client_session_ptr_.reset(session_ptr);
                        session_ptr->AddNetEvent(net_event_ptr);
                    } while (false);
                });

                httpSession->setWSCallback([this_ptr](const brynet::net::HttpSession::PTR & httpSession,
                                                      brynet::net::WebSocketFormat::WebSocketFrameType opcode,
                                                      const std::string & payload)
                {
                    const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
                    int64_t session_id = *ud;

                    do
                    {
                        AFScopeRLock xGuard(this_ptr->rw_lock_);
                        this_ptr->client_session_ptr_->AddBuffer(payload.c_str(), payload.size());
                        this_ptr->client_session_ptr_->ParseBufferToMsg();
                    } while (false);
                });

                httpSession->setCloseCallback([this_ptr](const brynet::net::HttpSession::PTR & httpSession)
                {
                    const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
                    int64_t session_id = *ud;

                    //create net event
                    AFNetEvent* net_event_ptr = AFNetEvent::AllocEvent();
                    net_event_ptr->id_ = session_id;
                    net_event_ptr->type_ = AFNetEventType::DISCONNECTED;
                    net_event_ptr->bus_id_ = this_ptr->dst_bus_id_;
                    //net_event_ptr->ip_ = session->getIP();

                    do
                    {
                        AFScopeWLock xGuard(this_ptr->rw_lock_);
                        this_ptr->client_session_ptr_->AddNetEvent(net_event_ptr);
                        this_ptr->client_session_ptr_->SetNeedRemove(true);
                    } while (false);

                });
            });
        };

        return tcp_service_ptr_->addDataSocket(std::move(socket),
                                               brynet::net::TcpService::AddSocketOption::WithEnterCallback(OnEnterCallback),
                                               brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE));
    }

    bool AFCWebSocktClient::Shutdown()
    {
        if (!CloseAllSession())
        {
            //add log
        }

        tcp_service_ptr_->stopWorkerThread();
        SetWorking(false);
        return true;
    }

    bool AFCWebSocktClient::CloseAllSession()
    {
        client_session_ptr_->GetSession()->postClose();
        return true;
    }

    void AFCWebSocktClient::UpdateNetSession()
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
            client_session_ptr_.release();
        }
    }

    void AFCWebSocktClient::UpdateNetEvent(AFHttpSessionPtr session)
    {
        if (session == nullptr)
        {
            return;
        }

        AFNetEvent* event = nullptr;
        if (session->PopNetEvent(event))
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

    void AFCWebSocktClient::UpdateNetMsg(AFHttpSessionPtr session)
    {
        if (session == nullptr)
        {
            return;
        }

        AFNetMsg* msg = nullptr;
        if (!session->PopNetMsg(msg))
        {
            return;
        }

        int msg_count = 0;
        while (msg != nullptr)
        {
            net_recv_cb_(msg);
            AFNetMsg::Release(msg);

            ++msg_count;
            if (msg_count > ARK_PROCESS_NET_MSG_COUNT_ONCE)
            {
                break;
            }

            session->PopNetMsg(msg);
        }
    }

    bool AFCWebSocktClient::SendMsg(const char* msg, const size_t msg_len, const AFGUID& conn_id/* = 0*/)
    {
        auto frame = std::make_shared<std::string>();
        brynet::net::WebSocketFormat::wsFrameBuild(msg,
                msg_len,
                *frame,
                brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME,
                true,
                false);

        if (client_session_ptr_->GetSession())
        {
            client_session_ptr_->GetSession()->send(frame);
        }

        return true;
    }

    bool AFCWebSocktClient::CloseSession(const AFGUID& session_id)
    {
        client_session_ptr_->GetSession()->postClose();
        return true;
    }

    bool AFCWebSocktClient::SendRawMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id/* = 0*/, const AFGUID& actor_id/* = 0*/)
    {
        //AFCSMsgHead head;
        //head.set_msg_id(msg_id);
        //head.set_uid(actor_id);
        //head.set_body_length(msg_len);

        //std::string out_data;
        //size_t whole_len = EnCode(head, msg, msg_len, out_data);
        //if (whole_len == msg_len + GetHeadLength())
        //{
        //    return SendMsg(out_data.c_str(), out_data.length(), conn_id);
        //}
        //else
        //{
        //    return false;
        //}

        return true;
    }

}