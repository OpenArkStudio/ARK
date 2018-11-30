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

#pragma once

#include "brynet/net/TCPService.h"
#include "brynet/net/http/HttpService.h"
#include "SDK/Core/AFMacros.hpp"
#include "SDK/Core/AFLockFreeQueue.hpp"
#include "SDK/Core/AFBuffer.hpp"
#include "AFNetMsg.h"

namespace ark
{

    class AFINet;

    enum AFNetEvent
    {
        NONE = 0,
        CONNECTED = 1,
        DISCONNECTED = 2,
        RECV_DATA = 3,
    };

    using NET_PKG_RECV_FUNCTOR = std::function<void(const AFIMsgHead& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id)>;
    using NET_PKG_RECV_FUNCTOR_PTR = std::shared_ptr<NET_PKG_RECV_FUNCTOR>;

    using NET_EVENT_FUNCTOR = std::function<void(const AFNetEvent event, const AFGUID& conn_id, const std::string& ip, const int bus_id)>;
    using NET_EVENT_FUNCTOR_PTR = std::shared_ptr<NET_EVENT_FUNCTOR>;

    template <typename SessionPTR>
    class AFNetSession
    {
    public:
        AFNetSession(AFINet* net_ptr, const int64_t& conn_id, const SessionPTR session) :
            conn_id_(conn_id),
            net_ptr_(net_ptr),
            session_(session)
        {
        }

        virtual ~AFNetSession() = default;

        const SessionPTR& get_session()
        {
            return session_;
        }

        int add_buffer(const char* data, size_t len)
        {
            buffer_.write(data, len);
            return (int)buffer_.get_length();
        }

        size_t remove_buffer(size_t len)
        {
            if (len > buffer_.get_length())
            {
                return 0;
            }

            buffer_.remove_data(len);
            return buffer_.get_length();
        }

        const char* get_buffer()
        {
            return buffer_.get_data();
        }

        size_t get_buffer_len()
        {
            return buffer_.get_length();
        }

        AFINet* get_net()
        {
            return net_ptr_;
        }

        bool get_need_remove()
        {
            return need_remove_;
        }

        void set_need_remove(bool b)
        {
            need_remove_ = b;
        }

        const int64_t& get_session_id()
        {
            return session_id_;
        }

        void set_session_id(const int64_t& id)
        {
            session_id_ = id;
        }

    private:
        int64_t session_id_{ 0 };
        AFGUID object_id_{ 0 };
        AFBuffer buffer_;

        AFINet* net_ptr_{ nullptr };
        AFLockFreeQueue<AFNetMsg<SessionPTR>*> msg_queue_;
        const SessionPTR session_;

        volatile bool connected_{ false };
        volatile bool need_remove_{ false };
    };

    using AFTCPSession = AFNetSession<brynet::net::DataSocket::PTR>;
    using AFHttpSession = AFNetSession<brynet::net::HttpSession::PTR>;

    class AFINet
    {
    public:
        AFINet() = default;
        virtual ~AFINet() = default;

        //need to call this function every frame to drive network library
        virtual void Update() = 0;
        virtual bool StartClient(const int dst_busid, const std::string& ip, const int port, bool ip_v6 = false) = 0;
        virtual bool StartServer(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6 = false) = 0;

        virtual bool Shutdown() = 0;

        //send a message with out msg-head[auto add msg-head in this function]
        virtual bool SendRawMsg(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id, const AFGUID& actor_id) = 0;

        //send a message to all client[need to add msg-head for this message by yourself]
        virtual bool SendMsgToAllClient(const char* msg, const uint32_t msg_len, const AFGUID& actor_id)
        {
            return false;
        }

        //send a message with out msg-head to all client[auto add msg-head in this function]
        virtual bool SendRawMsgToAllClient(const uint16_t msg_id, const char* msg, const size_t msg_len, const AFGUID& actor_id)
        {
            return false;
        }

        virtual bool CloseSession(const int64_t& conn_id) = 0;

        virtual bool IsServer() = 0;

        virtual bool Log(int severity, const char* msg) = 0;

        bool IsWorking() const
        {
            return working_;
        }

        void SetWorking(bool value)
        {
            working_ = value;
        }

        AFHeadLength GetHeadLength()
        {
            return head_len_;
        }

        void SetHeadLength(AFHeadLength len)
        {
            head_len_ = len;
        }

    protected:

        int EnCode(const AFIMsgHead* head, const char* msg, const size_t len, OUT std::string& out_data)
        {
            char head_string[AFHeadLength::CS_HEAD_LENGTH] = { 0 };
            head->encode(head_string);

            out_data.clear();
            out_data.append(head_string, GetHeadLength());
            out_data.append(msg, len);

            return head->body_length() + GetHeadLength();
        }

        int DeCode(const char* data, const size_t len, AFIMsgHead* head)
        {
            if (len < GetHeadLength())
            {
                return -1;
            }

            if (GetHeadLength() != head->decode(data))
            {
                return -2;
            }

            if (head->body_length() > (len - GetHeadLength()))
            {
                return -3;
            }

            return head->body_length();
        }

    private:
        bool working_{ false };
        AFHeadLength head_len_{ AFHeadLength::CS_HEAD_LENGTH };

    public:
        size_t statistic_recv_size_{ 0 };
        size_t statistic_send_size_{ 0 };
    };

    //////////////////////////////////////////////////////////////////////////

}