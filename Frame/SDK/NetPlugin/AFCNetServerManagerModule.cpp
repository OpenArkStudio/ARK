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

#include "SDK/Interface/AFIPluginManager.h"
#include "AFCNetServerManagerModule.h"
#include "AFCNetServerService.h"

namespace ark
{

    bool AFCNetServerManagerModule::Init()
    {
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();

        return true;
    }

    bool AFCNetServerManagerModule::Update()
    {
        for (bool bRet = net_servers_.Begin(); bRet; bRet = net_servers_.Increase())
        {
            const auto& pServer = net_servers_.GetCurrentData();
            if (pServer != nullptr)
            {
                pServer->Update();
            }
        }

        return true;
    }

    bool AFCNetServerManagerModule::Shut()
    {
        for (bool bRet = net_servers_.Begin(); bRet; bRet = net_servers_.Increase())
        {
            const auto* pServer = net_servers_.GetCurrentData();
            if (pServer != nullptr)
            {
                ARK_DELETE(pServer);
            }
        }

        return true;
    }

    int AFCNetServerManagerModule::CreateServer()
    {
        const AFServerConfig* server_config = m_pBusModule->GetAppServerInfo();
        if (server_config == nullptr)
        {
            ARK_LOG_ERROR("Cannot get process server info, bus id = {}", AFBusAddr(m_pBusModule->GetSelfBusID()).ToString());
            ARK_ASSERT_NO_EFFECT(0);
            return -1;
        }

        AFINetServerService* pServer = ARK_NEW AFCNetServerService(pPluginManager);
        net_servers_.AddElement(m_pBusModule->GetSelfBusID(), pServer);

        int nRet = pServer->Start(m_pBusModule->GetSelfBusID(), server_config->local_ep_, server_config->thread_num, server_config->max_connection);
        if (nRet)
        {
            ARK_LOG_INFO("Start net server successful, url = {}", server_config->local_ep_.to_string());
        }
        else
        {
            ARK_LOG_ERROR("Cannot start server net, url = {}", server_config->local_ep_.to_string());
            ARK_ASSERT_NO_EFFECT(0);
            return -2;
        }

        return 0;
    }

    AFINetServerService* AFCNetServerManagerModule::GetSelfNetServer()
    {
        return net_servers_.GetElement(m_pBusModule->GetSelfBusID());
    }

    bool AFCNetServerManagerModule::AddNetConnectionBus(int client_bus_id, AFINet* net_server_ptr)
    {
        if (client_bus_id <= 0 || net_server_ptr == nullptr)
        {
            return false;
        }

        int self_bus_id = m_pBusModule->GetSelfBusID();
        AFGUID bus_relation_id(self_bus_id, client_bus_id);
        return net_bus_relations_.AddElement(bus_relation_id, net_server_ptr);
    }

    bool AFCNetServerManagerModule::RemoveNetConnectionBus(int client_bus_id)
    {
        if (client_bus_id <= 0)
        {
            return false;
        }

        int self_bus_id = m_pBusModule->GetSelfBusID();
        AFGUID bus_relation_id(self_bus_id, client_bus_id);
        return net_bus_relations_.RemoveElement(bus_relation_id);
    }

    AFINet* AFCNetServerManagerModule::GetNetConnectionBus(int src_bus, int target_bus)
    {
        return net_bus_relations_.GetElement(AFGUID(src_bus, target_bus));
    }

}