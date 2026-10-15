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

#pragma once

#include "ark/kernel/kernel_module.hpp"
#include "ark/kernel/map_module.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

class sample3_module final : public module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit sample3_module() = default;

    bool init() override;
    bool post_init() override;
    bool pre_shut() override;
    bool shut() override;

protected:
    // create test
    void CreateEntityTest();

    // query test
    void DataTest();
    void TableTest();
    void DestroyTest();

    // call back test
    void CalssCallBackTest();
    void DataCallBackTest();
    void TableCallBackTest();
    void ContainerCallBackTest();

    // event test
    void EventTest();

    //------------inner interface----------------
    std::shared_ptr<entity> CreateAnPlayerAndInit();
    void QueryStringTest(const guid_t& guid, size_t count, const uint32_t index);
    void QueryIntTest(const guid_t& guid, size_t count, const uint32_t index);
    void SetDataTest(const guid_t& guid, size_t count, const uint32_t index);
    void CustomDataTest(const guid_t& guid, size_t count, const uint32_t index);

    //------------callback----------------------
    int OnClassCallBackEvent(
        const guid_t& self, const std::string& strClassName, const entity_event event, const data_list& arg);

    int OnDataCallBackEvent(const guid_t& self, const std::string& data_name, const uint32_t index,
        const ark::data& old_value, const ark::data& new_value);

    int OnTableCallBackEvent(
        const guid_t& self, const table_event_data& data, const ark::data& old_value, const ark::data& new_value);

    int OnContainerCallBackEvent(const guid_t& self, const uint32_t index, const container_op_type op_type,
        const uint32_t src_index, const uint32_t dest_index);

protected:
    kernel_module* m_pKernelModule;
    map_module* m_pMapModule;
};

} // namespace ark
