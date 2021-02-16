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
#include "mysql/interface/AFIMysqlModule.hpp"
#include "mysql/interface/AFIMysqlDriverManagerModule.hpp"

namespace ark {

class AFCMysqlModule final : public AFIMysqlModule
{
    ARK_DECLARE_MODULE_FUNCTIONS

private:
    std::shared_ptr<AFIMysqlDriverManagerModule> mysql_driver_manager;
    int64_t last_check_time_{0};

public:
    AFCMysqlModule();
    ~AFCMysqlModule();

    bool Init() override;
    bool PostInit() override;
    bool Update() override;
    bool Shut() override;

    //////////////////////////////////////////////////////////////////////////
    bool UpdateOrInsert(const std::string& strRecordName, const std::string& strKey,
        const std::vector<std::string>& fieldVec, const std::vector<std::string>& valueVec) override;
    bool Query(const std::string& strRecordName, const std::string& strKey, const std::vector<std::string>& fieldVec,
        std::vector<std::string>& valueVec) override;
    bool Select(const std::string& strRecordName, const std::string& strKey, const std::vector<std::string>& fieldVec,
        std::vector<std::string>& valueVec) override;

    bool Delete(const std::string& strRecordName, const std::string& strKey) override;
    bool Exists(const std::string& strRecordName, const std::string& strKey, bool& bExit) override;
    bool Keys(
        const std::string& strRecordName, const std::string& strKeyName, std::vector<std::string>& valueVec) override;
    bool AddMysqlServer(const int nServerID, const std::string& strDns, const std::string& strIP, const int nPort,
        const std::string strDBName, const std::string strDBUser, const std::string strDBPwd,
        const int nRconnectTime = 10, const int nRconneCount = -1) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Insert(const std::string& document, std::vector<KVPair>& data) override;
    bool UpdateRecord(const std::string& document, std::vector<KVPair>& data, std::vector<where>& where) override;
    bool Select(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        OrderBy& order, int limit, ResultArr& results) override;
    bool SelectOne(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        OrderBy& order, Result& result) override;
};

} // namespace ark
