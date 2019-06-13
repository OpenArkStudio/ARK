/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
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
#include "interface/AFINetServiceManagerModule.h"
#include "interface/AFIDirNetModule.h"

namespace ark
{

    class AFCDirNetModule : public AFIDirNetModule
    {
    public:
        AFCDirNetModule() = default;

        bool Init() override;
        bool PostInit() override;
        bool PreUpdate() override;

        virtual AFINetServerService* GetNetServer();

    protected:
        int StartServer();
        int StartClient();
        //void OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id);

        //void OnClientConnected(const AFGUID& conn_id);
        //void OnClientDisconnect(const AFGUID& conn_id);

    private:
        AFILogModule* m_pLogModule;
        AFIBusModule* m_pBusModule;
        AFINetServiceManagerModule* m_pNetServiceManagerModule;

        AFINetServerService* m_pNetServer;
    };

}