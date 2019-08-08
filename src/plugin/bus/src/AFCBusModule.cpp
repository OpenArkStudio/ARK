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

#include "base/AFEnum.hpp"
#include "base/AFDefine.hpp"
#include "base/AFXml.hpp"
#include "base/AFPluginManager.hpp"
#include "bus/include/AFCBusModule.hpp"

namespace ark {

bool AFCBusModule::Init()
{
    if (!LoadProcConfig())
    {
        return false;
    }

    if (!LoadBusRelation())
    {
        return false;
    }

    return true;
}

bool AFCBusModule::LoadProcConfig()
{
    static std::string proc_file = "../bus_conf/proc.xml";
    AFXml xml_doc(proc_file);

    auto root_node = xml_doc.GetRootNode();
    if (!root_node.IsValid())
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    auto host_nodes = root_node.FindNode("hosts");
    if (!host_nodes.IsValid())
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    for (auto host_node = host_nodes.FindNode("host"); host_node.IsValid(); host_node.NextNode())
    {
        std::string name = host_node.GetString("name");
        std::string intranet_ip = host_node.GetString("intranet_ip");
        proc_config_.hosts.insert(std::make_pair(name, intranet_ip));
    }

    auto server_nodes = root_node.FindNode("servers");
    if (!server_nodes.IsValid())
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    // self bus address
    AFBusAddr self_bus_id(GetSelfBusID());

    for (auto server_node = server_nodes.FindNode("server"); server_node.IsValid(); server_node.NextNode())
    {
        std::string name = server_node.GetString("name");
        ARK_APP_TYPE app_type = ARK_APP_TYPE(server_node.GetUint32("app_type"));
        std::string protocol = server_node.GetString("protocol");
        uint32_t max_connection = server_node.GetUint32("max_connection");
        uint8_t thread_num = static_cast<uint8_t>(server_node.GetUint32("thread_num"));

        std::vector<AFServerConfig> instances;
        for (auto proc_node = server_node.FindNode("proc"); proc_node.IsValid(); proc_node.NextNode())
        {
            uint8_t start = static_cast<uint8_t>(proc_node.GetUint32("start"));
            uint8_t end = static_cast<uint8_t>(proc_node.GetUint32("end"));
            std::string host_name = proc_node.GetString("host");

            auto iter = proc_config_.hosts.find(host_name);
            ARK_ASSERT_RET_VAL(iter != proc_config_.hosts.end(), false);
            const std::string& intranet_ip = iter->second;

            for (uint8_t i = start; i <= end; ++i)
            {
                AFServerConfig server_config;
                AFBusAddr server_bus(self_bus_id.channel_id, self_bus_id.zone_id, static_cast<uint8_t>(app_type), i);
                server_config.self_id = server_bus.bus_id;
                server_config.max_connection = max_connection;
                server_config.thread_num = thread_num;
                uint16_t port = CalcProcPort(server_bus);

                std::error_code ec;
                server_config.intranet_ep_ = AFEndpoint::FromString(AFSocket::MakeUrl(protocol, intranet_ip, port), ec);
                server_config.public_ep_ = AFEndpoint::FromString(AFSocket::MakeUrl(protocol, "0.0.0.0", port), ec);
                instances.emplace_back(server_config);
            }
        }

        proc_config_.instances.insert(std::make_pair(app_type, instances));
        proc_config_.app_name_types.insert(std::make_pair(name, app_type));
        proc_config_.app_type_names.insert(std::make_pair(app_type, name));
    }

    return true;
}

bool AFCBusModule::LoadBusRelation()
{
    // load bus relation files
    std::string bus_file = "../bus_conf/bus_relation.xml";
    AFXml xml_doc(bus_file);

    auto root_node = xml_doc.GetRootNode();
    if (!root_node.IsValid())
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    auto relation_nodes = root_node.FindNode("bus_relations");
    for (auto relation_node = relation_nodes.FindNode("relation"); relation_node.IsValid(); relation_node.NextNode())
    {
        std::string proc = relation_node.GetString("proc");
        std::string target_proc = relation_node.GetString("target_proc");
        ArkBusRelationType connection_type = ArkBusRelationType(relation_node.GetUint32("connect_type"));

        const ARK_APP_TYPE proc_type = GetAppType(proc);
        const ARK_APP_TYPE target_proc_type = GetAppType(target_proc);

        auto iter = bus_relations_.find(proc_type);
        if (iter != bus_relations_.end())
        {
            auto& target_process = iter->second;
            target_process.insert(std::make_pair(target_proc_type, connection_type));
        }
        else
        {
            std::map<ARK_APP_TYPE, ArkBusRelationType> target_process;
            target_process.insert(std::make_pair(target_proc_type, connection_type));

            bus_relations_.insert(std::make_pair(proc_type, target_process));
        }
    }

    return true;
}

const std::string& AFCBusModule::GetAppName(const ARK_APP_TYPE& app_type)
{
    auto iter = proc_config_.app_type_names.find(app_type);
    return ((iter != proc_config_.app_type_names.end()) ? iter->second : NULL_STR);
}

ARK_APP_TYPE AFCBusModule::GetAppType(const std::string& name)
{
    auto iter = proc_config_.app_name_types.find(name);
    return ((iter != proc_config_.app_name_types.end()) ? iter->second : ARK_APP_TYPE::ARK_APP_DEFAULT);
}

uint16_t AFCBusModule::CalcProcPort(const AFBusAddr& bus_addr)
{
    const ARK_APP_TYPE& app_type = ARK_APP_TYPE(bus_addr.app_type);
    if (app_type <= ARK_APP_TYPE::ARK_APP_DEFAULT || app_type >= ARK_APP_TYPE::ARK_APP_WORLD_MAX)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return 0;
    }

