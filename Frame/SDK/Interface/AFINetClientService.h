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
#include "SDK/Core/AFSocketFunc.hpp"
#include "AFINet.h"

namespace ark
{

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

        int server_bus_id_{ 0 };
        AFEndpoint endpoint_;
        AFINet* net_client_ptr_{ nullptr };

        ConnectState net_state_{ DISCONNECT }; //net state
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
            return AddRecvCallback(nMsgID, std::make_shared<NET_PKG_RECV_FUNCTOR>(functor));
        }

        template<typename BaseType>
        bool AddRecvCallback(BaseType* pBase, void (BaseType::*handleRecv)(const ARK_PKG_BASE_HEAD&, const int, const char*, const uint32_t, const AFGUID&))
        {
            NET_PKG_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
            return AddRecvCallback(std::make_shared<NET_PKG_RECV_FUNCTOR>(functor));
        }

        template<typename BaseType>
        bool AddEventCallBack(BaseType* pBase, void (BaseType::*handler)(const NetEventType, const AFGUID&, const std::string&, const int))
        {
            NET_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
            return AddEventCallBack(std::make_shared<NET_EVENT_FUNCTOR>(functor));
        }

        virtual bool StartClient(const int& target_bus_id, const AFEndpoint& endpoint) = 0;
        virtual void Update() = 0;
        virtual void Shutdown() = 0;

        virtual const ARK_SHARE_PTR<AFConnectionData>& GetServerNetInfo(const int nServerID) = 0;
        virtual AFMapEx<int, AFConnectionData>& GetServerList() = 0;

        virtual bool AddRecvCallback(const int nMsgID, const NET_PKG_RECV_FUNCTOR_PTR& cb) = 0;
        virtual bool AddRecvCallback(const NET_PKG_RECV_FUNCTOR_PTR& cb) = 0;
        virtual bool AddEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb) = 0;

        virtual void SendToServerByPB(const int nServerID, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID) = 0;
    };

}