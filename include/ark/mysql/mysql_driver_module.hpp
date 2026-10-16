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

namespace ark {

struct kv_pair
{
    kv_pair() {
    }

    kv_pair(std::string k, std::string v)
        : key(k)
        , value(v) {
    }

    std::string key;
    std::string value;
};

struct where
{
    enum op
    {
        equal,
    };

    where(std::string k, std::string v, op op = equal)
        : key(k)
        , value(v) {
    }

    std::string operation() {
        switch (comparison)
        {
        default:
            return "=";
        }
    }

    std::string key;
    std::string value;
    op comparison{equal};
};

struct order_by
{
    enum order
    {
        ascending,
        descending
    };

    order direction{ascending};
    std::vector<std::string> fields;
};

using result = std::unordered_map<std::string, std::string>;
using result_arr = std::vector<result>;

// AFIMysqlDriverModule defines the interface for other caller
// one driver defines a connection to mysql
class mysql_driver_module : public module
{
public:
    virtual bool connect(const std::string& db_name, const std::string& db_host, int db_port,
        const std::string& db_user, const std::string& db_pwd) = 0;
    virtual void close_connection() = 0;
    virtual mysqlpp::Connection* get_connection() = 0;

    // weather the connection is available
    virtual bool is_available() = 0;
    virtual bool reconnect() = 0;
    virtual bool can_reconnect() = 0;

    virtual bool query(const std::string& qstr, mysqlpp::StoreQueryResult& query_result) = 0;
    virtual bool query(const std::string& qstr) = 0;
    virtual bool query(const std::string& table_name, const std::string& key, const std::vector<std::string>& field_vec,
        std::vector<std::string>& value_vec) = 0;
    virtual bool update_or_insert(const std::string& table_name, const std::string& key,
        const std::vector<std::string>& field_vec, const std::vector<std::string>& value_vec) = 0;
    virtual bool select(const std::string& table_name, const std::string& key,
        const std::vector<std::string>& field_vec, std::vector<std::string>& value_vec) = 0;
    virtual bool erase(const std::string& table_name, const std::string& key) = 0;
    virtual bool exists(const std::string& table_name, const std::string& key, bool& exists) = 0;
    virtual bool keys(
        const std::string& table_name, const std::string& key_name, std::vector<std::string>& value_vec) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool insert(const std::string& document, std::vector<kv_pair>& data) = 0;
    virtual bool update_record(const std::string& document, std::vector<kv_pair>& data, std::vector<where>& where) = 0;
    virtual bool select(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        order_by& order, int limit, result_arr& results) = 0;
    virtual bool select_one(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        order_by& order, result& result) = 0;
};

} // namespace ark
