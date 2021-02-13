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

#include "base/AFPlatform.hpp"
#include "base/AFNoncopyable.hpp"
#include "net/include/AFNetDefine.hpp"

namespace ark {

/**
 * @brief 协议头部接口类，通过它支持任意类型协议头部格式
 */
class AFIMsgHeader
{
public:
    virtual ~AFIMsgHeader() {}

    // 协议头部必须实现的接口如下

    // 返回包头大小
    virtual uint32_t HeaderLength() const = 0;

    // 把读取到的buffer解析成包头
    virtual void ParseFromArray(const void* head_buf) = 0;
    // 把包头序列化到string里
    virtual void SerializeToString(std::string& buf) = 0;

    virtual msg_id_t MessageId() const = 0;
    virtual uint32_t MessageLength() const = 0;

    virtual void SetMessageId(msg_id_t) = 0;
    virtual void SetMessageLength(uint32_t) = 0;

    // 新建包头对象
    virtual AFIMsgHeader* New() const = 0;
    // 包头深拷贝
    virtual AFIMsgHeader* Clone() const = 0;

    // 以下接口是为了兼容原来的代码

    virtual bool IsCompressed() const
    {
        return false;
    }

    virtual guid_t ActorId() const
    {
        return 0;
    }

    virtual bus_id_t SourceBusId() const
    {
        return 0;
    }

    virtual bus_id_t DestBusId() const
    {
        return 0;
    }

    virtual void SetCompressed(bool compress)
    {
    }

    virtual void SetActorId(guid_t)
    {
    }

    virtual void SetSourceBusId(bus_id_t)
    {
    }

    virtual void SetDestBusId(bus_id_t)
    {
    }
};

// CS通信默认包头
class AFCSMsgHeader : public AFIMsgHeader
{
public:
    // 返回包头大小
    virtual uint32_t HeaderLength() const override
    {
        return sizeof header_;
    }

    // 把读取到的buffer解析成包头
    virtual void ParseFromArray(const void* head_buf) override
    {
        const HeaderImpl* bb = reinterpret_cast<const HeaderImpl*>(head_buf);
        const char* cc = reinterpret_cast<const char*>(head_buf);
        memcpy(&header_, head_buf, sizeof header_);
    }

    // 把包头序列化到string里
    virtual void SerializeToString(std::string& buf) override
    {
        buf.append(reinterpret_cast<char*>(&header_), sizeof header_);
    }

    virtual msg_id_t MessageId() const override
    {
        return header_.id_;
    }

    virtual uint32_t MessageLength() const override
    {
        return header_.length_;
    }

    virtual void SetMessageId(msg_id_t id) override
    {
        header_.id_ = id;
    }

    virtual void SetMessageLength(uint32_t msg_len) override
    {
        header_.length_ = msg_len;
    }

    // 新建包头对象
    virtual AFIMsgHeader* New() const override
    {
        return ARK_NEW AFCSMsgHeader();
    }

    // 包头深拷贝
    virtual AFIMsgHeader* Clone() const override
    {
        return ARK_NEW AFCSMsgHeader(*this);
    }

    virtual bool IsCompressed() const override
    {
        return header_.compress_;
    }

    virtual void SetCompressed(bool compress) override
    {
        header_.compress_ = compress;
    }

private:
#pragma pack(push, 1)
    struct HeaderImpl
    {
        msg_id_t id_{0};      // Msg id
        uint32_t length_{0};  // Msg length(without header length)
        uint8_t compress_{0}; // Msg data compressed
    };
#pragma pack(pop)

private:
    HeaderImpl header_;
};

// SS通信默认包头
class AFSSMsgHeader : public AFIMsgHeader
{
public:
    // 返回包头大小
    virtual uint32_t HeaderLength() const override
    {
        return sizeof header_;
    }

    // 把读取到的buffer解析成包头
    virtual void ParseFromArray(const void* head_buf) override
    {
        memcpy(&header_, head_buf, sizeof header_);
    }

    // 把包头序列化到string里
    virtual void SerializeToString(std::string& buf) override
    {
        buf.append(reinterpret_cast<char*>(&header_), sizeof header_);
    }

    virtual msg_id_t MessageId() const override
    {
        return header_.id_;
    }

    virtual uint32_t MessageLength() const override
    {
        return header_.length_;
    }

    virtual void SetMessageId(msg_id_t id) override
    {
        header_.id_ = id;
    }

