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

#include "SDK/Core/AFMap.hpp"
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFINetServerManagerModule.h"

class AFCNetServer : public AFINetServer
{
protected:
    AFCNetServer() = default;

public:
    explicit AFCNetServer(AFIPluginManager* p);
    virtual ~AFCNetServer();

    int Start(const int bus_id, const std::string& url, const uint8_t thread_count, const uint32_t max_connection) override;
    bool Update() override;

    bool SendBroadcastMsg(const int msg_id, const std::string& msg, const AFGUID& player_id) override;
    bool SendBroadcastPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& player_id) override;
    bool SendPBMsg(const uint16_t msg_id, const google::protobuf::Message& pb_msg, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list = nullptr) override;
    bool SendMsg(const uint16_t msg_id, const std::string& data, const AFGUID& connect_id, const AFGUID& player_id, const std::vector<AFGUID>* target_list = nullptr) override;
    AFINet* GetNet() override;

    bool AddRecvCallback(const int nMsgID, const NET_RECV_FUNCTOR_PTR& cb) override;
    bool AddRecvCallback(const NET_RECV_FUNCTOR_PTR& cb) override;
    bool AddEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb) override;

protected:
    void OnRecvNetPack(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID);

    void OnSocketNetEvent(const NetEventType eEvent, const AFGUID& xClientID, int nServerID);

private:
    AFINet* m_pNet{ nullptr };
    int64_t nLastTime{ 0 };
    int64_t nServerID{ 0 };

    std::map<int, NET_RECV_FUNCTOR_PTR> mxRecvCallBack;
    std::list<NET_EVENT_FUNCTOR_PTR> mxEventCallBackList;
    std::list<NET_RECV_FUNCTOR_PTR> mxCallBackList;
};

class AFCNetServerManagerModule : public AFINetServerManagerModule
{
public:
    explicit AFCNetServerManagerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    bool Init() override;
    bool Update() override;

    int CreateServer() override;
    AFINetServer* GetSelfNetServer() override;

private:
    AFMap<int, AFINetServer> _net_servers;

    AFIBusModule* m_pBusModule;
    AFILogModule* m_pLogModule;
};