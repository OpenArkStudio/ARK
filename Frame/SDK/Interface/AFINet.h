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

#pragma once

#include "brynet/net/TCPService.h"
#include "brynet/net/http/HttpService.h"
#include "SDK/Core/AFMacros.hpp"
#include "SDK/Core/AFGUID.hpp"
#include "SDK/Core/AFLockFreeQueue.hpp"
#include "SDK/Core/AFBuffer.hpp"
#include "SDK/Interface/AFNetMsg.h"
#include "SDK/Interface/AFNetDefine.h"

namespace ark
{

    class AFINet;

    class AFBaseNetEntity
    {
    public:
        AFBaseNetEntity(AFINet* net_ptr, const AFGUID& conn_id) :
            conn_id_(conn_id),
            net_ptr_(net_ptr),
            need_remove_(false)
        {
        }

        virtual ~AFBaseNetEntity() = default;

        int AddBuff(const char* str, size_t nLen)
        {
            buffer_.write(str, nLen);
            return (int)buffer_.getlength();
        }

        size_t RemoveBuff(size_t nLen)
        {
            if (nLen > buffer_.getlength())
            {
                return 0;
            }

            buffer_.removedata(nLen);

            return buffer_.getlength();
        }

        const char* GetBuff()
        {
            return buffer_.getdata();
        }

        size_t GetBuffLen()
        {
            return buffer_.getlength();
        }

        AFINet* GetNet()
        {
            return net_ptr_;
        }
        bool NeedRemove()
        {
            return need_remove_;
        }
        void SetNeedRemove(bool b)
        {
            need_remove_ = b;
        }
        const std::string& GetAccount() const
        {
            return user_data_;
        }

        void SetAccount(const std::string& strData)
        {
            user_data_ = strData;
        }

        const AFGUID& GetConnID()
        {
            return conn_id_;
        }

        void SetConnID(const AFGUID& conn_id)
        {
            conn_id_ = conn_id;
        }

    private:
        AFBuffer buffer_;
        std::string user_data_;
        AFGUID conn_id_;//temporary client id

        AFINet* net_ptr_;
        mutable bool need_remove_;
    };

    class AFINet
    {
    public:
        AFINet() = default;
        virtual ~AFINet() = default;

        //need to call this function every frame to drive network library
        virtual void Update() = 0;
        virtual bool Start(const int dst_busid, const std::string& ip, const int port, bool ip_v6 = false) = 0;
        virtual bool Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6 = false) = 0;

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

        virtual bool CloseNetEntity(const AFGUID& conn_id) = 0;

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

    private:
        bool working_{ false };

    public:
        size_t recv_size_{ 0 };
        size_t send_size_{ 0 };
    };

    //////////////////////////////////////////////////////////////////////////

    template <typename SessionPTR>
    class AFNetMsg
    {
    public:
        AFNetMsg(const SessionPTR session_ptr) : session_(session_ptr) {}

        NetEventType event_{ NONE };
        AFGUID conn_id_{ 0 };
        SessionPTR session_{ nullptr };
        std::string msg_data_{};
        ARK_PKG_CS_HEAD head_;
    };

    using AFTCPMsg = AFNetMsg<brynet::net::DataSocket::PTR>;
    using AFHttpMsg = AFNetMsg<brynet::net::HttpSession::PTR>;

    template <typename SessionPTR>
    class AFNetEntity : public AFBaseNetEntity
    {
    public:
        AFNetEntity(AFINet* net_ptr, const AFGUID& conn_id, const SessionPTR session) :
            AFBaseNetEntity(net_ptr, conn_id),
            session_(session)
        {
        }

        virtual ~AFNetEntity() = default;

        const SessionPTR& GetSession()
        {
            return session_;
        }

        AFLockFreeQueue<AFNetMsg<SessionPTR>*> msg_queue_;

    private:
        const SessionPTR session_;
    };

    using AFTCPEntity = AFNetEntity<brynet::net::DataSocket::PTR>;
    using AFHttpEntity = AFNetEntity<brynet::net::HttpSession::PTR>;

}