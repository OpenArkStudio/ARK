/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
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

#pragma once

#include "ark/bus/bus_module.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/xml.hpp"

namespace ark {

class bus_module_impl final : public bus_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;

    const std::string get_app_whole_name(bus_id_t bus_id) override;
    const std::string& get_app_name(const ark::app_type& app_type) override;
    app_type get_app_type(const std::string& name) override;

    const proc_config& get_self_proc() override;

    app_type get_self_app_type() override;
    int get_self_bus_id() override;
    const std::string get_self_bus_name() override;

    int combine_bus_id(const app_type app_type, const uint8_t inst_id) override;

    //uint16_t CalcProcPort(bus_id_t bus_id) override;

    const reg_center& get_reg_center() override {
        return app_config_.reg_center;
    }

    bool get_target_bus_relations(std::vector<app_type>& target_list) override;

    bool get_other_proc(bus_id_t bus_id, proc_config& proc) const override;

    int get_other_proc(const app_type app_type, std::vector<const proc_config*>& proc_list) const override;

protected:
    bool load_bus_relation_config(xml_node& root_node);
    bool load_reg_center_config(xml_node& root_node);
    bool load_proc_config(xml_node& root_node);

private:
    app_config app_config_;
};

} // namespace ark
