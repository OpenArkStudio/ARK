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
#include "mysql/interface/AFIMysqlDriverModule.hpp"

namespace ark {

#define ARK_MYSQL_TRY_BEGIN                                                                                            \
    try                                                                                                                \
    {

#define ARK_MYSQL_TRY_END(msg)                                                                                         \
    }                                                                                                                  \
    catch (mysqlpp::BadQuery & er)                                                                                     \
    {                                                                                                                  \
        std::cout << "BadQuery [" << msg << "] Error: " << er.what() << std::endl;                                     \
        return false;                                                                                                  \
    }                                                                                                                  \
    catch (const mysqlpp::BadConversion& er)                                                                           \
    {                                                                                                                  \
        std::cout << "BadConversion [" << msg << "] Error:" << er.what() << " retrieved data size:" << er.retrieved    \
                  << ", actual size:" << er.actual_size << std::endl;                                                  \
        return false;                                                                                                  \
    }                                                                                                                  \
    catch (const mysqlpp::Exception& er)                                                                               \
    {                                                                                                                  \
        std::cout << "mysqlpp::Exception [" << msg << "] Error:" << er.what() << std::endl;                            \
        return false;                                                                                                  \
    }                                                                                                                  \
    catch (...)                                                                                                        \
    {                                                                                                                  \
        std::cout << "std::exception [" << msg << "] Error:Unknown " << std::endl;                                     \
        return false;                                                                                                  \
    }

#define ARK_MYSQL_TRY_END_NULL_RETURN(msg)                                                                             \
    }                                                                                                                  \
    catch (mysqlpp::BadQuery er)                                                                                       \
    {                                                                                                                  \
        std::cout << "BadQuery [" << msg << "] Error: " << er.what() << std::endl;                                     \
        return;                                                                                                        \
    }                                                                                                                  \
    catch (const mysqlpp::BadConversion& er)                                                                           \
    {                                                                                                                  \
        std::cout << "BadConversion [" << msg << "] Error:" << er.what() << " retrieved data size:" << er.retrieved    \
                  << ", actual size:" << er.actual_size << std::endl;                                                  \
        return;                                                                                                        \
    }                                                                                                                  \
    catch (const mysqlpp::Exception& er)                                                                               \
    {                                                                                                                  \
        std::cout << "mysqlpp::Exception [" << msg << "] Error:" << er.what() << std::endl;                            \
        return;                                                                                                        \
    }                                                                                                                  \
    catch (...)                                                                                                        \
    {                                                                                                                  \
        std::cout << "std::exception [" << msg << "] Error:Unknown " << std::endl;                                     \
        return;                                                                                                        \
    }

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class AFCMysqlDriverModule final : public AFIMysqlDriverModule
{
    ARK_DECLARE_MODULE_FUNCTIONS

private:
    std::string db_name_;
    std::string db_host_;
    int db_port_{0};
    std::string db_user_;
    std::string db_pwd_;

    mysqlpp::Connection* mysql_conn_{nullptr};

    float max_check_reconnect_{0.0f};
    int max_reconnect_time_;
    int max_reconnect_count_;

    const static std::string DefaultKey;
    const static std::string DefaultTable;

public:
    explicit AFCMysqlDriverModule(int nReconnectTime = 60, int nReconnectCount = -1);
    AFCMysqlDriverModule(const std::string& strDBName, const std::string& strDBHost, int nDBPort,
        const std::string& strDBUser, const std::string& strDBPwd);
    ~AFCMysqlDriverModule();

    bool Update() override;

    bool Connect(const std::string& strDBName, const std::string& strDBHost, const int nDBPort,
        const std::string& strDBUser, const std::string& strDBPwd) override;
    void CloseConnection() override;
    mysqlpp::Connection* GetConnection() override;

    bool IsAvailable() override;
    bool CanReconnect() override;
    bool Reconnect() override;

    bool Query(const std::string& qstr) override;
    bool Query(const std::string& qstr, mysqlpp::StoreQueryResult& queryResult) override;
    bool Query(const std::string& tableName, const std::string& strKey, const std::vector<std::string>& fieldVec,
        std::vector<std::string>& valueVec) override;
    bool UpdateOrInsert(const std::string& tableName, const std::string& strKey,
        const std::vector<std::string>& fieldVec, const std::vector<std::string>& valueVec) override;
    bool Select(const std::string& tableName, const std::string& strKey, const std::vector<std::string>& fieldVec,
        std::vector<std::string>& valueVec) override;
    bool Delete(const std::string& tableName, const std::string& strKey) override;
    bool Exists(const std::string& tableName, const std::string& strKey, bool& bExit) override;
    bool Keys(const std::string& tableName, const std::string& strKeyName, std::vector<std::string>& valueVec) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Insert(const std::string& document, std::vector<KVPair>& data) override;
    bool UpdateRecord(const std::string& document, std::vector<KVPair>& data, std::vector<where>& where) override;
    bool Select(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        OrderBy& order, int limit, ResultArr& results) override;
    bool SelectOne(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        OrderBy& order, Result& result) override;

protected:
    bool IsNeedReconnect();
    bool Connect();
};

} // namespace ark
