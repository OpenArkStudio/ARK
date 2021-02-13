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

#include <mysql++/mysql++.h>
#include "mysql/include/AFCMysqlDriverModule.hpp"

namespace ark {

const std::string AFCMysqlDriverModule::DefaultKey = "ID";
const std::string AFCMysqlDriverModule::DefaultTable = "RoleInfo";

AFCMysqlDriverModule::AFCMysqlDriverModule(int nReconnectTime, int nReconnectCount)
    : max_reconnect_time_(nReconnectTime)
    , max_reconnect_count_(nReconnectCount)
{
}

AFCMysqlDriverModule::AFCMysqlDriverModule(const std::string& strDBName, const std::string& strDBHost,
    const int nDBPort, const std::string& strDBUser, const std::string& strDBPwd)
    : max_reconnect_time_(60)
    , max_reconnect_count_(-1)
{

    Connect(strDBName, strDBHost, nDBPort, strDBUser, strDBPwd);
}

AFCMysqlDriverModule::~AFCMysqlDriverModule()
{
    CloseConnection();
}

bool AFCMysqlDriverModule::Update()
{
    if (IsNeedReconnect() && CanReconnect())
    {
        return Connect(db_name_, db_host_, db_port_, db_user_, db_pwd_);
    }
    return true;
}

///////////////////////////////////////////////////////////////

bool AFCMysqlDriverModule::Connect(const std::string& strDBName, const std::string& strDBHost, const int nDBPort,
    const std::string& strDBUser, const std::string& strDBPwd)
{
    db_name_ = strDBName;
    db_host_ = strDBHost;
    db_port_ = nDBPort;
    db_user_ = strDBUser;
    db_pwd_ = strDBPwd;

    return Connect();
}

bool AFCMysqlDriverModule::Connect()
{
    mysql_conn_ = new mysqlpp::Connection();
    if (mysql_conn_ == nullptr)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysql_conn_->set_option(new mysqlpp::MultiStatementsOption(true));
    mysql_conn_->set_option(new mysqlpp::SetCharsetNameOption("utf8mb4"));
    mysql_conn_->set_option(new mysqlpp::ReconnectOption(true));
    mysql_conn_->set_option(new mysqlpp::ConnectTimeoutOption(60));
    if (!mysql_conn_->connect(db_name_.c_str(), db_host_.c_str(), db_user_.c_str(), db_pwd_.c_str(), db_port_))
    {
        CloseConnection();
        // connect fail
        return false;
    }

    // set time-out period 24h
    mysqlpp::Query query = mysql_conn_->query("set interactive_timeout = 24*3600");
    query.execute();
    query.reset();
    ARK_MYSQL_TRY_END("Connect failed")

    return true;
}

void AFCMysqlDriverModule::CloseConnection()
{
    mysql_conn_->disconnect();
    delete mysql_conn_;
    mysql_conn_ = nullptr;
}

mysqlpp::Connection* AFCMysqlDriverModule::GetConnection()
{
    return mysql_conn_;
}

bool AFCMysqlDriverModule::IsAvailable()
{
    return !IsNeedReconnect();
}

bool AFCMysqlDriverModule::Reconnect()
{
    CloseConnection();
    Connect(db_name_, db_host_, db_port_, db_user_, db_pwd_);

    if (max_reconnect_count_ > 0)
    {
        max_reconnect_count_--;
    }

    return true;
}

bool AFCMysqlDriverModule::CanReconnect()
{
    max_check_reconnect_ += 0.1f;

    //30分钟检查断线重连
    if (max_check_reconnect_ < max_reconnect_time_)
    {
        return false;
    }

    if (max_reconnect_count_ == 0)
    {
        return false;
    }

    max_check_reconnect_ = 0.0f;

    return true;
}

bool AFCMysqlDriverModule::IsNeedReconnect()
{
    if (db_host_.length() < 1 || db_user_.length() < 1)
    {
        return false;
    }

    if (mysql_conn_ == nullptr)
    {
        return true;
    }

    if (!mysql_conn_->connected())
    {
        CloseConnection();
        return true;
    }

    if (!mysql_conn_->ping())
    {
        CloseConnection();
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////

bool AFCMysqlDriverModule::Query(const std::string& qstr)
{
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query(qstr);
    query.execute();
    query.reset();
    ARK_MYSQL_TRY_END(qstr)

    return true;
}

bool AFCMysqlDriverModule::Query(const std::string& qstr, mysqlpp::StoreQueryResult& queryResult)
{
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query(qstr);
    queryResult = query.store();
    query.reset();
    ARK_MYSQL_TRY_END(qstr)

    return true;
}

bool AFCMysqlDriverModule::Query(const std::string& tableName, const std::string& strKey,
    const std::vector<std::string>& fieldVec, std::vector<std::string>& valueVec)
{
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "SELECT ";
    for (auto iter = fieldVec.begin(); iter != fieldVec.end(); ++iter)
    {
        if (iter == fieldVec.begin())
        {
            query << *iter;
        }
        else
        {
            query << "," << *iter;
        }
    }
    query << " FROM " << tableName << " WHERE " << DefaultKey << " = " << mysqlpp::quote << strKey << ";";
    //query.execute(); // 官网例子不需要execute
    mysqlpp::StoreQueryResult xResult = query.store();
    query.reset();

    if (xResult.empty() || !xResult)
    {
        return false;
    }

    // xResult应该只有一行的，为了以后可能出现的多条，所以用了循环
    for (size_t i = 0; i < xResult.size(); ++i)
    {
        for (size_t j = 0; j < fieldVec.size(); ++j)
        {
            const std::string& strFieldName = fieldVec[j];
            std::string strValue(xResult[i][strFieldName.data()].data(), xResult[i][strFieldName.data()].length());
            valueVec.push_back(strValue);
        }
    }
    ARK_MYSQL_TRY_END("query error")

    return true;
}

bool AFCMysqlDriverModule::UpdateOrInsert(const std::string& tableName, const std::string& strKey,
    const std::vector<std::string>& fieldVec, const std::vector<std::string>& valueVec)
{
    if (!mysql_conn_)
    {
        return false;
    }

    if (fieldVec.size() != valueVec.size())
    {
        return false;
    }

    bool b_exist = false;
    if (!Exists(tableName, strKey, b_exist))
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    if (b_exist)
    {
        // update
        query << "UPDATE " << tableName << " SET ";
        for (size_t i = 0; i < fieldVec.size(); ++i)
        {
            if (i == 0)
            {
                query << fieldVec[i] << " = " << mysqlpp::quote << valueVec[i];
            }
            else
            {
                query << "," << fieldVec[i] << " = " << mysqlpp::quote << valueVec[i];
            }
        }

        query << " WHERE " << DefaultKey << " = " << mysqlpp::quote << strKey << ";";
    }
    else
    {
        // insert
        query << "INSERT INTO " << tableName << "(" << DefaultKey << ",";
        for (size_t i = 0; i < fieldVec.size(); ++i)
        {
            if (i == 0)
            {
                query << fieldVec[i];
            }
            else
            {
                query << ", " << fieldVec[i];
            }
        }

        query << ") VALUES(" << mysqlpp::quote << strKey << ",";
        for (size_t i = 0; i < valueVec.size(); ++i)
        {
            if (i == 0)
            {
                query << mysqlpp::quote << valueVec[i];
            }
            else
            {
                query << ", " << mysqlpp::quote << valueVec[i];
            }
        }

        query << ");";
    }

    query.execute();
    query.reset();
    ARK_MYSQL_TRY_END("update or insert error")

    return true;
}

bool AFCMysqlDriverModule::Select(const std::string& tableName, const std::string& strKey,
    const std::vector<std::string>& fieldVec, std::vector<std::string>& valueVec)
{
    return false;
}

bool AFCMysqlDriverModule::Delete(const std::string& tableName, const std::string& strKey)
{
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "DELETE FROM " << tableName << " WHERE " << DefaultKey << " = " << mysqlpp::quote << strKey << ";";

    query.execute();
    query.reset();
    ARK_MYSQL_TRY_END("delete error")

    return true;
}

bool AFCMysqlDriverModule::Exists(const std::string& tableName, const std::string& strKey, bool& bExit)
{
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "SELECT 1 FROM " << tableName << " WHERE " << DefaultKey << " = " << mysqlpp::quote << strKey
          << " LIMIT 1;";

    //query.execute();
    mysqlpp::StoreQueryResult result = query.store();
    query.reset();

    if (!result || result.empty())
    {
        bExit = false;
        return true;
    }

    ARK_MYSQL_TRY_END("exist error")

    bExit = true;
    return true;
}

bool AFCMysqlDriverModule::Keys(
    const std::string& tableName, const std::string& strKeyName, std::vector<std::string>& valueVec)
{
    if (!mysql_conn_)
    {
        return false;
    }

    const std::string strLikeKey = "%" + strKeyName + "%";

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "SELECT " << DefaultKey << " FROM " << tableName << " WHERE " << DefaultKey << " LIKE " << mysqlpp::quote
          << strLikeKey << " LIMIT 100;";

    mysqlpp::StoreQueryResult xResult = query.store();
    query.reset();

    if (xResult.empty() || !xResult)
    {
        return false;
    }

    // xResult应该只有一行的，为了以后可能出现的多条，所以用了循环
    for (size_t i = 0; i < xResult.size(); ++i)
    {
        std::string strValue(xResult[i][DefaultKey.data()].data(), xResult[i][DefaultKey.data()].length());
        valueVec.push_back(strValue);
    }

    ARK_MYSQL_TRY_END("exist error")

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AFCMysqlDriverModule::Insert(const std::string& document, std::vector<KVPair>& data)
{
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "INSERT INTO " << document << " (";

    auto iter = data.begin();
    while (iter != data.end())
    {
        if (iter != data.begin())
        {
            query << ", ";
        }
        query << iter->key_;
        iter++;
    }
    query << ") VALUES (";

    iter = data.begin();
    while (iter != data.end())
    {
        if (iter != data.begin())
        {
            query << ", ";
        }
        query << mysqlpp::quote << iter->value_;
        iter++;
    }
    query << ");";

    query.execute();
    query.reset();
    return true;
    ARK_MYSQL_TRY_END("mysql insert error")
}

bool AFCMysqlDriverModule::UpdateRecord(
    const std::string& document, std::vector<KVPair>& data, std::vector<where>& where)
{
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "UPDATE " << document << " SET ";

    auto iter = data.begin();
    while (iter != data.end())
    {
        if (iter != data.begin())
        {
            query << " , ";
        }
        query << iter->key_ << " = " << mysqlpp::quote << iter->value_;
        iter++;
    }
    query << " WHERE ";

    auto where_iter = where.begin();
    while (where_iter != where.end())
    {
        if (where_iter != where.begin())
        {
            query << " , ";
        }
        query << where_iter->key_ << where_iter->Operation() << mysqlpp::quote << where_iter->value_;
        where_iter++;
    }
    query << ";";

    query.execute();
    query.reset();
    return true;
    ARK_MYSQL_TRY_END("mysql set (update or insert) error")
}

bool AFCMysqlDriverModule::Select(const std::string& document, std::vector<std::string>& columns,
    std::vector<where>& where, OrderBy& order, int limit, ResultArr& results)
{
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "SELECT ";

    for (size_t i = 0; i < columns.size(); ++i)
    {
        if (i != 0)
        {
            query << ", ";
        }
        query << columns[i];
    }

    query << " FROM " << document << " WHERE ";

    auto iter = where.begin();
    while (iter != where.end())
    {
        if (iter != where.begin())
        {
            query << " AND ";
        }
        query << iter->key_ << iter->Operation() << mysqlpp::quote << iter->value_;
        iter++;
    }

    // order by
    for (size_t i = 0; i < order.fields_.size(); ++i)
    {
        if (i == 0)
        {
            query << " ORDER BY ";
        }
        else if (i == order.fields_.size() - 1)
        {
            query << " " << order.order_;
        }
        else
        {
            query << ", ";
        }
        query << order.fields_[i];
    }

    // limit
    if (limit > 0)
    {
        query << "LIMIT " << limit;
    }

    query << ";";
    mysqlpp::StoreQueryResult query_result = query.store();
    query.reset();

    if (query_result.empty() || !query_result)
    {
        return false;
    }

    for (size_t i = 0; i < query_result.size(); ++i)
    {
        Result temp;
        for (size_t j = 0; j < columns.size(); ++j)
        {
            const std::string& strFieldName = columns[j];
            std::string strValue(
                query_result[i][strFieldName.data()].data(), query_result[i][strFieldName.data()].length());
            temp.insert(std::make_pair(columns[j], std::move(strValue)));
        }
        results.push_back(std::move(temp));
    }

    return true;
    ARK_MYSQL_TRY_END("mysql get (select) error")
}

bool AFCMysqlDriverModule::SelectOne(const std::string& document, std::vector<std::string>& columns,
    std::vector<where>& where, OrderBy& order, Result& result)
{
    std::vector<Result> res;
    if (!Select(document, columns, where, order, 1, res))
    {
        return false;
    }

    if (res.empty())
    {
        return false;
    }

    for (const auto& iter : res[0])
    {
        result.insert(std::make_pair(iter.first, iter.second));
    }

    return true;
}

} // namespace ark
