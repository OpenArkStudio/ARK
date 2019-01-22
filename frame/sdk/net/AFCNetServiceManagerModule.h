/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "base/AFMap.hpp"
#include "interface/AFIBusModule.h"
#include "interface/AFILogModule.h"
#include "interface/AFINetServiceManagerModule.h"
#include "interface/AFINetServerService.h"

namespace ark
{

    class AFCNetServiceManagerModule : public AFINetServiceManagerModule
    {
    public:
        explicit AFCNetServiceManagerModule() = default;

        bool Init() override;
        bool Update() override;
        bool Shut() override;

        int CreateServer(const AFHeadLength head_len = AFHeadLength::SS_HEAD_LENGTH) override;
        AFINetServerService* GetSelfNetServer() override;

        int CreateClusterClients(const AFHeadLength head_len = AFHeadLength::SS_HEAD_LENGTH) override;
        int CreateClusterClient(const AFHeadLength head_len, const int bus_id, const std::string& url) override;

        AFINetClientService* GetNetClientService(const uint8_t& app_type) override;
        AFINetClientService* GetNetClientServiceByBusID(const int bus_id) override;


        bool AddNetConnectionBus(int client_bus_id, AFINet* net_server_ptr) override;
        bool RemoveNetConnectionBus(int client_bus_id) override;
        AFINet* GetNetConnectionBus(int src_bus, int target_bus) override;

    private:
        AFMap<int, AFINetServerService> net_servers_;
        AFMap<uint8_t, AFINetClientService> net_clients_;

        AFMap<std::pair<int, int>, AFINet> net_bus_relations_;

        AFIBusModule* m_pBusModule;
        AFILogModule* m_pLogModule;
    };

}