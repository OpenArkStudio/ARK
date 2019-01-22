/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#pragma once

#include "interface/AFIKernelModule.h"
#include "interface/AFIConfigModule.h"
#include "interface/AFIMetaClassModule.h"
#include "interface/AFIPluginManager.h"
#include "interface/AFILogModule.h"
#include "interface/AFIPropertyModule.h"
#include "interface/AFIPropertyConfigModule.h"
#include "interface/AFIPropertyTrailModule.h"

namespace ark
{

    class AFCPropertyTrailModule : public AFIPropertyTrailModule
    {
    public:
        explicit AFCPropertyTrailModule() = default;

        bool Init() override;

        virtual void StartTrail(const AFGUID self);
        virtual void EndTrail(const AFGUID self);

    protected:

        int LogObjectData(const AFGUID& self);
        int TrailObjectData(const AFGUID& self);

        int OnObjectPropertyEvent(const AFGUID& self, const std::string& nodeName, const AFIData& oldVar, const AFIData& newVar);

        int OnEntityTableEvent(const AFGUID& self, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);

    private:
        AFIKernelModule* m_pKernelModule;
        AFIConfigModule* m_pConfigModule;
        AFIMetaClassModule* m_pClassModule;
        AFILogModule* m_pLogModule;
    };

}