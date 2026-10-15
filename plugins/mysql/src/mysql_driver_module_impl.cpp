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

#include "ark/mysql/mysql_driver_module_impl.hpp"

namespace ark {

const std::string mysql_driver_module_impl::default_key_ = "ID";
const std::string mysql_driver_module_impl::default_table_ = "RoleInfo";

mysql_driver_module_impl::mysql_driver_module_impl(int reconnect_time, int reconnect_count)
    : max_reconnect_time_(reconnect_time)
    , max_reconnect_count_(reconnect_count) {
}

mysql_driver_module_impl::mysql_driver_module_impl(const std::string& db_name, const std::string& db_host,
    const int db_port, const std::string& db_user, const std::string& db_pwd)
    : max_reconnect_time_(60)
    , max_reconnect_count_(-1) {

    connect(db_name, db_host, db_port, db_user, db_pwd);
}

mysql_driver_module_impl::~mysql_driver_module_impl() {
    close_connection();
}

bool mysql_driver_module_impl::update() {
    if (is_need_reconnect() && can_reconnect())
    {
        return connect(db_name_, db_host_, db_port_, db_user_, db_pwd_);
    }
    return true;
}

///////////////////////////////////////////////////////////////

bool mysql_driver_module_impl::connect(const std::string& db_name, const std::string& db_host, const int db_port,
    const std::string& db_user, const std::string& db_pwd) {
    db_name_ = db_name;
    db_host_ = db_host;
    db_port_ = db_port;
    db_user_ = db_user;
    db_pwd_ = db_pwd;

    return connect();
}

bool mysql_driver_module_impl::connect() {
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
        close_connection();
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

void mysql_driver_module_impl::close_connection() {
    mysql_conn_->disconnect();
    delete mysql_conn_;
    mysql_conn_ = nullptr;
}

mysqlpp::Connection* mysql_driver_module_impl::get_connection() {
    return mysql_conn_;
}

bool mysql_driver_module_impl::is_available() {
    return !is_need_reconnect();
}

bool mysql_driver_module_impl::reconnect() {
    close_connection();
    connect(db_name_, db_host_, db_port_, db_user_, db_pwd_);

    if (max_reconnect_count_ > 0)
    {
        max_reconnect_count_--;
    }

    return true;
}

bool mysql_driver_module_impl::can_reconnect() {
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

bool mysql_driver_module_impl::is_need_reconnect() {
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
        close_connection();
        return true;
    }

    if (!mysql_conn_->ping())
    {
        close_connection();
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////

bool mysql_driver_module_impl::query(const std::string& qstr) {
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

bool mysql_driver_module_impl::query(const std::string& qstr, mysqlpp::StoreQueryResult& query_result) {
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query(qstr);
    query_result = query.store();
    query.reset();
    ARK_MYSQL_TRY_END(qstr)

    return true;
}

bool mysql_driver_module_impl::query(const std::string& table_name, const std::string& key,
    const std::vector<std::string>& field_vec, std::vector<std::string>& value_vec) {
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "SELECT ";
    for (auto iter = field_vec.begin(); iter != field_vec.end(); ++iter)
    {
        if (iter == field_vec.begin())
        {
            query << *iter;
        }
        else
        {
            query << "," << *iter;
        }
    }
    query << " FROM " << table_name << " WHERE " << default_key_ << " = " << mysqlpp::quote << key << ";";
    //query.execute(); // 官网例子不需要execute
    mysqlpp::StoreQueryResult query_result = query.store();
    query.reset();

    if (query_result.empty() || !query_result)
    {
        return false;
    }

    // xResult应该只有一行的，为了以后可能出现的多条，所以用了循环
    for (size_t i = 0; i < query_result.size(); ++i)
    {
        for (size_t j = 0; j < field_vec.size(); ++j)
        {
            const std::string& field_name = field_vec[j];
            std::string value(query_result[i][field_name.data()].data(), query_result[i][field_name.data()].length());
            value_vec.push_back(value);
        }
    }
    ARK_MYSQL_TRY_END("query error")

    return true;
}

bool mysql_driver_module_impl::update_or_insert(const std::string& table_name, const std::string& key,
    const std::vector<std::string>& field_vec, const std::vector<std::string>& value_vec) {
    if (!mysql_conn_)
    {
        return false;
    }

    if (field_vec.size() != value_vec.size())
    {
        return false;
    }

    bool b_exist = false;
    if (!exists(table_name, key, b_exist))
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    if (b_exist)
    {
        // update
        query << "UPDATE " << table_name << " SET ";
        for (size_t i = 0; i < field_vec.size(); ++i)
        {
            if (i == 0)
            {
                query << field_vec[i] << " = " << mysqlpp::quote << value_vec[i];
            }
            else
            {
                query << "," << field_vec[i] << " = " << mysqlpp::quote << value_vec[i];
            }
        }

        query << " WHERE " << default_key_ << " = " << mysqlpp::quote << key << ";";
    }
    else
    {
        // insert
        query << "INSERT INTO " << table_name << "(" << default_key_ << ",";
        for (size_t i = 0; i < field_vec.size(); ++i)
        {
            if (i == 0)
            {
                query << field_vec[i];
            }
            else
            {
                query << ", " << field_vec[i];
            }
        }

        query << ") VALUES(" << mysqlpp::quote << key << ",";
        for (size_t i = 0; i < value_vec.size(); ++i)
        {
            if (i == 0)
            {
                query << mysqlpp::quote << value_vec[i];
            }
            else
            {
                query << ", " << mysqlpp::quote << value_vec[i];
            }
        }

        query << ");";
    }

    query.execute();
    query.reset();
    ARK_MYSQL_TRY_END("update or insert error")

    return true;
}

bool mysql_driver_module_impl::select(const std::string& table_name, const std::string& key,
    const std::vector<std::string>& field_vec, std::vector<std::string>& value_vec) {
    return false;
}

bool mysql_driver_module_impl::erase(const std::string& table_name, const std::string& key) {
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "DELETE FROM " << table_name << " WHERE " << default_key_ << " = " << mysqlpp::quote << key << ";";

    query.execute();
    query.reset();
    ARK_MYSQL_TRY_END("delete error")

    return true;
}

bool mysql_driver_module_impl::exists(const std::string& table_name, const std::string& key, bool& exists) {
    if (!mysql_conn_)
    {
        return false;
    }

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "SELECT 1 FROM " << table_name << " WHERE " << default_key_ << " = " << mysqlpp::quote << key
          << " LIMIT 1;";

    //query.execute();
    mysqlpp::StoreQueryResult result = query.store();
    query.reset();

    if (!result || result.empty())
    {
        exists = false;
        return true;
    }

    ARK_MYSQL_TRY_END("exist error")

    exists = true;
    return true;
}

bool mysql_driver_module_impl::keys(
    const std::string& table_name, const std::string& key_name, std::vector<std::string>& value_vec) {
    if (!mysql_conn_)
    {
        return false;
    }

    const std::string like_key = "%" + key_name + "%";

    ARK_MYSQL_TRY_BEGIN
    mysqlpp::Query query = mysql_conn_->query();
    query << "SELECT " << default_key_ << " FROM " << table_name << " WHERE " << default_key_ << " LIKE "
          << mysqlpp::quote << like_key << " LIMIT 100;";

    mysqlpp::StoreQueryResult query_result = query.store();
    query.reset();

    if (query_result.empty() || !query_result)
    {
        return false;
    }

    // xResult应该只有一行的，为了以后可能出现的多条，所以用了循环
    for (size_t i = 0; i < query_result.size(); ++i)
    {
        std::string value(query_result[i][default_key_.data()].data(), query_result[i][default_key_.data()].length());
        value_vec.push_back(value);
    }

    ARK_MYSQL_TRY_END("exist error")

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool mysql_driver_module_impl::insert(const std::string& document, std::vector<kv_pair>& data) {
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
        query << iter->key;
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
        query << mysqlpp::quote << iter->value;
        iter++;
    }
    query << ");";

    query.execute();
    query.reset();
    return true;
    ARK_MYSQL_TRY_END("mysql insert error")
}

bool mysql_driver_module_impl::update_record(
    const std::string& document, std::vector<kv_pair>& data, std::vector<where>& where) {
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
        query << iter->key << " = " << mysqlpp::quote << iter->value;
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
        query << where_iter->key << where_iter->operation() << mysqlpp::quote << where_iter->value;
        where_iter++;
    }
    query << ";";

    query.execute();
    query.reset();
    return true;
    ARK_MYSQL_TRY_END("mysql set (update or insert) error")
}

bool mysql_driver_module_impl::select(const std::string& document, std::vector<std::string>& columns,
    std::vector<where>& where, order_by& order, int limit, result_arr& results) {
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
        query << iter->key << iter->operation() << mysqlpp::quote << iter->value;
        iter++;
    }

    // order by
    for (size_t i = 0; i < order.fields.size(); ++i)
    {
        if (i == 0)
        {
            query << " ORDER BY ";
        }
        else if (i == order.fields.size() - 1)
        {
            query << " " << order.direction;
        }
        else
        {
            query << ", ";
        }
        query << order.fields[i];
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
        result temp;
        for (size_t j = 0; j < columns.size(); ++j)
        {
            const std::string& field_name = columns[j];
            std::string value(query_result[i][field_name.data()].data(), query_result[i][field_name.data()].length());
            temp.insert(std::make_pair(columns[j], std::move(value)));
        }
        results.push_back(std::move(temp));
    }

    return true;
    ARK_MYSQL_TRY_END("mysql get (select) error")
}

bool mysql_driver_module_impl::select_one(const std::string& document, std::vector<std::string>& columns,
    std::vector<where>& where, order_by& order, result& result) {
    std::vector<result> res;
    if (!select(document, columns, where, order, 1, res))
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
