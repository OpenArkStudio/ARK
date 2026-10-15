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

#include "ark/mysql/mysql_driver_module.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

#define ARK_MYSQL_TRY_BEGIN                                                                                            \
    try                                                                                                                \
    {

#define ARK_MYSQL_TRY_END(msg)                                                                                         \
    }                                                                                                                  \
    catch (mysqlpp::BadQuery & er) {                                                                                   \
        std::cout << "BadQuery [" << msg << "] Error: " << er.what() << std::endl;                                     \
        return false;                                                                                                  \
    }                                                                                                                  \
    catch (const mysqlpp::BadConversion& er) {                                                                         \
        std::cout << "BadConversion [" << msg << "] Error:" << er.what() << " retrieved data size:" << er.retrieved    \
                  << ", actual size:" << er.actual_size << std::endl;                                                  \
        return false;                                                                                                  \
    }                                                                                                                  \
    catch (const mysqlpp::Exception& er) {                                                                             \
        std::cout << "mysqlpp::Exception [" << msg << "] Error:" << er.what() << std::endl;                            \
        return false;                                                                                                  \
    }                                                                                                                  \
    catch (...) {                                                                                                      \
        std::cout << "std::exception [" << msg << "] Error:Unknown " << std::endl;                                     \
        return false;                                                                                                  \
    }

#define ARK_MYSQL_TRY_END_NULL_RETURN(msg)                                                                             \
    }                                                                                                                  \
    catch (mysqlpp::BadQuery er) {                                                                                     \
        std::cout << "BadQuery [" << msg << "] Error: " << er.what() << std::endl;                                     \
        return;                                                                                                        \
    }                                                                                                                  \
    catch (const mysqlpp::BadConversion& er) {                                                                         \
        std::cout << "BadConversion [" << msg << "] Error:" << er.what() << " retrieved data size:" << er.retrieved    \
                  << ", actual size:" << er.actual_size << std::endl;                                                  \
        return;                                                                                                        \
    }                                                                                                                  \
    catch (const mysqlpp::Exception& er) {                                                                             \
        std::cout << "mysqlpp::Exception [" << msg << "] Error:" << er.what() << std::endl;                            \
        return;                                                                                                        \
    }                                                                                                                  \
    catch (...) {                                                                                                      \
        std::cout << "std::exception [" << msg << "] Error:Unknown " << std::endl;                                     \
        return;                                                                                                        \
    }

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class mysql_driver_module_impl final : public mysql_driver_module
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

    const static std::string default_key_;
    const static std::string default_table_;

public:
    explicit mysql_driver_module_impl(int reconnect_time = 60, int reconnect_count = -1);
    mysql_driver_module_impl(const std::string& db_name, const std::string& db_host, int db_port,
        const std::string& db_user, const std::string& db_pwd);
    ~mysql_driver_module_impl();

    bool update() override;

    bool connect(const std::string& db_name, const std::string& db_host, const int db_port, const std::string& db_user,
        const std::string& db_pwd) override;
    void close_connection() override;
    mysqlpp::Connection* get_connection() override;

    bool is_available() override;
    bool can_reconnect() override;
    bool reconnect() override;

    bool query(const std::string& qstr) override;
    bool query(const std::string& qstr, mysqlpp::StoreQueryResult& query_result) override;
    bool query(const std::string& table_name, const std::string& key, const std::vector<std::string>& field_vec,
        std::vector<std::string>& value_vec) override;
    bool update_or_insert(const std::string& table_name, const std::string& key,
        const std::vector<std::string>& field_vec, const std::vector<std::string>& value_vec) override;
    bool select(const std::string& table_name, const std::string& key, const std::vector<std::string>& field_vec,
        std::vector<std::string>& value_vec) override;
    bool erase(const std::string& table_name, const std::string& key) override;
    bool exists(const std::string& table_name, const std::string& key, bool& exists) override;
    bool keys(const std::string& table_name, const std::string& key_name, std::vector<std::string>& value_vec) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool insert(const std::string& document, std::vector<kv_pair>& data) override;
    bool update_record(const std::string& document, std::vector<kv_pair>& data, std::vector<where>& where) override;
    bool select(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        order_by& order, int limit, result_arr& results) override;
    bool select_one(const std::string& document, std::vector<std::string>& columns, std::vector<where>& where,
        order_by& order, result& result) override;

protected:
    bool is_need_reconnect();
    bool connect();
};

} // namespace ark
