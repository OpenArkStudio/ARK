/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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
        ProcessMsgLogicThread();
    }

    bool AFCWebSocktClient::Start(const int target_busid, const std::string& ip, const int port, bool ip_v6)
    {
        dst_bus_id_ = target_busid;
        tcp_service_ptr_->startWorkerThread(1);

        sock fd = brynet::net::base::Connect(ip_v6, ip, port);
        auto socket = brynet::net::TcpSocket::Create(fd, false);
        socket->SocketNodelay();

        CONSOLE_LOG_NO_FILE << "connect success" << std::endl;
        auto enterCallback = [ = ](const brynet::net::DataSocket::PTR & session)
        {
            AFCWebSocktClient* pWSClient = this;
            brynet::net::HttpService::setup(session, [pWSClient, ip](const brynet::net::HttpSession::PTR & httpSession)
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

                httpSession->setWSConnected([pWSClient](const brynet::net::HttpSession::PTR & httpSession, const brynet::net::HTTPParser&)
                {
                    AFHttpMsg* pMsg = ARK_NEW AFHttpMsg(httpSession);
                    httpSession->setUD(static_cast<int64_t>(pMsg->conn_id_.nLow));
                    pMsg->event_ = CONNECTED;

                    do
                    {
                        AFScopeWrLock xGuard(pWSClient->rw_lock_);

                        AFHttpEntity* pEntity = ARK_NEW AFHttpEntity(pWSClient, pMsg->conn_id_, httpSession);
                        pWSClient->client_entity_ptr_.reset(pEntity);
                        pEntity->msg_queue_.Push(pMsg);
                    } while (false);
                });

                httpSession->setWSCallback([pWSClient](const brynet::net::HttpSession::PTR & httpSession,
                                                       brynet::net::WebSocketFormat::WebSocketFrameType opcode,
                                                       const std::string & payload)
                {
                    const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
                    AFGUID xClient(0, *ud);

                    AFScopeRdLock xGuard(pWSClient->rw_lock_);

                    pWSClient->client_entity_ptr_->AddBuff(payload.c_str(), payload.size());
                    pWSClient->DismantleNet(pWSClient->client_entity_ptr_.get());
                });

                httpSession->setCloseCallback([pWSClient](const brynet::net::HttpSession::PTR & httpSession)
                {
                    const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
                    AFGUID conn_id(0, *ud);
                    AFScopeWrLock xGuard(pWSClient->rw_lock_);

                    AFHttpMsg* pMsg = new AFHttpMsg(httpSession);
                    pMsg->conn_id_ = conn_id;
                    pMsg->event_ = DISCONNECTED;
                    pWSClient->client_entity_ptr_->msg_queue_.Push(pMsg);
                });
            });
        };

        return tcp_service_ptr_->addDataSocket(std::move(socket),
                                               brynet::net::TcpService::AddSocketOption::WithEnterCallback(enterCallback),
                                               brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE));
    }

    bool AFCWebSocktClient::Shutdown()
    {
        if (!CloseSocketAll())
        {
            //add log
        }

        tcp_service_ptr_->stopWorkerThread();
        SetWorking(false);
        return true;
    }

    bool AFCWebSocktClient::CloseSocketAll()
    {
        client_entity_ptr_->GetSession()->postClose();
        return true;
    }

    void AFCWebSocktClient::ProcessMsgLogicThread()
    {
        {
            AFScopeRdLock xGuard(rw_lock_);
            ProcessMsgLogicThread(client_entity_ptr_.get());
        }

        if (client_entity_ptr_ != nullptr && client_entity_ptr_->NeedRemove())
        {
            AFScopeWrLock xGuard(rw_lock_);
            client_entity_ptr_.release();
        }
    }

    void AFCWebSocktClient::ProcessMsgLogicThread(AFHttpEntityPtr entity_ptr)
    {
        if (entity_ptr == nullptr)
        {
            return;
        }

        //Handle messages
        size_t queue_size = entity_ptr->msg_queue_.Count();
        for (size_t i = 0; i < queue_size && i < 100; ++i)
        {
            AFHttpMsg* pMsg(nullptr);

            if (!entity_ptr->msg_queue_.Pop(pMsg))
            {
                break;
            }

            if (pMsg == nullptr)
            {
                continue;
            }

            switch (pMsg->event_)
            {
            case RECVDATA:
                {
                    if (net_recv_cb_)
                    {
                        net_recv_cb_(pMsg->head_, pMsg->head_.GetMsgID(), pMsg->msg_data_.c_str(), pMsg->msg_data_.size(), entity_ptr->GetConnID());
                    }
                }
                break;

            case CONNECTED:
                net_event_cb_(pMsg->event_, pMsg->conn_id_, "unknown", dst_bus_id_);//TODO:Wanna get ip, need to change brynet interface, give "unknown" for temp
                break;

            case DISCONNECTED:
                {
                    net_event_cb_(pMsg->event_, pMsg->conn_id_, "unknown", dst_bus_id_);
                    entity_ptr->SetNeedRemove(true);
                }
                break;

            default:
                break;
            }

            delete pMsg;
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

        if (client_entity_ptr_->GetSession())
        {
            client_entity_ptr_->GetSession()->send(frame);
        }

        return true;
    }

    bool AFCWebSocktClient::CloseNetEntity(const AFGUID& conn_id)
    {
        if (client_entity_ptr_->GetConnID() == conn_id)
        {
            client_entity_ptr_->GetSession()->postClose();
        }

        return true;
    }

    bool AFCWebSocktClient::DismantleNet(AFHttpEntityPtr entity_ptr)
    {
        while (entity_ptr->GetBuffLen() >= ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            ARK_PKG_CS_HEAD head;
            int msg_body_len = DeCode(entity_ptr->GetBuff(), entity_ptr->GetBuffLen(), head);
            if (msg_body_len >= 0 && head.GetMsgID() > 0)
            {
                AFHttpMsg* pMsg = new AFHttpMsg(entity_ptr->GetSession());
                pMsg->head_ = head;
                pMsg->event_ = RECVDATA;
                pMsg->msg_data_.append(entity_ptr->GetBuff() + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH, msg_body_len);
                entity_ptr->msg_queue_.Push(pMsg);
                entity_ptr->RemoveBuff(msg_body_len + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH);
            }
            else
            {
                break;
            }
        }

        return true;
    }

    bool AFCWebSocktClient::IsServer()
    {
        return false;
    }

    bool AFCWebSocktClient::Log(int severity, const char* msg)
    {
        //Will add log
        return true;
    }

    bool AFCWebSocktClient::SendRawMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id/* = 0*/, const AFGUID& actor_id/* = 0*/)
    {
        ARK_PKG_CS_HEAD head;
        head.SetMsgID(msg_id);
        head.SetUID(actor_id);
        head.SetBodyLength(msg_len);

        std::string out_data;
        int whole_len = EnCode(head, msg, msg_len, out_data);
        if (whole_len == msg_len + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            return SendMsg(out_data.c_str(), out_data.length(), conn_id);
        }
        else
        {
            return false;
        }
    }

    int AFCWebSocktClient::EnCode(const ARK_PKG_CS_HEAD& head, const char* msg, const size_t len, OUT std::string& out_data)
    {
        char head_string[ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH] = { 0 };
        head.EnCode(head_string);

        out_data.clear();
        out_data.append(head_string, ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH);
        out_data.append(msg, len);

        return head.GetBodyLength() + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH;
    }

    int AFCWebSocktClient::DeCode(const char* data, const size_t len, ARK_PKG_CS_HEAD& head)
    {
        if (len < ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            return -1;
        }

        if (ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH != head.DeCode(data))
        {
            return -2;
        }

        if (head.GetBodyLength() > (len - ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH))
        {
            return -3;
        }

        return head.GetBodyLength();
    }

}