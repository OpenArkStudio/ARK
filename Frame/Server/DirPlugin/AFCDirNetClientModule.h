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
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFINetClientManagerModule.h"
#include "SDK/Interface/AFINetServerManagerModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "Server/Interface/AFIDirNetClientModule.h"
#include "Server/Interface/AFIDirNetServerModule.h"

namespace ark
{

    class AFCDirNetClientModule : public AFIDirNetClientModule
    {
    public:
        explicit AFCDirNetClientModule() = default;

        bool Init() override;
        bool PreUpdate() override;

    protected:
        int StartClient();

        void Register(const int bus_id);
        void OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id);

        void OnClientConnected(const AFGUID& xClientID);
        void OnClientDisconnect(const AFGUID& xClientID);


        void OnRecvServerNotify(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id);
        void InvalidMessage(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id);

    private:
        AFILogModule* m_pLogModule;
        AFIBusModule* m_pBusModule;
        AFIMsgModule* m_pMsgModule;
        AFIDirNetServerModule* m_pDirNetServerModule;
        AFINetClientManagerModule* m_pNetClientManagerModule;

        std::map<std::pair<uint8_t, uint8_t>, std::map<int, AFMsg::msg_ss_server_report>> proxy_server_infos_; //<<channel, zone>, <bus_id, msg_ss_server_report>>
    };

}