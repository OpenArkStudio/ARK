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

#pragma once

#include "base/AFPluginManager.hpp"
#include "kernel/interface/AFIKernelModule.hpp"
#include "kernel/interface/AFIConfigModule.hpp"
#include "log/interface/AFILogModule.hpp"
#include "kernel/interface/AFIClassMetaModule.hpp"
#include "kernel/interface/AFIMapModule.hpp"

namespace ark {

class Sample3Module : public AFIModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit Sample3Module() = default;

    bool Init() override;
    bool PostInit() override;
    bool PreShut() override;
    bool Shut() override;

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

    // event test
    void EventTest();

    //------------inner interface----------------
    ARK_SHARE_PTR<AFIEntity> CreateAnPlayerAndInit();
    void QueryStringTest(const AFGUID& guid, size_t count, const uint32_t index);
    void QueryIntTest(const AFGUID& guid, size_t count, const uint32_t index);
    void SetDataTest(const AFGUID& guid, size_t count, const uint32_t index);
    void CustomDataTest(const AFGUID& guid, size_t count, const uint32_t index);

    //------------callback----------------------
    int OnClassCallBackEvent(
        const AFGUID& self, const std::string& strClassName, const ArkEntityEvent event, const AFIDataList& arg);

    int OnDataCallBackEvent(const AFGUID& self, const std::string& data_name, const uint32_t index,
        const AFIData& old_value, const AFIData& new_value);

    int OnTableCallBackEvent(
        const AFGUID& self, const TABLE_EVENT_DATA& data, const AFIData& old_value, const AFIData& new_value);

protected:
    AFIKernelModule* m_pKernelModule;
    AFIConfigModule* m_pConfigModule;
    AFILogModule* m_pLogModule;
    AFIClassMetaModule* m_pClassModule;
    AFIMapModule* m_pMapModule;
};

} // namespace ark
