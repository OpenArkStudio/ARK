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
#include "mysql/interface/AFIMysqlDriverManagerModule.hpp"
#include "mysql/interface/AFIMysqlDriverModule.hpp"

namespace ark {

class AFCMysqlDriverManagerModule final : public AFIMysqlDriverManagerModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
private:
    AFMap<int, AFIMysqlDriverModule> mysql_modules_;
    AFMap<int, AFIMysqlDriverModule> invalid_mysql_modules_;
    //    int64_t last_check_time{0};

public:
    bool AddMySQLServer(const int nServerID, const std::string& strDns, const std::string& strIP, const int nPort,
        const std::string strDBName, const std::string strDBUser, const std::string strDBPwd,
        const int nRconnectTime = 10, const int nRconneCount = -1) override;
    AFIMysqlDriverModule* GetMySQLDriver() override;
    void CheckMySQL() override;

protected:
    std::string GetIPByHostName(const std::string& strHostName);
};

} // namespace ark
