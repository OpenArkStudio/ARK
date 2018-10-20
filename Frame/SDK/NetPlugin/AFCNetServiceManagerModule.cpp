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
#include "AFCNetServerService.h"
#include "AFCNetClientService.h"
#include "AFCNetServiceManagerModule.h"

namespace ark
{

    bool AFCNetServiceManagerModule::Init()
    {
        m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();

        return true;
    }

    bool AFCNetServiceManagerModule::Update()
    {
        net_servers_.DoEveryElement([ & ](AFMap<int, AFINetServerService>::PTRTYPE & pServerData)
        {
            if (pServerData)
            {
                pServerData->Update();
            }
            return true;
        });

        net_clients_.DoEveryElement([&](AFMap<uint8_t, AFINetClientService>::PTRTYPE & pData)
        {
            if (pData)
            {
                pData->Update();
            }
            return true;
        });

        return true;
    }

    bool AFCNetServiceManagerModule::Shut()
    {
        net_servers_.DoEveryElement([ = ](AFMap<int, AFINetServerService>::PTRTYPE & pServerData)
        {
            if (pServerData)
            {
                ARK_DELETE(pServerData);
            }
            return true;
        });

        net_clients_.DoEveryElement([&](AFMap<uint8_t, AFINetClientService>::PTRTYPE & pData)
        {
            if (pData != nullptr)
            {
                ARK_DELETE(pData);
            }
            return true;
        });

        return true;
    }

    int AFCNetServiceManagerModule::CreateServer()
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

    AFINetServerService* AFCNetServiceManagerModule::GetSelfNetServer()
    {
        return net_servers_.GetElement(m_pBusModule->GetSelfBusID());
    }

    int AFCNetServiceManagerModule::CreateClusterClients()
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

    int AFCNetServiceManagerModule::CreateClusterClient(const int bus_id, const std::string& url)
    {
        if (!m_pBusModule->IsUndirectBusRelation(bus_id))
        {
            return 0;
        }

        uint8_t app_type = AFBusAddr(bus_id).proc_id;
        AFINetClientService* pClient = net_clients_.GetElement(app_type);
        if (pClient == nullptr)
        {
            pClient = ARK_NEW AFCNetClientService(pPluginManager);
            net_clients_.AddElement(app_type, pClient);
        }

        std::error_code ec;
        AFEndpoint target_ep = AFEndpoint::from_string(url, ec);
        bool ret = pClient->StartClient(bus_id, target_ep);
        if (!ret)
        {
            ARK_LOG_ERROR("start net client failed, self_bus_id={} target_url={}", m_pBusModule->GetSelfBusName(), url);
        }
        else
        {
            return ret;
        }

        return 0;
    }

    AFINetClientService* AFCNetServiceManagerModule::GetNetClientService(const uint8_t& app_type)
    {
        return net_clients_.GetElement(app_type);
    }

    AFINetClientService* AFCNetServiceManagerModule::GetNetClientServiceByBusID(const int bus_id)
    {
        AFBusAddr addr(bus_id);
        return GetNetClientService(addr.proc_id);
    }

    bool AFCNetServiceManagerModule::AddNetConnectionBus(int client_bus_id, AFINet* net_server_ptr)
    {
        if (client_bus_id <= 0 || net_server_ptr == nullptr)
        {
            return false;
        }

        int self_bus_id = m_pBusModule->GetSelfBusID();
        AFGUID bus_relation_id(self_bus_id, client_bus_id);
        return net_bus_relations_.AddElement(bus_relation_id, net_server_ptr);
    }

    bool AFCNetServiceManagerModule::RemoveNetConnectionBus(int client_bus_id)
    {
        if (client_bus_id <= 0)
        {
            return false;
        }

        int self_bus_id = m_pBusModule->GetSelfBusID();
        AFGUID bus_relation_id(self_bus_id, client_bus_id);
        return net_bus_relations_.RemoveElement(bus_relation_id);
    }

    AFINet* AFCNetServiceManagerModule::GetNetConnectionBus(int src_bus, int target_bus)
    {
        return net_bus_relations_.GetElement(AFGUID(src_bus, target_bus));
    }

}