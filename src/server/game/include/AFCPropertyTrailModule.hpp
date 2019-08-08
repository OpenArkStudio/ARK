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

#include "log/interface/AFILogModule.hpp"
#include "kernel/interface/AFIKernelModule.hpp"
#include "kernel/interface/AFIConfigModule.hpp"
#include "kernel/interface/AFIMetaClassModule.hpp"
#include "game/interface/AFIPropertyModule.hpp"
#include "game/interface/AFIPropertyConfigModule.hpp"
#include "game/interface/AFIPropertyTrailModule.hpp"

namespace ark {

class AFCPropertyTrailModule : public AFIPropertyTrailModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;

    virtual void StartTrail(const AFGUID self);
    virtual void EndTrail(const AFGUID self);

protected:
    int LogObjectData(const AFGUID& self);
    int TrailObjectData(const AFGUID& self);

    int OnObjectPropertyEvent(
        const AFGUID& self, const std::string& nodeName, const AFIData& oldVar, const AFIData& newVar);

    int OnEntityTableEvent(
        const AFGUID& self, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);

private:
    AFIKernelModule* m_pKernelModule;
    AFIConfigModule* m_pConfigModule;
    AFIMetaClassModule* m_pClassModule;
    AFILogModule* m_pLogModule;
};

} // namespace ark
