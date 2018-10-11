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

#include "AFCWebSocktServer.h"

namespace ark
{

    AFCWebSocktServer::AFCWebSocktServer()
    {
        brynet::net::base::InitSocket();
        tcp_service_ptr_ = brynet::net::TcpService::Create();
        listen_thread_ptr_ = brynet::net::ListenThread::Create();
    }

    AFCWebSocktServer::~AFCWebSocktServer()
    {
        Shutdown();
        brynet::net::base::DestroySocket();
    }

    void AFCWebSocktServer::Update()
    {
        ProcessMsgLogicThread();
    }

    bool AFCWebSocktServer::Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6/* = false*/)
    {
        bus_id_ = busid;
        tcp_service_ptr_->startWorkerThread(thread_num);

        listen_thread_ptr_->startListen(ip_v6, ip, port, [ = ](brynet::net::TcpSocket::PTR socket)
        {
            AFCWebSocktServer* pWSServer = this;
            auto OnEnterCallback = [pWSServer](const brynet::net::DataSocket::PTR & session)
            {
                brynet::net::HttpService::setup(session, [pWSServer](const brynet::net::HttpSession::PTR & httpSession)
                {
                    AFHttpMsg* pMsg = new AFHttpMsg(httpSession);
                    pMsg->conn_id_.nLow = pWSServer->next_conn_id_++;
                    httpSession->setUD(static_cast<int64_t>(pMsg->conn_id_.nLow));
                    pMsg->event_ = CONNECTED;

                    do
                    {
                        AFScopeWrLock xGuard(pWSServer->rw_lock_);

                        AFHttpEntity* pEntity = new AFHttpEntity(pWSServer, pMsg->conn_id_, httpSession);

                        if (pWSServer->AddNetEntity(pMsg->conn_id_, pEntity))
                        {
                            pEntity->msg_queue_.Push(pMsg);
                        }
                    } while (false);

                    httpSession->setWSCallback([pWSServer](const brynet::net::HttpSession::PTR & httpSession,
                                                           brynet::net::WebSocketFormat::WebSocketFrameType opcode,
                                                           const std::string & payload)
                    {
                        switch (opcode)
                        {
                        case brynet::net::WebSocketFormat::WebSocketFrameType::ERROR_FRAME:
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::CONTINUATION_FRAME:
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::TEXT_FRAME:
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME:
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::CLOSE_FRAME:
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::PING_FRAME:
                            {
                                auto frame = std::make_shared<std::string>();
                                brynet::net::WebSocketFormat::wsFrameBuild(payload.c_str(),
                                        payload.size(),
                                        *frame,
                                        brynet::net::WebSocketFormat::WebSocketFrameType::PONG_FRAME,
                                        true,
                                        false);
                                httpSession->send(frame);
                                return;
                            }
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::PONG_FRAME:
                            break;
                        default:
                            break;
                        }

                        const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
                        AFGUID xClient(0, *ud);

                        AFScopeRdLock xGuard(pWSServer->rw_lock_);
                        auto xFind = pWSServer->net_entities_.find(xClient);
                        if (xFind == pWSServer->net_entities_.end())
                        {
                            return;
                        }

                        xFind->second->AddBuff(payload.c_str(), payload.size());
                        pWSServer->DismantleNet(xFind->second);
                    });

                    httpSession->setHttpCallback([](const brynet::net::HTTPParser & httpParser, const brynet::net::HttpSession::PTR & session)
                    {
                        brynet::net::HttpResponse response;
                        response.setBody("<html>Hello ArkGame</html>");
                        std::string result = response.getResult();
                        session->send(result.c_str(), result.size(), [session]()
                        {
                            session->postShutdown();
                        });
                    });

                    httpSession->setCloseCallback([pWSServer](const brynet::net::HttpSession::PTR & httpSession)
                    {
                        const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
                        AFGUID conn_id(0, 0);
                        conn_id.nLow = *ud;
                        AFScopeWrLock xGuard(pWSServer->rw_lock_);

                        auto xFind = pWSServer->net_entities_.find(conn_id);
                        if (xFind == pWSServer->net_entities_.end())
                        {
                            return;
                        }

                        AFHttpMsg* pMsg = ARK_NEW AFHttpMsg(httpSession);
                        pMsg->conn_id_ = conn_id;
                        pMsg->event_ = DISCONNECTED;

                        xFind->second->msg_queue_.Push(pMsg);
                    });
                });
            };

            tcp_service_ptr_->addDataSocket(std::move(socket),
                                            brynet::net::TcpService::AddSocketOption::WithEnterCallback(OnEnterCallback),
                                            brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE));
        });

        return true;
    }

    void AFCWebSocktServer::ProcessMsgLogicThread()
    {
        std::list<AFGUID> need_remove_list;
        do
        {
            AFScopeRdLock xGuard(rw_lock_);

            for (auto& iter : net_entities_)
            {
                ProcessMsgLogicThread(iter.second);

                if (!iter.second->NeedRemove())
                {
                    continue;
                }

                need_remove_list.push_back(iter.second->GetConnID());
            }
        } while (false);

        for (auto iter : need_remove_list)
        {
            AFScopeWrLock xGuard(rw_lock_);
            RemoveNetEntity(iter);
        }
    }

    void AFCWebSocktServer::ProcessMsgLogicThread(AFHttpEntityPtr pEntity)
    {
        //Handle Msg
        size_t queue_size = pEntity->msg_queue_.Count();
        for (size_t i = 0; i < queue_size && i < 100; ++i)
        {
            AFHttpMsg* pMsg(nullptr);

            if (!pEntity->msg_queue_.Pop(pMsg))
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
                        net_recv_cb_(pMsg->head_, pMsg->head_.GetMsgID(), pMsg->msg_data_.c_str(), pMsg->msg_data_.size(), pEntity->GetConnID());
                    }
                }
                break;
            case CONNECTED:
                net_event_cb_(pMsg->event_, pMsg->conn_id_, "unknown", bus_id_); //TODO:Wanna get ip, need to change brynet interface, give "unknown" for temp
                break;
            case DISCONNECTED:
                {
                    net_event_cb_(pMsg->event_, pMsg->conn_id_, "unknown", bus_id_);
                    pEntity->SetNeedRemove(true);
                }
                break;

            default:
                break;
            }

            delete pMsg;
        }
    }

    bool AFCWebSocktServer::Shutdown()
    {
        SetWorking(false);
        return true;
    }

    bool AFCWebSocktServer::IsServer()
    {
        return true;
    }

    bool AFCWebSocktServer::SendMsgToAllClient(const char* msg, const size_t nLen)
    {
        auto frame = std::make_shared<std::string>();
        brynet::net::WebSocketFormat::wsFrameBuild(msg,
                nLen,
                *frame,
                brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME,
                true,
                false);

        for (auto& it : net_entities_)
        {
            AFHttpEntityPtr pNetObject = it.second;

            if (pNetObject && !pNetObject->NeedRemove())
            {
                pNetObject->GetSession()->send(frame);
            }
        }

        return true;
    }

    bool AFCWebSocktServer::SendMsg(const char* msg, const size_t msg_len, const AFGUID& conn_id)
    {
        AFScopeRdLock xGuard(rw_lock_);

        AFHttpEntity* pNetObject = GetNetEntity(conn_id);

        if (pNetObject == nullptr)
        {
            return false;
        }

        auto frame = std::make_shared<std::string>();
        brynet::net::WebSocketFormat::wsFrameBuild(msg,
                msg_len,
                *frame,
                brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME,
                true,
                false);

        pNetObject->GetSession()->send(frame);
        return true;
    }

    bool AFCWebSocktServer::AddNetEntity(const AFGUID& conn_id, AFHttpEntityPtr entity_ptr)
    {
        return net_entities_.insert(std::make_pair(conn_id, entity_ptr)).second;
    }

    bool AFCWebSocktServer::RemoveNetEntity(const AFGUID& conn_id)
    {
        AFHttpEntity* pNetObject = GetNetEntity(conn_id);

        if (pNetObject)
        {
            delete pNetObject;
        }

        return net_entities_.erase(conn_id);
    }

    bool AFCWebSocktServer::CloseNetEntity(const AFGUID& conn_id)
    {
        AFHttpEntity* pEntity = GetNetEntity(conn_id);
        if (pEntity)
        {
            pEntity->GetSession()->postShutdown();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool AFCWebSocktServer::Log(int severity, const char* msg)
    {
        return true;
    }

    bool AFCWebSocktServer::DismantleNet(AFHttpEntityPtr entity_ptr)
    {
        while (entity_ptr->GetBuffLen() >= ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            ARK_PKG_CS_HEAD head;
            int msg_body_len = DeCode(entity_ptr->GetBuff(), entity_ptr->GetBuffLen(), head);
            if (msg_body_len >= 0 && head.GetMsgID() > 0)
            {
                AFHttpMsg* pMsg = ARK_NEW AFHttpMsg(entity_ptr->GetSession());
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

    bool AFCWebSocktServer::CloseSocketAll()
    {
        for (auto it : net_entities_)
        {
            it.second->GetSession()->postShutdown();
            ARK_DELETE(it.second);
        }

        net_entities_.clear();
        return true;
    }

    AFCWebSocktServer::AFHttpEntityPtr AFCWebSocktServer::GetNetEntity(const AFGUID& conn_id)
    {
        auto it = net_entities_.find(conn_id);
        return (it != net_entities_.end() ? it->second : nullptr);
    }

    bool AFCWebSocktServer::SendRawMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id, const AFGUID& actor_rid)
    {
        std::string out_data;
        ARK_PKG_CS_HEAD head;
        head.SetMsgID(msg_id);
        head.SetUID(actor_rid);
        head.SetBodyLength(msg_len);

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

    bool AFCWebSocktServer::SendRawMsgToAllClient(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& actor_rid)
    {
        std::string out_data;
        ARK_PKG_CS_HEAD head;
        head.SetMsgID(msg_id);
        head.SetUID(actor_rid);

        int whole_len = EnCode(head, msg, msg_len, out_data);
        if (whole_len == msg_len + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            return SendMsgToAllClient(out_data.c_str(), out_data.length());
        }
        else
        {
            return false;
        }
    }

    int AFCWebSocktServer::EnCode(const ARK_PKG_CS_HEAD& head, const char* msg, const size_t len, OUT std::string& out_data)
    {
        char head_string[ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH] = { 0 };
        head.EnCode(head_string);

        out_data.clear();
        out_data.append(head_string, ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH);
        out_data.append(msg, len);

        return head.GetBodyLength() + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH;
    }

    int AFCWebSocktServer::DeCode(const char* data, const size_t len, ARK_PKG_CS_HEAD& head)
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