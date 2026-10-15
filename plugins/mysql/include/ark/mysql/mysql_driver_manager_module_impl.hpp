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

#include "ark/mysql/mysql_driver_manager_module.hpp"
#include "ark/mysql/mysql_driver_module.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

class mysql_driver_manager_module_impl final : public mysql_driver_manager_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
private:
    object_map<int, mysql_driver_module> mysql_modules_;
    object_map<int, mysql_driver_module> invalid_mysql_modules_;
    //    int64_t last_check_time{0};

public:
    bool AddMySQLServer(const int nServerID, const std::string& strDns, const std::string& strIP, const int nPort,
        const std::string strDBName, const std::string strDBUser, const std::string strDBPwd,
        const int nRconnectTime = 10, const int nRconneCount = -1) override;
    mysql_driver_module* GetMySQLDriver() override;
    void CheckMySQL() override;

protected:
    std::string GetIPByHostName(const std::string& strHostName);
};

} // namespace ark
