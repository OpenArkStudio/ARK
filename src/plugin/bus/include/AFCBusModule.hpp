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

#include "bus/interface/AFIBusModule.hpp"

namespace ark {

class AFCBusModule final : public AFIBusModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;

    const std::string GetAppWholeName(bus_id_t bus_id) override;
    const std::string& GetAppName(const ARK_APP_TYPE& app_type) override;
    ARK_APP_TYPE GetAppType(const std::string& name) override;

    const AFProcConfig& GetSelfProc() override;

    ARK_APP_TYPE GetSelfAppType() override;
    int GetSelfBusID() override;
    const std::string GetSelfBusName() override;

    int CombineBusID(const ARK_APP_TYPE app_type, const uint8_t inst_id) override;

    //uint16_t CalcProcPort(bus_id_t bus_id) override;

    const AFRegCenter& GetRegCenter() override
    {
        return app_config_.reg_center;
    }

    bool GetTargetBusRelations(std::vector<ARK_APP_TYPE>& target_list) override;

    bool GetOtherProc(bus_id_t bus_id, AFProcConfig& proc) const override;

    int GetOtherProc(const ARK_APP_TYPE app_type, std::vector<const AFProcConfig*>& proc_list) const override;

protected:
    bool LoadBusRelationConfig(AFXmlNode& root_node);
    bool LoadRegCenterConfig(AFXmlNode& root_node);
    bool LoadProcConfig(AFXmlNode& root_node);

private:
    AFAppConfig app_config_;
};

} // namespace ark
