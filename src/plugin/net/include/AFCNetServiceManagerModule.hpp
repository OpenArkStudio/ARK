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

#include "base/AFMap.hpp"
#include "base/AFPluginManager.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "log/interface/AFILogModule.hpp"
#include "consul/interface/AFIConsulModule.hpp"
#include "utility/interface/AFITimerModule.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "net/interface/AFINetServerService.hpp"
#include "net/interface/AFINetClientService.hpp"

namespace ark {

class AFCNetServiceManagerModule : public AFINetServiceManagerModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool PostInit() override;
    bool Update() override;
    bool Shut() override;

    ananas::Future<std::pair<bool, std::string>> CreateServer(
        const AFHeadLength head_len = AFHeadLength::SS_HEAD_LENGTH) override;

    std::shared_ptr<AFINetServerService> GetSelfNetServer() override;

    //int CreateClusterClients(const AFHeadLength head_len = AFHeadLength::SS_HEAD_LENGTH) override;
    //int CreateClusterClient(const AFHeadLength head_len, const int bus_id, const std::string& url) override;

    //AFINetClientService* GetNetClientService(const ARK_APP_TYPE& app_type) override;
    //AFINetClientService* GetNetClientServiceByBusID(const int bus_id) override;

    bool AddNetConnectionBus(int client_bus_id, AFINet* net_server_ptr) override;
    bool RemoveNetConnectionBus(int client_bus_id) override;
    AFINet* GetNetConnectionBus(int src_bus, int target_bus) override;

protected:
    ananas::Future<std::pair<bool, std::string>> RegisterToConsul(const int bus_id);
    int DeregisterFromConsul(const int bus_id);

    void HealthCheck(const std::string& name, const AFGUID& entity_id);

    bool CreateClientService(const AFBusAddr& bus_addr, const std::string& ip, uint16_t port);

    bool CheckConnectedTargetServer(const AFBusAddr& bus_addr);
    std::shared_ptr<AFConnectionData> GetTargetClientConnection(const AFBusAddr& bus_addr);

private:
    // bus_id -> net_server_service - support multi-server in the same app
    AFSmartPtrMap<int, AFINetServerService> server_services_;

    // target_app_type -> net_client_service
    // the NetClientService have target multi-client info.
    AFSmartPtrMap<ARK_APP_TYPE, AFINetClientService> client_services_;

    // All net relations, for finding AFINet
    AFMap<std::pair<int, int>, AFINet> net_bus_relations_;

    AFIBusModule* m_pBusModule;
    AFILogModule* m_pLogModule;
    AFIConsulModule* m_pConsulModule;
    AFITimerModule* m_pTimerModule;
};

} // namespace ark
