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
        for (bool bRet = _net_clients.Begin(); bRet; bRet = _net_clients.Increase())
        {
            const auto& pCluster = _net_clients.GetCurrentData();
            ARK_ASSERT_CONTINUE(pCluster != nullptr);

            pCluster->Update();
        }

        return true;
    }

    bool AFCNetClientManagerModule::Shut()
    {
        for (bool bRet = _net_clients.Begin(); bRet; bRet = _net_clients.Increase())
        {
            const AFINetClientService* pCluster = _net_clients.GetCurrentData();
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
            return -2;
        }

        for (auto& target : target_list)
        {
            uint8_t app_type = AFBusAddr(target.self_id).proc_id;
            AFINetClientService* pClient = _net_clients.GetElement(app_type);
            if (pClient == nullptr)
            {
                pClient = ARK_NEW AFCNetClientService(pPluginManager);
                _net_clients.AddElement(app_type, pClient);
            }

            bool ret = pClient->StartClient(target.self_id, target.public_ep_);
            if (!ret)
            {
                ARK_LOG_ERROR("start net client failed, self_bus_id = {} target_url={}", m_pBusModule->GetSelfBusName(), target.public_ep_.to_string());
            }
        }

        return 0;
    }

    AFINetClientService* AFCNetClientManagerModule::GetNetClientService(const uint8_t& app_type)
    {
        return _net_clients.GetElement(app_type);
    }

    AFINetClientService* AFCNetClientManagerModule::GetNetClientServiceByBusID(const int bus_id)
    {
        AFBusAddr addr(bus_id);
        return GetNetClientService(addr.proc_id);
    }

}