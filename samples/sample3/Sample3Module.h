/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
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
#include "interface/AFIPluginManager.h"
#include "interface/AFILogModule.h"
#include "interface/AFIMetaClassModule.h"
#include "interface/AFIMapModule.h"

namespace ark
{

    class Sample3Module : public AFIModule
    {
    public:
        explicit Sample3Module() = default;

        bool Init() override;
        bool PostInit() override;
        bool PreShut() override;
        bool Shut() override;

    protected:
        int OnEvent(const AFGUID& self, const int event, const AFIDataList& arg);
        int OnClassCallBackEvent(const AFGUID& self, const std::string& strClassName, const ArkEntityEvent event, const AFIDataList& arg);
        int OnIntDataNodeCB(const AFGUID& self, const std::string& strProperty, const AFIData& oldVarList, const AFIData& newVarList);
        int OnStrDataNodeCB(const AFGUID& self, const std::string& strProperty, const AFIData& oldVarList, const AFIData& newVarList);

    protected:
        int64_t mLastTime{ 0 };

    protected:
        AFIKernelModule* m_pKernelModule;
        AFIConfigModule* m_pConfigModule;
        AFILogModule* m_pLogModule;
        AFIMetaClassModule* m_pClassModule;
        AFIMapModule* m_pMapModule;
    };

}