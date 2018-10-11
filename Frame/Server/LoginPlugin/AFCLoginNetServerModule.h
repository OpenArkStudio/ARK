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
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFINetServerManagerModule.h"
#include "Server/Interface/AFILoginNetClientModule.h"
#include "Server/Interface/AFILoginNetServerModule.h"

namespace ark
{

    class AFCLoginNetServerModule : public AFILoginNetServerModule
    {
    public:
        explicit AFCLoginNetServerModule() = default;

        bool Init() override;
        bool PostInit() override;

        int OnSelectWorldResultsProcess(const int nWorldID, const AFGUID xSenderID, const int nLoginID, const std::string& strAccount, const std::string& strWorldURL, const std::string& strKey) override;

    protected:
        int StartServer();

        void OnSocketClientEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip,  int bus_id);

        void OnClientDisconnect(const AFGUID& xClientID);
        void OnClientConnected(const AFGUID& xClientID);

        //登入
        void OnLoginProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        //选择大世界
        void OnSelectWorldProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        //申请查看世界列表
        void OnViewWorldProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        void OnHeartBeat(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void OnLogOut(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void InvalidMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        void SynWorldToClient(const AFGUID& xClientID);

    private:
        AFMapEx<AFGUID, AFSessionData> mmClientSessionData;

        AFILoginNetClientModule* m_pLoginNetClientModule;
        AFILogModule* m_pLogModule;
        AFIBusModule* m_pBusModule;
        AFINetServerManagerModule* m_pNetServerManagerModule;

        AFINetServerService* m_pNetServer;
    };

}