/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "interface/AFIModule.hpp"
#include "net/interface/AFINetServerService.hpp"
#include "net/interface/AFINetClientService.hpp"

namespace ark {

class AFINetServiceManagerModule : public AFIModule
{
public:
    // server-side net service
    virtual bool CreateServer(const size_t silent_timeout = std::chrono::milliseconds(ARK_NET_HEART_TIME).count(), 
        const AFIMsgHeader& csReqHeader = AFCSMsgHeader(), const AFIMsgHeader& ssReqHeader = AFSSMsgHeader()) = 0;
    virtual std::shared_ptr<AFINetServerService> GetInterNetServer() = 0;
    virtual std::shared_ptr<AFINetServerService> GetIntraNetServer() = 0;

    // client
    //virtual int CreateClusterClients(const AFHeaderLen head_len = AFHeaderLen::SS_HEAD_LENGTH) = 0;
    //virtual int CreateClusterClient(const AFHeaderLen head_len, bus_id_t bus_id, const std::string& url) = 0;

    //virtual AFINetClientService* GetNetClientService(const ARK_APP_TYPE& app_type) = 0;
    //virtual AFINetClientService* GetNetClientServiceByBusID(bus_id_t bus_id) = 0;

    virtual bool AddNetConnectionBus(bus_id_t target_bus_id, std::shared_ptr<AFINet> net_ptr) = 0;
    virtual bool RemoveNetConnectionBus(bus_id_t target_bus_id) = 0;
    virtual std::shared_ptr<AFINet> GetNetConnectionBus(bus_id_t src_bus, bus_id_t target_bus) = 0;

    virtual std::shared_ptr<AFINetClientService> GetClientService(const ARK_APP_TYPE app_type) = 0;

    virtual conv_id_t GetSessionID(bus_id_t client_bus_id) = 0;
    virtual bool RemoveSessionID(conv_id_t session_id) = 0;

    virtual bool CreateClientService(const AFBusAddr& bus_addr, proto_type proto, const std::string& ip, uint16_t port,
        std::shared_ptr<const AFIMsgHeader> head = std::shared_ptr<const AFIMsgHeader>(new AFSSMsgHeader)) = 0;
};

} // namespace ark
