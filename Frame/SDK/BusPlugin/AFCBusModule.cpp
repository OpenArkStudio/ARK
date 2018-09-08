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

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "SDK/Interface/AFIPluginManager.h"
#include "AFCBusModule.h"

AFCBusModule::AFCBusModule(AFIPluginManager* p)
{
    pPluginManager = p;
    LoadProcConfig();
    LoadBusRelation();
}

bool AFCBusModule::LoadProcConfig()
{
    std::string proc_file_path = "bus/proc.xml";

    std::string proc_file = pPluginManager->GetResPath() + proc_file_path;
    rapidxml::file<> xFileSource(proc_file.c_str());
    rapidxml::xml_document<> xFileDoc;
    xFileDoc.parse<0>(xFileSource.data());

    rapidxml::xml_node<>* pRoot = xFileDoc.first_node();
    if (pRoot == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    rapidxml::xml_node<>* pHostsNode = pRoot->first_node("hosts");
    if (pHostsNode == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    for (rapidxml::xml_node<>* pHostNode = pHostsNode->first_node(); pHostNode != nullptr; pHostNode = pHostNode->next_sibling())
    {
        std::string name = pHostNode->first_attribute("name")->value();
        std::string private_ip = pHostNode->first_attribute("private_ip")->value();
        mxProcConfig.hosts.insert(std::make_pair(name, private_ip));
    }

    rapidxml::xml_node<>* pServersNode = pRoot->first_node("servers");
    if (pServersNode == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    for (rapidxml::xml_node<>* pServerNode = pServersNode->first_node(); pServerNode != nullptr; pServerNode = pServerNode->next_sibling())
    {
        std::string name = pServerNode->first_attribute("name")->value();
        uint8_t proc_id = uint8_t(ARK_LEXICAL_CAST<int>(pServerNode->first_attribute("proc_id")->value()));
        std::string protocol = pServerNode->first_attribute("protocol")->value();
        int max_connection = ARK_LEXICAL_CAST<int>(pServerNode->first_attribute("max_connection")->value());
        uint8_t thread_num = ARK_LEXICAL_CAST<int>(pServerNode->first_attribute("thread_num")->value());

        std::vector<AFServerConfig> instances;
        for (rapidxml::xml_node<>* pProcNode = pServerNode->first_node(); pProcNode != nullptr; pProcNode = pProcNode->next_sibling())
        {
            uint8_t start = ARK_LEXICAL_CAST<int>(pProcNode->first_attribute("start")->value());
            uint8_t end = ARK_LEXICAL_CAST<int>(pProcNode->first_attribute("end")->value());
            std::string host_name = pProcNode->first_attribute("host")->value();

            for (uint8_t i = start; i <= end; ++i)
            {
                AFServerConfig server_config;
                server_config.inst_id = i;
                server_config.max_connection = max_connection;
                server_config.thread_num = thread_num;
                const std::string& real_host = GetHost(host_name);
                ARK_ASSERT_RET_VAL(real_host != NULL_STR, false);
                uint16_t port = CalcProcPort(proc_id, server_config.inst_id);
                server_config.private_url = protocol + "0.0.0.0:" + ARK_TO_STRING(port);
                server_config.public_url = protocol + real_host + ":" + ARK_TO_STRING(port);
                instances.emplace_back(server_config);
            }
        }

        mxProcConfig.instances.insert(std::make_pair(proc_id, instances));
        mxProcConfig.proc_types.insert(std::make_pair(name, proc_id));
        mxProcConfig.proc_names.insert(std::make_pair(proc_id, name));
    }

    return true;
}

bool AFCBusModule::LoadBusRelation()
{
    //load bus relation files
    std::string bus_file_path = "bus/bus_relation.xml";
    std::string bus_file = pPluginManager->GetResPath() + bus_file_path;

    rapidxml::file<> xFileSource(bus_file.c_str());
    rapidxml::xml_document<> xFileDoc;
    xFileDoc.parse<0>(xFileSource.data());

    rapidxml::xml_node<>* pRoot = xFileDoc.first_node();
    if (pRoot == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }
    rapidxml::xml_node<>* pRelationNodes = pRoot->first_node("bus_relations");
    for (rapidxml::xml_node<>* pRelationNode = pRelationNodes->first_node("bus_relations"); pRelationNode != nullptr; pRelationNode = pRelationNode->next_sibling())
    {
        std::string proc = pRelationNode->first_attribute("proc")->value();
        std::string target_proc = pRelationNode->first_attribute("target_proc")->value();
        bool connection_type = ARK_LEXICAL_CAST<bool>(pRelationNode->first_attribute("target_proc")->value());

        const uint8_t& proc_type = GetAppType(proc);
        const uint8_t& target_proc_type = GetAppType(target_proc);

        auto iter = mxBusRelations.find(proc_type);
        if (iter != mxBusRelations.end())
        {
            auto target_process = iter->second;
            target_process.insert(std::make_pair(target_proc_type, connection_type));
        }
        else
        {
            std::map<uint8_t, bool> target_process;
            target_process.insert(std::make_pair(target_proc_type, connection_type));

            mxBusRelations.insert(std::make_pair(proc_type, target_process));
        }
    }

    return true;
}

const std::string& AFCBusModule::GetAppName(const uint8_t& type)
{
    auto iter = mxProcConfig.proc_names.find(type);
    return ((iter != mxProcConfig.proc_names.end()) ? iter->second : NULL_STR);
}

const uint8_t AFCBusModule::GetAppType(const std::string& name)
{
    auto iter = mxProcConfig.proc_types.find(name);
    return ((iter != mxProcConfig.proc_types.end()) ? iter->second : ARK_APP_DEFAULT);
}

uint16_t AFCBusModule::CalcProcPort(const uint8_t& type, const uint8_t inst_id)
{
    if (type <= ARK_APP_DEFAULT || type >= ARK_APP_WORLD_MAX)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return 0;
    }

    if (type > ARK_APP_DEFAULT && type < ARK_APP_CLUSTER_MAX)
    {
        return (10000 + type * 100 + inst_id);
    }
    else
    {
        return (20000 + type * 100 + inst_id);
    }
}

const AFServerConfig* AFCBusModule::GetAppServerInfo()
{
    AFBusAddr bus_addr(GetSelfBusID());
    return GetAppServerInfo(bus_addr);
}

const AFServerConfig* AFCBusModule::GetAppServerInfo(const AFBusAddr& bus_addr)
{
    auto iter = mxProcConfig.instances.find(bus_addr.proc_id);
    if (iter != mxProcConfig.instances.end())
    {
        for (auto& server : iter->second)
        {
            if (server.inst_id == bus_addr.inst_id)
            {
                return &server;
            }
        }
    }

    return nullptr;
}

const std::string& AFCBusModule::GetHost(const std::string& host)
{
    auto iter = mxProcConfig.hosts.find(host);
    if (iter != mxProcConfig.hosts.end())
    {
        return iter->second;
    }
    else
    {
        return NULL_STR;
    }
}

const std::string& AFCBusModule::GetAppHost(const int bus_id)
{
    AFBusAddr bus_addr(bus_id);
    const AFServerConfig* server_config = GetAppServerInfo(bus_addr);
    if (server_config == nullptr)
    {
        return NULL_STR;
    }
    else
    {
        return server_config->private_url;
    }
}

const int AFCBusModule::GetSelfBusID()
{
    return pPluginManager->BusID();
}

const std::string AFCBusModule::GetSelfBusName()
{
    AFBusAddr bus_id(GetSelfBusID());
    return bus_id.ToString();
}

const uint8_t AFCBusModule::GetSelfAppType()
{
    AFBusAddr bus_id(GetSelfBusID());
    return bus_id.proc_id;
}

bool AFCBusModule::GetDirectBusRelations(std::vector<std::string>& target_host_list, std::vector<uint16_t>& target_port_list)
{
    const uint8_t& app_type = GetSelfAppType();
    auto iter = mxBusRelations.find(app_type);
    if (iter != mxBusRelations.end())
    {
        for (auto it : iter->second)
        {
            if (!it.second)
            {
                //undirected
                continue;
            }

            uint8_t target_proc_id = it.first;
            auto proc_conf_iter = mxProcConfig.instances.find(target_proc_id);
            for (auto proc_conf_it : proc_conf_iter->second)
            {
                target_host_list.push_back(proc_conf_it.public_url);
                target_port_list.push_back(proc_conf_it.port);
            }
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool AFCBusModule::IsUndirectBusRelation(const int bus_id)
{
    const uint8_t& app_type = GetSelfAppType();
    auto iter = mxBusRelations.find(app_type);
    if (iter != mxBusRelations.end())
    {
        for (auto it : iter->second)
        {
            if (it.first == bus_id && !it.second)
            {
                return true;
            }
        }

        return false;
    }
    else
    {
        return false;
    }
}