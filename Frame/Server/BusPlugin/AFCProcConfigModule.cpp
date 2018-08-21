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
#include "RapidXML/rapidxml_utils.hpp"
#include "SDK/Interface/AFIPluginManager.h"
#include "AFCProcConfigModule.h"

AFCProcConfigModule::AFCProcConfigModule(AFIPluginManager* p)
{
    pPluginManager = p;
    LoadProcConfig();
}

bool AFCProcConfigModule::LoadProcConfig()
{
    std::string proc_file_path = "cfg/proc.xml";

    std::string proc_file = pPluginManager->GetConfigPath() + proc_file_path;
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
        AFHostConfig host;
        std::string name = pHostNode->first_attribute("name")->value();
        host.private_ip = pHostNode->first_attribute("private_ip")->value();
        for (rapidxml::xml_node<>* pPublicIPNode = pHostNode->first_node(); pPublicIPNode != nullptr; pPublicIPNode = pPublicIPNode->next_sibling())
        {
            std::string public_ip = pPublicIPNode->first_attribute("ip")->value();
            host.public_ips.emplace_back(public_ip);
        }

        mxProcConfig.hosts.insert(std::make_pair(name, host));
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
        ARK_PROCESS_TYPE type = ARK_PROCESS_TYPE(ARK_LEXICAL_CAST<int>(pServerNode->first_attribute("type")->value()));
        int max_connection = ARK_LEXICAL_CAST<int>(pServerNode->first_attribute("max_connection")->value());
        uint8_t thread_num = ARK_LEXICAL_CAST<int>(pServerNode->first_attribute("thread_num")->value());

        std::vector<AFServerConfig> servers;
        for (rapidxml::xml_node<>* pProcNode = pServerNode->first_node(); pProcNode != nullptr; pProcNode = pProcNode->next_sibling())
        {
            uint8_t start = ARK_LEXICAL_CAST<int>(pProcNode->first_attribute("start")->value());
            uint8_t end = ARK_LEXICAL_CAST<int>(pProcNode->first_attribute("end")->value());
            std::string host = pProcNode->first_attribute("host")->value();

            for (uint8_t i = start; i <= end; ++i)
            {
                AFServerConfig server_config;
                server_config.inst_id = i;
                server_config.max_connection = max_connection;
                server_config.thread_num = thread_num;
                server_config.host = host;
                server_config.port = CalcProcPort(type, server_config.inst_id);
                servers.emplace_back(server_config);
            }
        }

        mxProcConfig.servers.insert(std::make_pair(type, servers));
        mxProcConfig.server_names.insert(std::make_pair(type, name));
        mxProcConfig.server_types.insert(std::make_pair(name, type));
    }

    return true;
}

const std::string& AFCProcConfigModule::GetProcName(const ARK_PROCESS_TYPE& type)
{
    auto iter = mxProcConfig.server_names.find(type);
    return ((iter != mxProcConfig.server_names.end()) ? iter->second : NULL_STR);
}

const ARK_PROCESS_TYPE AFCProcConfigModule::GetProcType(const std::string& name)
{
    auto iter = mxProcConfig.server_types.find(name);
    return ((iter != mxProcConfig.server_types.end()) ? iter->second : ARK_PROC_NONE);
}

uint16_t AFCProcConfigModule::CalcProcPort(const ARK_PROCESS_TYPE& type, const uint8_t inst_id)
{
    if (type <= ARK_PROC_WORLD_MAX)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return 0;
    }

    if (type > ARK_PROC_NONE && type < ARK_PROC_CLUSTER_MAX)
    {
        return (10000 + type * 100 + inst_id);
    }
    else
    {
        return (20000 + type * 100 + inst_id);
    }
}

bool AFCProcConfigModule::GetProcServerInfo(const ARK_PROCESS_TYPE& type, uint8_t inst_id, AFServerConfig& server_config)
{
    auto iter = mxProcConfig.servers.find(type);
    if (iter != mxProcConfig.servers.end())
    {
        for (auto server : iter->second)
        {
            if (server.inst_id == inst_id)
            {
                server_config = server;
                return true;
            }
        }
    }

    return false;
}

bool AFCProcConfigModule::GetProcHostInfo(const ARK_PROCESS_TYPE& type, uint8_t inst_id, AFHostConfig& host_config)
{
    AFServerConfig server_config;
    if (!GetProcServerInfo(type, inst_id, server_config))
    {
        return false;
    }
    else
    {
        auto iter = mxProcConfig.hosts.find(server_config.host);
        if (iter != mxProcConfig.hosts.end())
        {
            host_config = iter->second;
            return true;
        }
        else
        {
            return false;
        }
    }
}

//pluginloader.exe -d -x app_name=world app_bus_id=1.1.100.1 cfg=plugin.xml
//pluginloader.exe -d -x app_name=world app_bus_id=1.1.100.2 cfg=plugin.xml