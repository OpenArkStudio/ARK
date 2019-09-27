/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include <ananas/future/future.h>

#include "interface/AFIModule.hpp"
#include "net/interface/AFINetServerService.hpp"
#include "net/interface/AFINetClientService.hpp"

namespace ark {

class AFINetServiceManagerModule : public AFIModule
{
public:
    // server-side net service
    virtual ananas::Future<std::pair<bool, std::string>> CreateServer(
        const AFHeadLength head_len = AFHeadLength::SS_HEAD_LENGTH) = 0;
    virtual std::shared_ptr<AFINetServerService> GetSelfNetServer() = 0;

    // client
    //virtual int CreateClusterClients(const AFHeadLength head_len = AFHeadLength::SS_HEAD_LENGTH) = 0;
    //virtual int CreateClusterClient(const AFHeadLength head_len, const int bus_id, const std::string& url) = 0;

    //virtual AFINetClientService* GetNetClientService(const ARK_APP_TYPE& app_type) = 0;
    //virtual AFINetClientService* GetNetClientServiceByBusID(const int bus_id) = 0;

    virtual bool AddNetConnectionBus(int client_bus_id, AFINet* net_server_ptr) = 0;
    virtual bool RemoveNetConnectionBus(int client_bus_id) = 0;
    virtual AFINet* GetNetConnectionBus(int src_bus, int target_bus) = 0;
};

} // namespace ark