    if (app_type > ARK_APP_TYPE::ARK_APP_DEFAULT && app_type < ARK_APP_TYPE::ARK_APP_CLUSTER_MAX)
    {
        return (10000 + bus_addr.app_type * 100 + bus_addr.inst_id);
    }
    else
    {
        return (20000 + bus_addr.app_type * 100 + bus_addr.inst_id);
    }
}

const AFServerConfig* AFCBusModule::GetAppServerInfo()
{
    AFBusAddr bus_addr(GetSelfBusID());
    return GetAppServerInfo(bus_addr);
}

const AFServerConfig* AFCBusModule::GetAppServerInfo(const AFBusAddr& bus_addr)
{
    auto iter = proc_config_.instances.find(ARK_APP_TYPE(bus_addr.app_type));
    if (iter != proc_config_.instances.end())
    {
        for (auto& server : iter->second)
        {
            AFBusAddr server_bus(server.self_id);
            if (server_bus.inst_id == bus_addr.inst_id)
            {
                return &server;
            }
        }
    }

    return nullptr;
}

const std::string& AFCBusModule::GetHost(const std::string& host)
{
    auto iter = proc_config_.hosts.find(host);
    if (iter != proc_config_.hosts.end())
    {
        return iter->second;
    }
    else
    {
        return NULL_STR;
    }
}

const std::string AFCBusModule::GetAppHost(const int bus_id)
{
    AFBusAddr bus_addr(bus_id);
    const AFServerConfig* server_config = GetAppServerInfo(bus_addr);
    return (server_config == nullptr) ? NULL_STR : server_config->intranet_ep_.ToString();
}

int AFCBusModule::GetSelfBusID()
{
    return GetPluginManager()->GetBusID();
}

const std::string AFCBusModule::GetSelfBusName()
{
    AFBusAddr bus_id(GetSelfBusID());
    return bus_id.ToString();
}

ARK_APP_TYPE AFCBusModule::GetSelfAppType()
{
    AFBusAddr bus_id(GetSelfBusID());
    return ARK_APP_TYPE(bus_id.app_type);
}

const std::string AFCBusModule::GetAppWholeName(const int bus_id)
{
    AFBusAddr addr(bus_id);
    std::string name = GetAppName(ARK_APP_TYPE(addr.app_type));
    return (name + "-" + addr.ToString());
}

bool AFCBusModule::GetDirectBusRelations(std::vector<AFServerConfig>& target_list)
{
    const ARK_APP_TYPE app_type = GetSelfAppType();
    auto iter = bus_relations_.find(app_type);
    if (iter != bus_relations_.end())
    {
        for (auto it : iter->second)
        {
            if (it.second != ArkBusRelationType::BRT_DIRECT)
            {
                // undirected
                continue;
            }

            const ARK_APP_TYPE target_app_type = it.first;
            auto proc_conf_iter = proc_config_.instances.find(target_app_type);
            target_list.assign(proc_conf_iter->second.begin(), proc_conf_iter->second.end());
        }
    }

    // If someone haven't any directly connection relationship, it's OK.
    return true;
}

ArkBusRelationType AFCBusModule::GetBusRelationType(const int bus_id)
{
    if (bus_id == GetSelfBusID())
    {
        return ArkBusRelationType::BRT_UNKNOWN;
    }

    AFBusAddr target_bus(bus_id);
    const ARK_APP_TYPE app_type = GetSelfAppType();
    auto iter = bus_relations_.find(app_type);
    if (iter != bus_relations_.end())
    {
        for (auto it : iter->second)
        {
            if (it.first == ARK_APP_TYPE(target_bus.app_type))
            {
                return it.second;
            }
        }
    }

    return ArkBusRelationType::BRT_UNKNOWN;
}

int AFCBusModule::CombineBusID(const ARK_APP_TYPE app_type, const uint8_t inst_id)
{
    ARK_ASSERT_RET_VAL(app_type > ARK_APP_TYPE::ARK_APP_DEFAULT && app_type < ARK_APP_TYPE::ARK_APP_MAX, 0);

    int self_bus = GetSelfBusID();
    AFBusAddr self_bus_addr(self_bus);
    AFBusAddr target_bus_addr(self_bus_addr.channel_id, self_bus_addr.zone_id, static_cast<uint8_t>(app_type), inst_id);
    return target_bus_addr.bus_id;
}

} // namespace ark
