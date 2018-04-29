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
#include "AFINet.h"
#include "brynet/net/WrapTCPService.h"
#include "brynet/net/http/HttpService.h"

#pragma pack(push, 1)

class AFIBryNet : public AFINet
{
public:
    bool SplitHostPort(const std::string& strIpPort, std::string& host, int& port)
    {
        std::string a = strIpPort;
        if(a.empty())
        {
            return false;
        }

        size_t index = a.rfind(':');
        if(index == std::string::npos)
        {
            return false;
        }

        if(index == a.size() - 1)
        {
            return false;
        }

        port = std::atoi(&a[index + 1]);

        host = std::string(strIpPort, 0, index);
        if(host[0] == '[')
        {
            if(*host.rbegin() != ']')
            {
                return false;
            }

            // trim the leading '[' and trail ']'
            host = std::string(host.data() + 1, host.size() - 2);
        }

        // Compatible with "fe80::886a:49f3:20f3:add2]:80"
        if(*host.rbegin() == ']')
        {
            // trim the trail ']'
            host = std::string(host.data(), host.size() - 1);
        }

        return true;
    }
};

template <class SessionPTR>
struct BaseMsgFromNetInfo
{
    BaseMsgFromNetInfo(const SessionPTR TCPConPtr) : mTCPConPtr(TCPConPtr)
    {
        nType = None;
    }

    NetEventType nType;
    AFGUID xClientID;
    SessionPTR mTCPConPtr;
    std::string strMsg;
    AFCMsgHead xHead;
};

using MsgFromBryNetInfo = BaseMsgFromNetInfo<brynet::net::TCPSession::PTR>;
using MsgFromBryHttpNetInfo = BaseMsgFromNetInfo<brynet::net::HttpSession::PTR>;

template <class SessionPTR>
class BaseNetObject : public NetObject
{
public:
    BaseNetObject(AFINet* pNet, const AFGUID& xClientID, const SessionPTR session) : NetObject(pNet, xClientID), mBryNetConnPtr(session)
    {
    }

    virtual ~BaseNetObject()
    {
    }

    const SessionPTR& GetConnPtr()
    {
        return mBryNetConnPtr;
    }
public:
    AFLockFreeQueue<BaseMsgFromNetInfo<SessionPTR>*> mqMsgFromNet;
    SessionPTR mBryNetHttpConnPtr;
    AFGUID xHttpClientID;

private:
    const SessionPTR mBryNetConnPtr;
};

using BryNetObject = BaseNetObject<brynet::net::TCPSession::PTR>;
using BryHttpNetObject = BaseNetObject<brynet::net::HttpSession::PTR>;
#pragma pack(pop)