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
    if (!LoadBusRelationConfig())
    {
        return false;
    }

    if (!LoadRegCenterConfig())
    {
        return false;
    }

    if (!LoadProcConfig())
    {
        return false;
    }

    return true;
}

bool AFCBusModule::LoadProcConfig()
{
    static std::string proc_file = "../conf/proc.xml";
    AFXml xml_doc(proc_file);

    auto root_node = xml_doc.GetRootNode();
    if (!root_node.IsValid())
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    auto proc_nodes = root_node.FindNode("processes");
    ARK_ASSERT_RET_VAL(proc_nodes.IsValid(), false);

    for (auto proc_node = proc_nodes.FindNode("proc"); proc_node.IsValid(); proc_node.NextNode())
    {
        std::string bus_name = proc_node.GetString("busid");
        std::string endpoint_server = proc_node.GetString("endpoint_server");
        std::string endpoint_intranet = proc_node.GetString("endpoint_intranet");
        uint32_t max_connection = proc_node.GetUint32("max_connection");
        uint8_t thread_num = static_cast<uint8_t>(proc_node.GetUint32("thread_num"));

        AFBusAddr bus_addr;
        ARK_ASSERT_CONTINUE(bus_addr.FromString(bus_name));

        // just load self process configuration
        if (bus_addr.bus_id != GetSelfBusID())
        {
            continue;
        }

        app_config_.self_proc.bus_id = bus_addr.bus_id;
        app_config_.self_proc.thread_num = thread_num;
        app_config_.self_proc.max_connection = max_connection;

        std::error_code ec;
        app_config_.self_proc.server_ep = AFEndpoint::FromString(endpoint_server, ec);
        app_config_.self_proc.intranet_ep = AFEndpoint::FromString(endpoint_intranet, ec);

        break;
    }

    return true;
}

bool AFCBusModule::LoadBusRelationConfig()
{
    // load bus relation files
    std::string bus_file = "../conf/bus_relation.xml";
    AFXml xml_doc(bus_file);

    auto root_node = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root_node.IsValid(), false);

    auto application_nodes = root_node.FindNode("applications");
    ARK_ASSERT_RET_VAL(application_nodes.IsValid(), false);
    for (auto app_node = application_nodes.FindNode("application"); app_node.IsValid(); app_node.NextNode())
    {
        std::string app_name = app_node.GetString("name");
        uint32_t app_type = app_node.GetUint32("type");

        app_config_.name2types.insert(std::make_pair(app_name, static_cast<ARK_APP_TYPE>(app_type)));
        app_config_.type2names.insert(std::make_pair(static_cast<ARK_APP_TYPE>(app_type), app_name));
    }

    auto relation_nodes = root_node.FindNode("relations");
    for (auto relation_node = relation_nodes.FindNode("relation"); relation_node.IsValid(); relation_node.NextNode())
    {
        std::string source_app = relation_node.GetString("source");
        std::string target_app = relation_node.GetString("target");

        const ARK_APP_TYPE source_type = GetAppType(source_app);
        const ARK_APP_TYPE target_type = GetAppType(target_app);

        ARK_ASSERT_CONTINUE(source_type > ARK_APP_TYPE::ARK_APP_DEFAULT && source_type < ARK_APP_TYPE::ARK_APP_MAX);
        ARK_ASSERT_CONTINUE(target_type > ARK_APP_TYPE::ARK_APP_DEFAULT && target_type < ARK_APP_TYPE::ARK_APP_MAX);

        auto iter = app_config_.connection_relations.find(source_type);
        if (iter != app_config_.connection_relations.end())
        {
            iter->second.emplace_back(target_type);
        }
        else
        {
            std::vector<ARK_APP_TYPE> target_list;
            target_list.emplace_back(target_type);
            app_config_.connection_relations.insert(std::make_pair(source_type, target_list));
        }
    }

    return true;
}

bool AFCBusModule::LoadRegCenterConfig()
{
    // load reg center files
    std::string reg_file = "../conf/reg_center.xml";
    AFXml xml_doc(reg_file);

    auto root_node = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root_node.IsValid(), false);

    auto node = root_node.FindNode("reg_center");
    ARK_ASSERT_RET_VAL(node.IsValid(), false);

    app_config_.reg_center.ip = node.GetString("ip");
    app_config_.reg_center.port = node.GetUint32("port");
    app_config_.reg_center.service_name = node.GetString("service_name");
    app_config_.reg_center.check_interval = node.GetString("check_interval");
    app_config_.reg_center.check_timeout = node.GetString("check_timeout");

    return true;
}

const std::string& AFCBusModule::GetAppName(const ARK_APP_TYPE& app_type)
{
    auto iter = app_config_.type2names.find(app_type);
    return ((iter != app_config_.type2names.end()) ? iter->second : NULL_STR);
}

ARK_APP_TYPE AFCBusModule::GetAppType(const std::string& name)
{
    auto iter = app_config_.name2types.find(name);
    return ((iter != app_config_.name2types.end()) ? iter->second : ARK_APP_TYPE::ARK_APP_DEFAULT);
}

//uint16_t AFCBusModule::CalcProcPort(const int bus_id)
//{
//    AFBusAddr bus_addr(bus_id);
//    const ARK_APP_TYPE& app_type = ARK_APP_TYPE(bus_addr.app_type);
//    if (app_type <= ARK_APP_TYPE::ARK_APP_DEFAULT || app_type >= ARK_APP_TYPE::ARK_APP_WORLD_MAX)
//    {
//        ARK_ASSERT_NO_EFFECT(0);
//        return 0;
//    }
//
//    // need check if valid
//    if (app_type > ARK_APP_TYPE::ARK_APP_DEFAULT && app_type < ARK_APP_TYPE::ARK_APP_CLUSTER_MAX)
//    {
//        return (10000 + bus_addr.app_type * 100 + bus_addr.inst_id);
//    }
//    else
//    {
//        return (20000 + bus_addr.app_type * 100 + bus_addr.inst_id);
//    }
//}

const AFProcConfig& AFCBusModule::GetSelfProc()
{
    return app_config_.self_proc;
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

int AFCBusModule::CombineBusID(const ARK_APP_TYPE app_type, const uint8_t inst_id)
{
    ARK_ASSERT_RET_VAL(app_type > ARK_APP_TYPE::ARK_APP_DEFAULT && app_type < ARK_APP_TYPE::ARK_APP_MAX, 0);

    int self_bus = GetSelfBusID();
    AFBusAddr self_bus_addr(self_bus);
    AFBusAddr target_bus_addr(self_bus_addr.channel_id, self_bus_addr.zone_id, static_cast<uint8_t>(app_type), inst_id);
    return target_bus_addr.bus_id;
}

bool AFCBusModule::GetTargetBusRelations(std::vector<ARK_APP_TYPE>& target_list)
{
    AFBusAddr bus_addr(GetSelfBusID());
    auto iter = app_config_.connection_relations.find(static_cast<ARK_APP_TYPE>(bus_addr.app_type));
    if (iter != app_config_.connection_relations.end())
    {
        target_list = iter->second;
    }

    return true;
}

} // namespace ark
