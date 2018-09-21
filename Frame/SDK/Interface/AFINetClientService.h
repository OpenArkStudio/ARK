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

#include "Common/AFProtoCPP.hpp"
#include "SDK/Core/AFMap.hpp"
#include "SDK/Core/AFCConsistentHash.hpp"
#include "SDK/Core/AFNoncopyable.hpp"
#include "AFINet.h"

class AFConnectionData
{
public:
    enum ConnectState
    {
        DISCONNECT,
        CONNECTING,
        CONNECTED,
        RECONNECT,
    };

    AFConnectionData() = default;

    int _server_bus_id{ 0 };
    std::string _protocol{};
    std::string _ip{};
    uint16_t _port{ 0 };
    bool _is_ip_v6{ false };
    AFINet* _net_client_ptr{ nullptr };

    ConnectState _net_state{ DISCONNECT }; //net state
    int64_t _last_active_time{ 0 };
};

class AFINetClientService : public AFNoncopyable
{
public:
    virtual ~AFINetClientService() = default;

    template<typename BaseType>
    bool AddRecvCallback(const int nMsgID, BaseType* pBase, void (BaseType::*handleRecv)(const ARK_PKG_BASE_HEAD&, const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_PKG_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        NET_PKG_RECV_FUNCTOR_PTR functorPtr = std::make_shared<NET_PKG_RECV_FUNCTOR>(functor);

        return AddRecvCallback(nMsgID, functorPtr);
    }

    template<typename BaseType>
    bool AddRecvCallback(BaseType* pBase, void (BaseType::*handleRecv)(const ARK_PKG_BASE_HEAD&, const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_PKG_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        NET_PKG_RECV_FUNCTOR_PTR functorPtr = std::make_shared<NET_PKG_RECV_FUNCTOR>(functor);

        return AddRecvCallback(functorPtr);
    }

    template<typename BaseType>
    bool AddEventCallBack(BaseType* pBase, void (BaseType::*handler)(const NetEventType, const AFGUID&, const int))
    {
        NET_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        NET_EVENT_FUNCTOR_PTR functorPtr(new NET_EVENT_FUNCTOR(functor));

        return AddEventCallBack(functorPtr);
    }

    virtual int StartClient(const int& target_bus_id, const std::string& url) = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;

    virtual const ARK_SHARE_PTR<AFConnectionData>& GetServerNetInfo(const int nServerID) = 0;
    virtual AFMapEx<int, AFConnectionData>& GetServerList() = 0;

    virtual bool AddRecvCallback(const int nMsgID, const NET_PKG_RECV_FUNCTOR_PTR& cb) = 0;
    virtual bool AddRecvCallback(const NET_PKG_RECV_FUNCTOR_PTR& cb) = 0;
    virtual bool AddEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual void SendToServerByPB(const int nServerID, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID) = 0;
};