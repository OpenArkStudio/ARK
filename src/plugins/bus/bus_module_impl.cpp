/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/bus/bus_module_impl.hpp"
#include "ark/bus/bus_plugin.hpp"
#include "ark/define.hpp"
#include "ark/enum.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/xml.hpp"

namespace ark {

bool bus_module_impl::init() {
    xml xml_doc(bus_plugin::get_plugin_conf());
    auto root_node = xml_doc.get_root_node();
    ARK_ASSERT_RET_VAL(root_node.is_valid(), false);

    if (!load_bus_relation_config(root_node))
    {
        return false;
    }

    if (!load_reg_center_config(root_node))
    {
        return false;
    }

    if (!load_proc_config(root_node))
    {
        return false;
    }

    return true;
}

bool bus_module_impl::load_proc_config(xml_node& root_node) {
    auto processes_nodes = root_node.find_node("processes");
    ARK_ASSERT_RET_VAL(processes_nodes.is_valid(), false);

    for (auto proc_node = processes_nodes.find_node("proc"); proc_node.is_valid(); proc_node.next_node())
    {
        std::string bus_name = proc_node.get_string("busid");
        std::string endpoint_server = proc_node.get_string("endpoint_server");
        std::string endpoint_intranet = proc_node.get_string("endpoint_intranet");
        uint32_t max_connection = proc_node.get_uint32("max_connection");
        uint8_t thread_num = static_cast<uint8_t>(proc_node.get_uint32("thread_num"));

        bus_addr bus_addr;
        ARK_ASSERT_CONTINUE(bus_addr.from_string(bus_name));

        proc_config proc;
        proc.bus_id = bus_addr.bus_id;
        proc.thread_num = thread_num;
        proc.max_connection = max_connection;

        std::error_code ec;
        proc.server_ep = endpoint::from_string(endpoint_server, ec);
        proc.intranet_ep = endpoint::from_string(endpoint_intranet, ec);
        if (bus_addr.bus_id == get_self_bus_id())
        {
#ifdef ARK_DOCKER_ALPINE
            proc.intranet_ep.set_ip(getenv("MY_POD_IP"));
#endif
            app_config_.self_proc = proc;
        }
        else
        {
            app_config_.other_proc_list.insert(std::make_pair(proc.bus_id, proc));
        }

        // just load self process configuration
        //if (bus_addr.bus_id != GetSelfBusID())
        //{
        //    continue;
        //}

        //app_config_.self_proc.bus_id = bus_addr.bus_id;
        //app_config_.self_proc.thread_num = thread_num;
        //app_config_.self_proc.max_connection = max_connection;

        //std::error_code ec;
        //app_config_.self_proc.server_ep = AFEndpoint::FromString(endpoint_server, ec);
        //app_config_.self_proc.intranet_ep = AFEndpoint::FromString(endpoint_intranet, ec);

        //break;
    }

    return true;
}

bool bus_module_impl::load_bus_relation_config(xml_node& root_node) {
    auto application_nodes = root_node.find_node("applications");
    ARK_ASSERT_RET_VAL(application_nodes.is_valid(), false);

    for (auto app_node = application_nodes.find_node("application"); app_node.is_valid(); app_node.next_node())
    {
        std::string app_name = app_node.get_string("name");
        uint32_t app_type = app_node.get_uint32("type");

        app_config_.name2types.insert(std::make_pair(app_name, static_cast<ark::app_type>(app_type)));
        app_config_.type2names.insert(std::make_pair(static_cast<ark::app_type>(app_type), app_name));
    }

    auto relation_nodes = root_node.find_node("relations");
    ARK_ASSERT_RET_VAL(relation_nodes.is_valid(), false);

    for (auto relation_node = relation_nodes.find_node("relation"); relation_node.is_valid(); relation_node.next_node())
    {
        std::string source_app = relation_node.get_string("source");
        std::string target_app = relation_node.get_string("target");

        const app_type source_type = get_app_type(source_app);
        const app_type target_type = get_app_type(target_app);

        ARK_ASSERT_CONTINUE(source_type > ark::app_type::ark_app_default && source_type < ark::app_type::ark_app_max);
        ARK_ASSERT_CONTINUE(target_type > ark::app_type::ark_app_default && target_type < ark::app_type::ark_app_max);

        auto iter = app_config_.connection_relations.find(source_type);
        if (iter != app_config_.connection_relations.end())
        {
            iter->second.emplace_back(target_type);
        }
        else
        {
            std::vector<app_type> target_list;
            target_list.emplace_back(target_type);
            app_config_.connection_relations.insert(std::make_pair(source_type, target_list));
        }
    }

    return true;
}

bool bus_module_impl::load_reg_center_config(xml_node& root_node) {
    auto node = root_node.find_node("center");
    ARK_ASSERT_RET_VAL(node.is_valid(), false);

    app_config_.reg_center.ip = node.get_string("ip");
    app_config_.reg_center.port = node.get_uint32("port");
    app_config_.reg_center.service_name = node.get_string("service_name");
    app_config_.reg_center.check_interval = node.get_string("check_interval");
    app_config_.reg_center.check_timeout = node.get_string("check_timeout");

    return true;
}

const std::string& bus_module_impl::get_app_name(const ark::app_type& app_type) {
    auto iter = app_config_.type2names.find(app_type);
    return ((iter != app_config_.type2names.end()) ? iter->second : null_str);
}

app_type bus_module_impl::get_app_type(const std::string& name) {
    auto iter = app_config_.name2types.find(name);
    return ((iter != app_config_.name2types.end()) ? iter->second : ark::app_type::ark_app_default);
}

//uint16_t AFCBusModule::CalcProcPort(bus_id_t bus_id)
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

const proc_config& bus_module_impl::get_self_proc() {
    return app_config_.self_proc;
}

bus_id_t bus_module_impl::get_self_bus_id() {
    return get_plugin_manager()->get_bus_id();
}

const std::string bus_module_impl::get_self_bus_name() {
    bus_addr bus_id(get_self_bus_id());
    return bus_id.to_string();
}

app_type bus_module_impl::get_self_app_type() {
    bus_addr bus_id(get_self_bus_id());
    return app_type(bus_id.app_type);
}

const std::string bus_module_impl::get_app_whole_name(bus_id_t bus_id) {
    bus_addr addr(bus_id);
    std::string name = get_app_name(app_type(addr.app_type));
    return (name + "-" + addr.to_string());
}

int bus_module_impl::combine_bus_id(const app_type app_type, const uint8_t inst_id) {
    ARK_ASSERT_RET_VAL(app_type > ark::app_type::ark_app_default && app_type < ark::app_type::ark_app_max, 0);

    int self_bus = get_self_bus_id();
    bus_addr self_bus_addr(self_bus);
    bus_addr target_bus_addr(self_bus_addr.channel_id, self_bus_addr.zone_id, static_cast<uint8_t>(app_type), inst_id);
    return target_bus_addr.bus_id;
}

bool bus_module_impl::get_target_bus_relations(std::vector<app_type>& target_list) {
    bus_addr bus_addr(get_self_bus_id());
    auto iter = app_config_.connection_relations.find(static_cast<ark::app_type>(bus_addr.app_type));
    if (iter != app_config_.connection_relations.end())
    {
        target_list = iter->second;
    }

    return true;
}

bool bus_module_impl::get_other_proc(bus_id_t bus_id, proc_config& proc) const {
    auto iter = app_config_.other_proc_list.find(bus_id);
    if (iter == app_config_.other_proc_list.end())
    {
        return false;
    }

    proc = iter->second;
    return true;
}

int bus_module_impl::get_other_proc(const app_type app_type, std::vector<const proc_config*>& proc_list) const {
    for (auto& iter : app_config_.other_proc_list)
    {
        auto& proc = iter.second;
        bus_addr bus_id(proc.bus_id);
        if (static_cast<ark::app_type>(bus_id.app_type) != app_type)
        {
            continue;
        }

        proc_list.push_back(&proc);
    }

    return proc_list.size();
}

} // namespace ark