    virtual void SetMessageLength(uint32_t msg_len) override
    {
        header_.length_ = msg_len;
    }

    // 新建包头对象
    virtual AFIMsgHeader* New() const override
    {
        return ARK_NEW AFSSMsgHeader();
    }

    // 包头深拷贝
    virtual AFIMsgHeader* Clone() const override
    {
        return ARK_NEW AFSSMsgHeader(*this);
    }

    virtual bool IsCompressed() const override
    {
        return header_.compress_;
    }

    virtual guid_t ActorId() const override
    {
        return header_.actor_id_;
    }

    virtual bus_id_t SourceBusId() const override
    {
        return header_.src_bus_;
    }

    virtual bus_id_t DestBusId() const override
    {
        return header_.dst_bus_;
    }

    virtual void SetCompressed(bool compress) override
    {
        header_.compress_ = compress;
    }

    virtual void SetActorId(guid_t actor_id) override
    {
        header_.actor_id_ = actor_id;
    }

    virtual void SetSourceBusId(bus_id_t src_bus) override
    {
        header_.src_bus_ = src_bus;
    }

    virtual void SetDestBusId(bus_id_t dst_bus) override
    {
        header_.dst_bus_ = dst_bus;
    }

private:
#pragma pack(push, 1)
    struct HeaderImpl
    {
        msg_id_t id_{0};      // Msg id
        uint32_t length_{0};  // Msg length(without header length)
        uint8_t compress_{0}; // Msg data compressed
        guid_t actor_id_{0};  // Actor id
        bus_id_t src_bus_{0}; // Source bus id
        bus_id_t dst_bus_{0}; // Destination bus id
    };
#pragma pack(pop)

private:
    HeaderImpl header_;
};

class AFNetMsg final : private AFNoncopyable
{
protected:
    AFNetMsg(AFIMsgHeader* head)
    {
        head_ = head;
        msg_data_ = nullptr;
        AllocData(head_->MessageLength());
    }

    ~AFNetMsg()
    {
        delete head_;
        delete []msg_data_;
    }

public:
    static AFNetMsg* AllocMsg(AFIMsgHeader* head)
    {
        return ARK_NEW AFNetMsg(head);
    }

    static void Release(AFNetMsg*& msg)
    {
        ARK_DELETE(msg);
    }

    void CopyData(const char* data, const uint32_t len)
    {
        ARK_ASSERT_RET_NONE(data != nullptr);

        SetMsgLength(len);
        memcpy(this->msg_data_, data, len);
    }

    void CopyFrom(AFNetMsg* msg)
    {
        ARK_ASSERT_RET_NONE(msg != nullptr);

        delete head_;
        head_ = msg->head_->Clone();
        CopyData(msg->msg_data_, msg->GetMsgLength());
    }

    msg_id_t GetMsgId() const
    {
        return head_->MessageId();
    }

    uint32_t GetMsgLength() const
    {
        return head_->MessageLength();
    }

    uint8_t GetCompress() const
    {
        return head_->IsCompressed();
    }

    guid_t GetActorId() const
    {
        return head_->ActorId();
    }

    bus_id_t GetSrcBus() const
    {
        return head_->SourceBusId();
    }

    bus_id_t GetDstBus() const
    {
        return head_->DestBusId();
    }

    void SetMsgId(msg_id_t value)
    {
        head_->SetMessageId(value);
    }

    void SetMsgLength(uint32_t value)
    {
        head_->SetMessageLength(value);
    }

    void SetActorId(guid_t value)
    {
        head_->SetActorId(value);
    }

    void SetSrcBus(bus_id_t value)
    {
        head_->SetSourceBusId(value);
    }

    void SetDstBus(bus_id_t value)
    {
        head_->SetDestBusId(value);
    }

    char* GetMsgData() const
    {
        return msg_data_;
    }

    bool IsSSMsg() const
    {
        return head_->SourceBusId() != 0;
    }

    AFIMsgHeader& GetHead()
    {
        return *head_;
    }

private:
    void AllocData(uint32_t len)
    {
        if (len > 0)
        {
            msg_data_ = ARK_NEW char[len];
        }
    }

    void DeallocData()
    {
        if (msg_data_ != nullptr)
        {
            ARK_DELETE_ARRAY(char, msg_data_);
        }

        head_->SetMessageLength(0);
    }

private:
    AFIMsgHeader* head_;
    char* msg_data_{nullptr};
};

} // namespace ark
