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

#include "AFCTCPServer.h"

#if ARK_PLATFORM == PLATFORM_WIN
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma  comment(lib,"Ws2_32.lib")
#elif ARK_PLATFORM == PLATFORM_APPLE
#include <arpa/inet.h>
#endif

namespace ark
{

    AFCTCPServer::AFCTCPServer()
    {
        brynet::net::base::InitSocket();
        tcp_service_ptr_ = brynet::net::TcpService::Create();
        listen_thread_ptr_ = brynet::net::ListenThread::Create();
    }

    AFCTCPServer::~AFCTCPServer()
    {
        Shutdown();
        brynet::net::base::DestroySocket();
    }

    void AFCTCPServer::Update()
    {
        ProcessMsgLogicThread();
    }

    bool AFCTCPServer::Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6/* = false*/)
    {
        tcp_service_ptr_->startWorkerThread(thread_num);
        bus_id_ = busid;

        listen_thread_ptr_->startListen(ip_v6, ip, port, [ = ](brynet::net::TcpSocket::PTR socket)
        {
            AFCTCPServer* pTcpServer = this;
            socket->SocketNodelay();
            auto OnEnterCallback = [pTcpServer](const brynet::net::DataSocket::PTR & session)
            {
                AFTCPMsg* pMsg = new AFTCPMsg(session);
                pMsg->conn_id_.nLow = pTcpServer->next_conn_id_++;
                pMsg->event_ = CONNECTED;

                do
                {
                    AFScopeWrLock xGuard(pTcpServer->rw_lock_);

                    AFTCPEntityPtr pEntity = ARK_NEW AFTCPEntity(pTcpServer, pMsg->conn_id_, session);
                    session->setUD(int64_t(pEntity));
                    if (pTcpServer->AddNetEntity(pMsg->conn_id_, pEntity))
                    {
                        pEntity->msg_queue_.Push(pMsg);
                    }
                } while (false);

                session->setDataCallback([pTcpServer, session](const char* buffer, size_t len)
                {
                    auto pUD = brynet::net::cast<int64_t>(session->getUD());

                    if (nullptr != pUD)
                    {
                        const AFTCPEntityPtr pEntity = AFTCPEntityPtr(*pUD);
                        pEntity->AddBuff(buffer, len);
                        pTcpServer->DismantleNet(pEntity);
                    }

                    return len;
                });

                session->setDisConnectCallback([](const brynet::net::DataSocket::PTR & session)
                {
                    auto pUD = brynet::net::cast<int64_t>(session->getUD());

                    if (nullptr == pUD)
                    {
                        return;
                    }

                    const AFTCPEntityPtr pEntity = AFTCPEntityPtr(*pUD);
                    AFTCPMsg* pMsg = new AFTCPMsg(session);
                    pMsg->conn_id_ = pEntity->GetConnID();
                    pMsg->event_ = DISCONNECTED;

                    pEntity->msg_queue_.Push(pMsg);
                });

                //session->setHeartBeat(ARK_NET_HEART_TIME);
            };

            tcp_service_ptr_->addDataSocket(std::move(socket),
                                            brynet::net::TcpService::AddSocketOption::WithEnterCallback(OnEnterCallback),
                                            brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_TCP_RECV_BUFFER_SIZE));
        });

        SetWorking(true);
        return true;
    }

    void AFCTCPServer::ProcessMsgLogicThread()
    {
        std::list<AFGUID> xNeedRemoveList;

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

                xNeedRemoveList.push_back(iter.second->GetConnID());
            }
        } while (false);

        for (auto iter : xNeedRemoveList)
        {
            AFScopeWrLock xGuard(rw_lock_);
            RemoveNetEntity(iter);
        }
    }

    void AFCTCPServer::ProcessMsgLogicThread(AFTCPEntityPtr pEntity)
    {
        //Handle Msg
        size_t queue_size = pEntity->msg_queue_.Count();
        for (size_t i = 0; i < queue_size && i < 100; ++i)
        {
            AFTCPMsg* pMsg(nullptr);

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
                net_event_cb_((NetEventType)pMsg->event_, pMsg->conn_id_, pMsg->session_->getIP(), bus_id_);
                break;
            case DISCONNECTED:
                {
                    net_event_cb_((NetEventType)pMsg->event_, pMsg->conn_id_, pMsg->session_->getIP(), bus_id_);
                    pEntity->SetNeedRemove(true);
                }
                break;

            default:
                break;
            }

            ARK_DELETE(pMsg);
        }
    }

    bool AFCTCPServer::Shutdown()
    {
        SetWorking(false);
        return true;
    }

    bool AFCTCPServer::IsServer()
    {
        return true;
    }

    bool AFCTCPServer::SendMsgToAllClient(const char* msg, const size_t msg_len)
    {
        for (auto it : net_entities_)
        {
            AFTCPEntityPtr pNetObject = (AFTCPEntityPtr)it.second;
            if (pNetObject != nullptr && !pNetObject->NeedRemove())
            {
                pNetObject->GetSession()->send(msg, msg_len);
            }
        }

        return true;
    }

    bool AFCTCPServer::SendMsg(const char* msg, const size_t msg_len, const AFGUID& conn_id)
    {
        AFScopeRdLock xGuard(rw_lock_);

        AFTCPEntityPtr pNetObject = GetNetEntity(conn_id);
        if (pNetObject == nullptr)
        {
            return false;
        }
        else
        {
            pNetObject->GetSession()->send(msg, msg_len);
            return true;
        }
    }

    bool AFCTCPServer::AddNetEntity(const AFGUID& conn_id, AFTCPEntityPtr entity_ptr)
    {
        return net_entities_.insert(std::make_pair(conn_id, entity_ptr)).second;
    }

    bool AFCTCPServer::RemoveNetEntity(const AFGUID& conn_id)
    {
        AFTCPEntityPtr pEntity = GetNetEntity(conn_id);

        if (pEntity != nullptr)
        {
            ARK_DELETE(pEntity);
        }

        return net_entities_.erase(conn_id);
    }

    bool AFCTCPServer::CloseNetEntity(const AFGUID& conn_id)
    {
        AFTCPEntityPtr pEntity = GetNetEntity(conn_id);

        if (pEntity != nullptr)
        {
            pEntity->GetSession()->postDisConnect();
            return true;
        }
        else
        {
            return true;
        }
    }

    bool AFCTCPServer::Log(int severity, const char* msg)
    {
        //will add msg log
        return true;
    }

    bool AFCTCPServer::DismantleNet(AFTCPEntityPtr entity_ptr)
    {
        while (entity_ptr->GetBuffLen() >= ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            ARK_PKG_CS_HEAD head;
            int msg_body_len = DeCode(entity_ptr->GetBuff(), entity_ptr->GetBuffLen(), head);

            if (msg_body_len >= 0 && head.GetMsgID() > 0)
            {
                AFTCPMsg* pNetInfo = ARK_NEW AFTCPMsg(entity_ptr->GetSession());
                pNetInfo->head_ = head;
                pNetInfo->event_ = RECVDATA;
                pNetInfo->msg_data_.append(entity_ptr->GetBuff() + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH, msg_body_len);
                entity_ptr->msg_queue_.Push(pNetInfo);
                entity_ptr->RemoveBuff(msg_body_len + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH);
            }
            else
            {
                break;
            }
        }

        return true;
    }

    bool AFCTCPServer::CloseSocketAll()
    {
        for (auto it : net_entities_)
        {
            it.second->GetSession()->postDisConnect();
            ARK_DELETE(it.second);
        }

        net_entities_.clear();
        return true;
    }

    AFCTCPServer::AFTCPEntityPtr AFCTCPServer::GetNetEntity(const AFGUID& conn_id)
    {
        auto it = net_entities_.find(conn_id);
        return (it != net_entities_.end() ? it->second : nullptr);
    }

    bool AFCTCPServer::SendRawMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id, const AFGUID& actor_rid)
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

    bool AFCTCPServer::SendRawMsgToAllClient(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& actor_rid)
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

    int AFCTCPServer::EnCode(const ARK_PKG_CS_HEAD& head, const char* msg, const size_t len, OUT std::string& out_data)
    {
        char head_string[ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH] = { 0 };
        head.EnCode(head_string);

        out_data.clear();
        out_data.append(head_string, ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH);
        out_data.append(msg, len);

        return head.GetBodyLength() + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH;
    }

    int AFCTCPServer::DeCode(const char* data, const size_t len, ARK_PKG_CS_HEAD& head)
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