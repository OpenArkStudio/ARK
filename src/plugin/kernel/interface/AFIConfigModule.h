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

#include "kernel/include/AFCDataList.hpp"
#include "interface/AFIModule.h"
#include "kernel/interface/AFIDataNodeManager.h"
#include "kernel/interface/AFIDataTableManager.h"

namespace ark
{

    class AFIConfigModule : public AFIModule
    {
    public:
        virtual bool Load() = 0;
        virtual bool Save() = 0;
        virtual bool Clear() = 0;
        virtual bool ExistConfig(const std::string& config_id) = 0;
        virtual bool ExistConfig(const std::string& class_name, const std::string& config_id) = 0;

        virtual ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager(const std::string& config_id) = 0;
        virtual ARK_SHARE_PTR<AFIDataTableManager> GetTableManager(const std::string& config_id) = 0;

        virtual bool GetNodeBool(const std::string& config_id, const std::string& node_name) = 0;
        virtual int32_t GetNodeInt(const std::string& config_id, const std::string& node_name) = 0;
        virtual int64_t GetNodeInt64(const std::string& config_id, const std::string& node_name) = 0;
        virtual float GetNodeFloat(const std::string& config_id, const std::string& node_name) = 0;
        virtual double GetNodeDouble(const std::string& config_id, const std::string& node_name) = 0;
        virtual const char* GetNodeString(const std::string& config_id, const std::string& node_name) = 0;
    };

}