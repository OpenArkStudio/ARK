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

#include <brynet/net/TCPService.h>
#include <brynet/net/http/HttpService.h>
#include <brynet/net/SocketLibFunction.h>
#include <brynet/net/TCPService.h>
#include <brynet/net/Connector.h>
#include <brynet/net/SyncConnector.h>
#include "SDK/Core/AFMacros.hpp"
#include "SDK/Core/AFBuffer.hpp"
#include "SDK/Core/AFRWLock.hpp"
#include "SDK/Core/AFLockFreeQueue.hpp"
#include "SDK/Interface/AFNetMsg.h"

namespace ark
{

    template <typename SessionPTR>
    class AFNetSession
    {
    public:
        AFNetSession(AFHeadLength head_len, const int64_t& conn_id, const SessionPTR session) :
            head_len_(head_len_),
            conn_id_(conn_id),
            session_(session)
        {
        }

        virtual ~AFNetSession()
        {
            ReleaseQueueData();
        }

        void ReleaseQueueData()
        {
            AFNetEvent* event = nullptr;
            if (PopNetEvent(event))
            {
                while (event != nullptr)
                {
                    ARK_DELETE(event);
                    PopNetEvent(event)
                }
            }

            AFNetMsg* msg = nullptr;
            if (PopNetMsg(msg))
            {
                while (msg != nullptr)
                {
                    ARK_DELETE(msg);
                    PopNetMsg(msg);
                }
            }
        }

        const SessionPTR& GetSession()
        {
            return session_;
        }

        int AddBuffer(const char* data, size_t len)
        {
            buffer_.write(data, len);
            return (int)buffer_.get_length();
        }

        size_t RemoveBuffer(size_t len)
        {
            if (len > buffer_.get_length())
            {
                return 0;
            }

            buffer_.remove_data(len);
            return buffer_.get_length();
        }

        const char* GetBuffer()
        {
            return buffer_.get_data();
        }

        size_t GetBufferLen()
        {
            return buffer_.get_length();
        }

        uint32_t GetHeadLen()
        {
            return head_len_;
        }

        const int64_t& GetSessionId()
        {
            return session_id_;
        }

        void SetSessionId(const int64_t& id)
        {
            session_id_ = id;
        }

        bool NeedRemove()
        {
            return need_remove_;
        }

        void SetNeedRemove(bool value)
        {
            need_remove_ = value;
        }

        bool AddNetEvent(AFNetEvent* event)
        {
            return event_queue_.Push(event);
        }

        bool PopNetEvent(AFNetEvent* event)
        {
            return event_queue_.Pop(event);
        }

        bool AddNetMsg(AFNetMsg* msg)
        {
            return msg_queue_.Push(msg);
        }

        bool PopNetMsg(AFNetMsg* msg)
        {
            return msg_queue_.Pop(msg)
        }

        void ParseBufferToMsg()
        {
            uint32_t pos = 0;
            AFMsgHead* msg_head = CheckRecvDataValid(pos);
            if (msg_head == nullptr)
            {
                return;
            }

            while (GetBufferLen() >= pos + GetHeadLen() + msg_head->length_)
            {
                AFNetMsg* msg = AFNetMsg::AllocMsg(msg_head->length_);
                memcpy(msg, msg_head, GetHeadLen());

                pos += GetHeadLen();
                if (msg_head->length_ > 0)
                {
                    msg->CopyFrom(GetBuffer() + pos, msg_head->length_);
                    pos += msg_head->length_;
                }

                AddNetMsg(msg);

                msg_head = CheckRecvDataValid(pos);
                if (msg_head == nullptr)
                {
                    break;;
                }
            }

            //remove [0, pos] buffers
            if (pos > 0)
            {
                RemoveBuffer(pos);
            }
        }

    protected:
        AFMsgHead* CheckRecvDataValid(uint32_t pos)
        {
            if (GetBufferLen() < (pos + GetHeadLen()))
            {
                return nullptr;
            }

            auto head = reinterpret_cast<AFMsgHead*>(GetBuffer() + pos);
            ARK_ASSERT_RET_VAL(head->length_ <= ARK_MSG_MAX_LENGTH, nullptr);

            return head;
        }

    private:
        uint32_t head_len_{ 0 };
        int64_t session_id_{ 0 };
        AFGUID object_id_{ 0 };
        AFBuffer buffer_;

        //AFINet* net_ptr_{ nullptr };
        AFLockFreeQueue<AFNetMsg*> msg_queue_;
        AFLockFreeQueue<AFNetEvent*> event_queue_;
        const SessionPTR session_;

        volatile bool connected_{ false };
        volatile bool need_remove_{ false };
    };

    using AFTCPSession = AFNetSession<brynet::net::DataSocket::PTR>;
    using AFTCPSessionPtr = AFTCPSession * ;

    using AFHttpSession = AFNetSession<brynet::net::HttpSession::PTR>;
    using AFHttpSessionPtr = AFHttpSession * ;

}