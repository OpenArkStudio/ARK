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

#include "ark/mysql/mysql_driver_manager_module_impl.hpp"
#include "ark/mysql/mysql_driver_module_impl.hpp"

namespace ark {

mysql_driver_module* mysql_driver_manager_module_impl::get_mysql_driver() {
    return mysql_modules_.begin()->second;
}

void mysql_driver_manager_module_impl::check_mysql() {
    int server_id = 0;
    std::vector<int> server_ids;
    for (auto iter : mysql_modules_)
    {
        auto mysql_driver_ptr = iter.second;
        if (!mysql_driver_ptr->is_available())
        {
            server_ids.push_back(server_id);
            invalid_mysql_modules_.insert(server_id, mysql_driver_ptr);
        }
    }

    for (auto i : server_ids)
    {
        mysql_modules_.erase(server_ids[i]);
    }

    server_ids.clear();
    server_id = 0;

    for (auto iter : invalid_mysql_modules_)
    {
        auto mysql_driver_ptr = iter.second;
        if (!mysql_driver_ptr->is_available() && mysql_driver_ptr->can_reconnect())
        {
            mysql_driver_ptr->reconnect();
            if (mysql_driver_ptr->is_available())
            {
                server_ids.push_back(server_id);
                mysql_modules_.insert(server_id, mysql_driver_ptr);
            }
        }
    }

    for (size_t i : server_ids)
    {
        invalid_mysql_modules_.erase(server_ids[i]);
    }
}

bool mysql_driver_manager_module_impl::add_mysql_server(const int server_id, const std::string& dns,
    const std::string& ip, const int port, const std::string db_name, const std::string db_user,
    const std::string db_pwd, const int reconnect_time, const int reconnect_count) {
    std::string dns_ip;
    if (!dns.empty())
    {
        dns_ip = get_ip_by_host_name(dns);
    }

    if (dns_ip.empty())
    {
        dns_ip = ip;
    }

    if (dns_ip.empty())
    {
        return false;
    }

    if (dns_ip.empty())
    {
        return false;
    }

    mysql_driver_module* mysql_driver_ptr = mysql_modules_.find_value(server_id);
    if (mysql_driver_ptr)
    {
        return false;
    }

    mysql_driver_module* invalid_redis_driver_ptr = invalid_mysql_modules_.find_value(server_id);
    if (invalid_redis_driver_ptr)
    {
        return false;
    }

    mysql_driver_ptr = ARK_NEW mysql_driver_module_impl(reconnect_time, reconnect_count);
    if (mysql_driver_ptr->connect(db_name, dns_ip, port, db_user, db_pwd))
    {
        mysql_modules_.insert(server_id, mysql_driver_ptr);
    }
    else
    {
        invalid_mysql_modules_.insert(server_id, mysql_driver_ptr);
    }

    return true;
}

std::string mysql_driver_manager_module_impl::get_ip_by_host_name(const std::string& host_name) {
    if (host_name.empty())
    {
        return null_str;
    }

    hostent* host_ptr = gethostbyname(host_name.c_str());
    if (!host_ptr)
    {
        return null_str;
    }

    if (host_ptr->h_addr_list[0])
    {
        char ip[32] = {0};
        inet_ntop(host_ptr->h_addrtype, host_ptr->h_addr_list[0], ip, sizeof(ip));
        return std::string(ip);
    }

    return null_str;
}

} // namespace ark
