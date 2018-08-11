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
#include "AFCBusConfigModule.h"

AFCBusConfigModule::AFCBusConfigModule(AFIPluginManager* p)
{
    pPluginManager = p;
}

bool AFCBusConfigModule::Init()
{
    m_pProcConfigModule = pPluginManager->FindModule<AFIProcConfigModule>();
    ARK_ASSERT_NO_EFFECT(m_pProcConfigModule != nullptr);

    LoadBusRelation();

    return true;
}

bool AFCBusConfigModule::LoadBusRelation()
{
    //load bus relation files
    std::string bus_file_path = "cfg/bus_relation.xml";
    std::string bus_file = pPluginManager->GetConfigPath() + bus_file_path;

    rapidxml::file<> xFileSource(bus_file.c_str());
    rapidxml::xml_document<> xFileDoc;
    xFileDoc.parse<0>(xFileSource.data());

    rapidxml::xml_node<>* pRoot = xFileDoc.first_node();
    if (pRoot == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    for (rapidxml::xml_node<>* pRelationNode = pRoot->first_node("bus_relations"); pRelationNode != nullptr; pRelationNode = pRelationNode->next_sibling())
    {
        std::string proc = pRelationNode->first_attribute("proc")->value();
        std::string target_proc = pRelationNode->first_attribute("target_proc")->value();
        ARK_CONNECTION_TYPE connection_type = ARK_CONNECTION_TYPE(ARK_LEXICAL_CAST<int>(pRelationNode->first_attribute("target_proc")->value()));

        const ARK_PROCESS_TYPE& proc_type = m_pProcConfigModule->GetProcType(proc);
        const ARK_PROCESS_TYPE& target_proc_type = m_pProcConfigModule->GetProcType(target_proc);

        auto iter = mxBusRelations.find(proc_type);
        if (iter != mxBusRelations.end())
        {
            auto target_process = iter->second;
            target_process.insert(std::make_pair(target_proc_type, connection_type));
        }
        else
        {
            std::map<ARK_PROCESS_TYPE, ARK_CONNECTION_TYPE> target_process;
            target_process.insert(std::make_pair(target_proc_type, connection_type));

            mxBusRelations.insert(std::make_pair(proc_type, target_process));
        }
    }

    return true;
}

bool AFCBusConfigModule::GetBusRelation(const ARK_PROCESS_TYPE& target_type, const uint8_t& target_inst_id, ARK_CONNECTION_TYPE& connect_type, AFHostConfig& host_config)
{
    const ARK_PROCESS_TYPE& type = GetSelfProcType();
    auto iter = mxBusRelations.find(type);
    if (iter == mxBusRelations.end())
    {
        return false;
    }

    auto it = iter->second.find(target_type);
    connect_type = it->second;

    return m_pProcConfigModule->GetProcHostInfo(target_type, target_inst_id, host_config);
}

bool AFCBusConfigModule::GetBusServer(const ARK_PROCESS_TYPE& type, const uint8_t inst_id, AFHostConfig& host_config)
{
    return m_pProcConfigModule->GetProcHostInfo(type, inst_id, host_config);
}

const AFBusAddr AFCBusConfigModule::GetSelfBusID()
{
    AFBusAddr bus_addr;
    bus_addr.bus_id = pPluginManager->AppID();
    return bus_addr;
}

const ARK_PROCESS_TYPE AFCBusConfigModule::GetSelfProcType()
{
    AFBusAddr bus_id = GetSelfBusID();
    return ARK_PROCESS_TYPE(bus_id.proc_id);
}