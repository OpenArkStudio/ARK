/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "SDK/Interface/AFIModule.h"
#include "AFINetServerModule.h"

class AFServerData
{
public:
    AFServerData()
    {
        pData = std::make_shared<AFMsg::ServerInfoReport>();
    }

    ~AFServerData() = default;

    void Init(const AFGUID& xClientID, const AFMsg::ServerInfoReport& xData)
    {
        xClient = xClientID;
        *(pData) = xData;
    }

    AFGUID xClient{ 0 };
    ARK_SHARE_PTR<AFMsg::ServerInfoReport> pData{ nullptr };
};

class AFSessionData
{
public:
    AFSessionData() = default;

    int32_t mnLogicState{ 0 };
    int32_t mnGameID{ 0 };
    AFGUID mnUserID{ 0 };
    AFGUID mnClientID{ 0 };
    AFGUID mnHashIdentID{ 0 };
    std::string mstrAccout{ "" };
};

class AFINetServer
{
public:
    virtual ~AFINetServer() = default;

    template<typename BaseType>
    bool AddRecvCallback(const int nMsgID, BaseType* pBase, void (BaseType::*handleRecv)(const AFIMsgHead&, const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        NET_RECV_FUNCTOR_PTR functorPtr = std::make_shared<NET_RECV_FUNCTOR>(functor);

        return AddRecvCallback(nMsgID, functorPtr);
    }

    template<typename BaseType>
    bool AddRecvCallback(BaseType* pBase, void (BaseType::*handleRecv)(const AFIMsgHead&, const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        NET_RECV_FUNCTOR_PTR functorPtr = std::make_shared < NET_RECV_FUNCTOR>(functor);

        return AddRecvCallback(functorPtr);
    }

    template<typename BaseType>
    bool AddEventCallBack(BaseType* pBase, void (BaseType::*handler)(const NetEventType, const AFGUID&, const int))
    {
        NET_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        NET_EVENT_FUNCTOR_PTR functorPtr(new NET_EVENT_FUNCTOR(functor));

        return AddEventCallBack(functorPtr);
    }

    virtual int Start(const int bus_id, const std::string& url, const uint8_t thread_count, const uint32_t max_connection) = 0;
    virtual bool Update() = 0;

    virtual bool SendBroadcastMsg(const int nMsgID, const std::string& msg, const AFGUID& player_id) = 0;
    virtual bool SendBroadcastPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& player_id) = 0;
    virtual bool SendPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list = nullptr) = 0;
    virtual bool SendMsg(const uint16_t msg_id, const std::string& data, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list = nullptr) = 0;
    virtual AFINet* GetNet() = 0;

    virtual bool AddRecvCallback(const int nMsgID, const NET_RECV_FUNCTOR_PTR& cb) = 0;
    virtual bool AddRecvCallback(const NET_RECV_FUNCTOR_PTR& cb) = 0;
    virtual bool AddEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb) = 0;
};

//////////////////////////////////////////////////////////////////////////

class AFINetServerManagerModule : public AFIModule
{
public:
    virtual int CreateServer() = 0;
    virtual AFINetServer* GetSelfNetServer() = 0;
};