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

mysql_driver_module* mysql_driver_manager_module_impl::GetMySQLDriver() {
    return mysql_modules_.begin()->second;
}

void mysql_driver_manager_module_impl::CheckMySQL() {
    int nServerID = 0;
    std::vector<int> xIntVec;
    for (auto iter : mysql_modules_)
    {
        auto pMysqlDriver = iter.second;
        if (!pMysqlDriver->IsAvailable())
        {
            xIntVec.push_back(nServerID);
            invalid_mysql_modules_.insert(nServerID, pMysqlDriver);
        }
    }

    for (auto i : xIntVec)
    {
        mysql_modules_.erase(xIntVec[i]);
    }

    xIntVec.clear();
    nServerID = 0;

    for (auto iter : invalid_mysql_modules_)
    {
        auto pMysqlDriver = iter.second;
        if (!pMysqlDriver->IsAvailable() && pMysqlDriver->CanReconnect())
        {
            pMysqlDriver->Reconnect();
            if (pMysqlDriver->IsAvailable())
            {
                xIntVec.push_back(nServerID);
                mysql_modules_.insert(nServerID, pMysqlDriver);
            }
        }
    }

    for (size_t i : xIntVec)
    {
        invalid_mysql_modules_.erase(xIntVec[i]);
    }
}

bool mysql_driver_manager_module_impl::AddMySQLServer(const int nServerID, const std::string& strDns,
    const std::string& strIP, const int nPort, const std::string strDBName, const std::string strDBUser,
    const std::string strDBPwd, const int nRconnectTime, const int nRconneCount) {
    std::string strDnsIp;
    if (!strDns.empty())
    {
        strDnsIp = GetIPByHostName(strDns);
    }

    if (strDnsIp.empty())
    {
        strDnsIp = strIP;
    }

    if (strDnsIp.empty())
    {
        return false;
    }

    if (strDnsIp.empty())
    {
        return false;
    }

    mysql_driver_module* pMysqlDriver = mysql_modules_.find_value(nServerID);
    if (pMysqlDriver)
    {
        return false;
    }

    mysql_driver_module* pInvalidRedisDriver = invalid_mysql_modules_.find_value(nServerID);
    if (pInvalidRedisDriver)
    {
        return false;
    }

    pMysqlDriver = ARK_NEW mysql_driver_module_impl(nRconnectTime, nRconneCount);
    if (pMysqlDriver->Connect(strDBName, strDnsIp, nPort, strDBUser, strDBPwd))
    {
        mysql_modules_.insert(nServerID, pMysqlDriver);
    }
    else
    {
        invalid_mysql_modules_.insert(nServerID, pMysqlDriver);
    }

    return true;
}

std::string mysql_driver_manager_module_impl::GetIPByHostName(const std::string& strHostName) {
    if (strHostName.empty())
    {
        return NULL_STR;
    }

    hostent* pHost = gethostbyname(strHostName.c_str());
    if (!pHost)
    {
        return NULL_STR;
    }

    if (pHost->h_addr_list[0])
    {
        char strIp[32] = {0};
        inet_ntop(pHost->h_addrtype, pHost->h_addr_list[0], strIp, sizeof(strIp));
        return std::string(strIp);
    }

    return NULL_STR;
}

} // namespace ark
