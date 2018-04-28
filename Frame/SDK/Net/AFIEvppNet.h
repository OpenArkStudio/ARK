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

#include "SDK/Core/Base/AFPlatform.hpp"
#include "SDK/Core/Base/AFMacros.hpp"
#include "SDK/Core/Base/AFGUID.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <event2/event_compat.h>
#include "evpp/tcp_callbacks.h"
#include "evpp/buffer.h"
#include "SDK/Core/Base/AFLockFreeQueue.h"
#include "AFINet.h"

#pragma pack(push, 1)

class MsgEvppFromNetInfo : public MsgFromNetInfo
{
public:
    MsgEvppFromNetInfo(const evpp::TCPConnPtr TCPConPtr) : mTCPConPtr(TCPConPtr)
    {
        nType = None;
    }
    NetEventType nType;
    AFGUID xClientID;
    evpp::TCPConnPtr mTCPConPtr;
    std::string strMsg;
    AFCMsgHead xHead;
};

class NFCEvppNetObject : public NetObject
{
public:
    NFCEvppNetObject(AFINet* pNet, const AFGUID& xClientID, const evpp::TCPConnPtr& conn): NetObject(pNet, xClientID), mConnPtr(conn)
    {
    }

    virtual ~NFCEvppNetObject()
    {
    }

    const evpp::TCPConnPtr& GetConnPtr()
    {
        return mConnPtr;
    }

public:
    AFLockFreeQueue<MsgEvppFromNetInfo*> mqMsgFromNet;
private:
    const evpp::TCPConnPtr mConnPtr;

};



#pragma pack(pop)