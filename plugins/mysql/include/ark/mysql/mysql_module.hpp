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

#include <mysql++/mysql++.h>

#include "ark/module.hpp"
#include "ark/mysql/mysql_driver_module.hpp"

namespace ark {

class mysql_module : public module
{
public:
    virtual bool AddMysqlServer(const int nServerID, const std::string& strDns, const std::string& strIP,
        const int nPort, const std::string strDBName, const std::string strDBUser, const std::string strDBPwd,
        const int nRconnectTime = 10, const int nRconneCount = -1) = 0;
    virtual bool UpdateOrInsert(const std::string& strRecordName, const std::string& strKey,
        const std::vector<std::string>& fieldVec, const std::vector<std::string>& valueVec) = 0;
    virtual bool Query(const std::string& strRecordName, const std::string& strKey,
        const std::vector<std::string>& fieldVec, std::vector<std::string>& valueVec) = 0;
    virtual bool Select(const std::string& strRecordName, const std::string& strKey,
        const std::vector<std::string>& fieldVec, std::vector<std::string>& valueVec) = 0;

    virtual bool Delete(const std::string& strRecordName, const std::string& strKey) = 0;
    virtual bool Exists(const std::string& strRecordName, const std::string& strKey, bool& bExit) = 0;
    virtual bool Keys(
        const std::string& strRecordName, const std::string& strKeyName, std::vector<std::string>& valueVec) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool Insert(const std::string& document, std::vector<kv_pair>& data) = 0;
    virtual bool UpdateRecord(const std::string& document, std::vector<kv_pair>& data, std::vector<where>& where) = 0;
    virtual bool Select(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        order_by& order, int limit, result_arr& results) = 0;
    virtual bool SelectOne(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        order_by& order, result& result) = 0;
};

} // namespace ark
