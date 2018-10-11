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
#include "SDK/Interface/AFINetServerManagerModule.h"
#include "Server/Interface/AFIDirNetServerModule.h"

namespace ark
{

    class AFCDirNetServerModule : public AFIDirNetServerModule
    {
    public:
        AFCDirNetServerModule() = default;

        bool Init() override;
        bool PostInit() override;

        virtual AFINetServerService* GetNetServer();

    protected:
        int StartServer();
        void OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id);


        void OnClientConnected(const AFGUID& conn_id);
        void OnClientDisconnect(const AFGUID& conn_id);

    private:
        AFILogModule* m_pLogModule;
        AFIBusModule* m_pBusModule;
        AFINetServerManagerModule* m_pNetServerManagerModule;

        AFINetServerService* m_pNetServer;
    };

}