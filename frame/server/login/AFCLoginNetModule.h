/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
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

#include "base/AFProtoCPP.hpp"
#include "interface/AFILogModule.h"
#include "interface/AFIMsgModule.h"
#include "interface/AFIBusModule.h"
#include "interface/AFIPluginManager.h"
#include "interface/AFINetServiceManagerModule.h"
#include "interface/AFILoginNetModule.h"

namespace ark
{

    class AFCLoginNetModule : public AFILoginNetModule
    {
    public:
        explicit AFCLoginNetModule() = default;

        bool Init() override;
        bool PostInit() override;
        bool PreUpdate() override;

        //int OnSelectWorldResultsProcess(const int nWorldID, const AFGUID xSenderID, const int nLoginID, const std::string& strAccount, const std::string& strWorldURL, const std::string& strKey) override;

    protected:
        int StartServer();
        int StartClient();

        AFMapEx<int, AFMsg::ServerInfoReport>& GetWorldMap();

        //void OnSocketClientEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip,  int bus_id);

        //void OnClientDisconnect(const AFGUID& xClientID);
        //void OnClientConnected(const AFGUID& xClientID);

        ////登入
        //void OnLoginProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        ////选择大世界
        //void OnSelectWorldProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        ////申请查看世界列表
        //void OnViewWorldProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        //void OnHeartBeat(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        //void OnLogOut(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        ////void InvalidMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        //void SynWorldToClient(const AFGUID& xClientID);

    private:
        AFMapEx<AFGUID, AFClientConnectionData> mmClientSessionData;

        AFILogModule* m_pLogModule;
        AFIBusModule* m_pBusModule;
        AFINetServiceManagerModule* m_pNetServiceManagerModule;

        AFINetServerService* m_pNetServer;
    };

}