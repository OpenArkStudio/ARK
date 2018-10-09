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

#include "SDK/Core/AFDateTime.hpp"
#include "AFCNetClientManagerModule.h"
#include "AFCNetClientService.h"

namespace ark
{

    bool AFCNetClientManagerModule::Init()
    {
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();

        return true;
    }

    bool AFCNetClientManagerModule::Update()
    {
        for (bool bRet = net_clients_.Begin(); bRet; bRet = net_clients_.Increase())
        {
            const auto& pCluster = net_clients_.GetCurrentData();
            ARK_ASSERT_CONTINUE(pCluster != nullptr);

            pCluster->Update();
        }

        return true;
    }

    bool AFCNetClientManagerModule::Shut()
    {
        for (bool bRet = net_clients_.Begin(); bRet; bRet = net_clients_.Increase())
        {
            const AFINetClientService* pCluster = net_clients_.GetCurrentData();
            ARK_DELETE(pCluster);
        }

        return true;
    }

    int AFCNetClientManagerModule::CreateClusterClients()
    {
        std::vector<AFServerConfig> target_list;
        if (!m_pBusModule->GetDirectBusRelations(target_list))
        {
            return -1;
        }

        if (target_list.empty())
        {
            return 0;
        }

        for (auto& target : target_list)
        {
            uint8_t app_type = AFBusAddr(target.self_id).proc_id;
            AFINetClientService* pClient = net_clients_.GetElement(app_type);
            if (pClient == nullptr)
            {
                pClient = ARK_NEW AFCNetClientService(pPluginManager);
                net_clients_.AddElement(app_type, pClient);
            }

            bool ret = pClient->StartClient(target.self_id, target.public_ep_);
            if (!ret)
            {
                ARK_LOG_ERROR("start net client failed, self_bus_id={} target_url={}", m_pBusModule->GetSelfBusName(), target.public_ep_.to_string());
            }
        }

        return 0;
    }

    AFINetClientService* AFCNetClientManagerModule::GetNetClientService(const uint8_t& app_type)
    {
        return net_clients_.GetElement(app_type);
    }

    AFINetClientService* AFCNetClientManagerModule::GetNetClientServiceByBusID(const int bus_id)
    {
        AFBusAddr addr(bus_id);
        return GetNetClientService(addr.proc_id);
    }

    bool AFCNetClientManagerModule::AddNetConnectionBus(const int server_bus_id, AFINet* net_client_ptr)
    {
        if (server_bus_id <= 0 || net_client_ptr == nullptr)
        {
            return false;
        }

        int self_bus_id = m_pBusModule->GetSelfBusID();
        AFGUID bus_relation_id(self_bus_id, server_bus_id);
        return net_bus_relations_.AddElement(bus_relation_id, net_client_ptr);
    }

    bool AFCNetClientManagerModule::RemoveNetConnectionBus(const int server_bus_id)
    {
        if (server_bus_id <= 0)
        {
            return false;
        }

        int self_bus_id = m_pBusModule->GetSelfBusID();
        AFGUID bus_relation_id(self_bus_id, server_bus_id);
        return net_bus_relations_.RemoveElement(bus_relation_id);
    }

    AFINet* AFCNetClientManagerModule::GetNetConnectionBus(const int src_bus, const int target_bus)
    {
        return net_bus_relations_.GetElement(AFGUID(src_bus, target_bus));
    }

}