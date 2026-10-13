/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#pragma once

#include <atomic>
#include <memory>

#include <arknet/http/http_session.hpp>
#include <arknet/rudp/rudp_session.hpp>
#include <arknet/tcp/tcp_session.hpp>
#include <arknet/websocket/ws_session.hpp>

#include "base/AFMacros.hpp"
#include "base/AFNoncopyable.hpp"
#include "base/AFBuffer.hpp"
#include "base/lock/AFRWLock.hpp"
#include "base/container/AFLockFreeQueue.hpp"
#include "net/include/AFNetMsg.hpp"
#include "net/include/AFNetDefine.hpp"
#include "net/include/AFNetEvent.hpp"

namespace ark {

template<typename SessionPTR, bool isSession>
class AFNetSession : public AFNoncopyable
{
public:
    explicit AFNetSession(
        std::shared_ptr<const AFIMsgHeader> req_head, conv_id_t session_id, const SessionPTR session_ptr = nullptr)
        : req_header_(req_head)
        , session_id_(session_id)
        , session_(session_ptr)
    {
    }

    virtual ~AFNetSession()
    {
        ReleaseQueueData();
    }

    void ReleaseQueueData()
    {
        AFNetEvent* event = nullptr;
        while (PopNetEvent(event))
        {
            AFNetEvent::Release(event);
        }

        AFNetMsg* msg = nullptr;
        while (PopNetMsg(msg))
        {
            AFNetMsg::Release(msg);
        }
    }

    const SessionPTR GetSession()
    {
        if constexpr (isSession)
        {
            ARK_ASSERT_RET_VAL(session_ != nullptr, nullptr);
            return session_;
        }
        else
        {
            ARK_ASSERT_NO_EFFECT(0);
            return nullptr;
        }
    }

    int AddBuffer(const char* data, size_t len)
    {
        buffer_.write(data, len);
        return (int)buffer_.get_length();
    }

    int AddBuffer(std::string const& data)
    {
        buffer_.write(data.data(), data.length());
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

    char* GetBuffer()
    {
        return buffer_.get_data();
    }

    size_t GetBufferLen()
    {
        return buffer_.get_length();
    }

    uint32_t GetHeadLen()
    {
        return req_header_->HeaderLength();
    }

    conv_id_t const& GetSessionId()
    {
        return session_id_;
    }

    void SetSessionId(conv_id_t const& id)
    {
        session_id_ = id;
    }

    bool NeedRemove()
    {
        return need_remove_.load();
    }

    void NeedRemove(bool value)
    {
        need_remove_.store(value);
    }

    bool AddNetEvent(AFNetEvent*& event)
    {
        return event_queue_.Push(event);
    }

    bool PopNetEvent(AFNetEvent*& event)
    {
        return event_queue_.Pop(event);
    }

    bool AddNetMsg(AFNetMsg*& msg)
    {
        return msg_queue_.Push(msg);
    }

    bool PopNetMsg(AFNetMsg*& msg)
    {
        return msg_queue_.Pop(msg);
    }

    bool ParseBufferToMsg()
    {
        size_t pos = 0;
        const size_t head_len = GetHeadLen();
        while (GetBufferLen() - pos >= head_len)
        {
            std::unique_ptr<AFIMsgHeader> msg_head(req_header_->New());
            msg_head->ParseFromArray(GetBuffer() + pos);
            const size_t msg_len = msg_head->MessageLength();
            if (msg_len > ARK_MSG_MAX_LENGTH)
            {
                return false;
            }
            if (GetBufferLen() - pos - head_len < msg_len)
            {
                break;
            }

            AFNetMsg* msg = AFNetMsg::AllocMsg(msg_head.release());
            pos += head_len;
            if (msg_len > 0)
            {
                msg->CopyData(GetBuffer() + pos, msg_len);
                pos += msg_len;
            }

            if (!AddNetMsg(msg))
            {
                AFNetMsg::Release(msg);
                return false;
            }
        }

        // remove [0, pos] buffers
        if (pos > 0)
        {
            RemoveBuffer(pos);
        }
        return true;
    }

private:
    std::shared_ptr<const AFIMsgHeader> req_header_;
    conv_id_t session_id_{0};
    AFBuffer buffer_;

    AFLockFreeQueue<AFNetMsg*> msg_queue_;
    AFLockFreeQueue<AFNetEvent*> event_queue_;
    // this field is meaning when isSession is true
    const SessionPTR session_{nullptr};

    std::atomic<bool> need_remove_{false};
};

/// server-side
using AFTCPSession = AFNetSession<std::shared_ptr<arknet::tcp_session>, true>;
using AFTCPSessionPtr = std::shared_ptr<AFTCPSession>;

using AFUDPSession = AFNetSession<std::shared_ptr<arknet::rudp_session>, true>;
using AFUDPSessionPtr = std::shared_ptr<AFUDPSession>;

using AFWSSession = AFNetSession<std::shared_ptr<arknet::ws_session>, true>;
using AFWSSessionPtr = std::shared_ptr<AFWSSession>;

using AFHttpSession = AFNetSession<std::shared_ptr<arknet::http_session>, true>;
using AFHttpSessionPtr = std::shared_ptr<AFHttpSession>;

/// client-side
using AFTCPClientSession = AFNetSession<std::shared_ptr<arknet::tcp_session>, false>;
using AFTCPClientSessionPtr = std::shared_ptr<AFTCPClientSession>;

using AFRDPClientSession = AFNetSession<std::shared_ptr<arknet::rudp_session>, false>;
using AFRDPClientSessionPtr = std::shared_ptr<AFRDPClientSession>;

using AFWSClientSession = AFNetSession<std::shared_ptr<arknet::ws_session>, false>;
using AFWSClientSessionPtr = std::shared_ptr<AFWSClientSession>;

using AFHttpClientSession = AFNetSession<std::shared_ptr<arknet::http_session>, false>;
using AFHttpClientSessionPtr = std::shared_ptr<AFHttpClientSession>;

} // namespace ark
