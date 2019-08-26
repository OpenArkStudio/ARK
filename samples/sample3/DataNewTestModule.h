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
#include "kernel/interface/AFIKernelNewModule.hpp"
#include "kernel/interface/AFIConfigNewModule.hpp"
#include "log/interface/AFILogModule.hpp"
#include "kernel/interface/AFIClassNewMetaModule.hpp"
#include "kernel/interface/AFIMapModule.hpp"

namespace ark {

class DataNewTestModule : public AFIModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit DataNewTestModule() = default;

    bool Init() override;
    bool PostInit() override;
    bool PreShut() override;
    bool Shut() override;

protected:
    // create test
    void CreateObjectTest();

    // query test
    void DataTest();
    void RecordTest();
    void DestroyTest();

    // call back test
    void CalssCallBackTest();
    void DataCallBackTest();
    void RecordCallBackTest();

    // event test
    void EventTest();

    //------------inner interface----------------
    ARK_SHARE_PTR<AFIObject> CreateAnPlayerAndInit();
    void QueryDataStringTest(const AFGUID& guid, size_t count);
    void QueryDataIntTest(const AFGUID& guid, size_t count);
    void SetDataTest(const AFGUID& guid, size_t count);
    void CustomDataTest(const AFGUID& guid, size_t count);

    //------------callback----------------------
    int OnClassCallBackEvent(
        const AFGUID& self, const std::string& strClassName, const ArkEntityEvent event, const AFIDataList& arg);

    int OnDataCallBackEvent(const AFGUID& self, const std::string& data_name, const AFIDataNewType& old_value,
        const AFIDataNewType& new_value);

    int OnRecordCallBackEvent(const AFGUID& self, const DATA_RECORD_EVENT_DATA& data, const AFIDataNewType& old_value,
        const AFIDataNewType& new_value);

protected:
    AFIKernelNewModule* m_pKernelModule;
    AFIConfigNewModule* m_pConfigModule;
    AFILogModule* m_pLogModule;
    AFIClassNewMetaModule* m_pClassModule;
    AFIMapModule* m_pMapModule;
};

} // namespace ark
