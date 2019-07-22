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

#include "base/AFApp.hpp"
#include "base/AFPlatform.hpp"
#include "base/AFMacros.hpp"
#include "AFSocketFunc.hpp"

namespace ark {

// bus address, like IP address, 8.8.8.8
union AFBusAddr
{
    AFBusAddr() = default;

    explicit AFBusAddr(const int id)
        : bus_id(id)
    {
    }

    explicit AFBusAddr(const uint8_t c_id, const uint8_t z_id, const uint8_t p_id, const uint8_t i_id)
        : channel_id(c_id)
        , zone_id(z_id)
        , app_type(p_id)
        , inst_id(i_id)
    {
    }

    inline AFBusAddr& operator=(const int rhs)
    {
        this->bus_id = rhs;
        return *this;
    }

    inline std::string ToString()
    {
        std::string bus_name = ARK_FORMAT("{}.{}.{}.{}", channel_id, zone_id, app_type, inst_id);
        return bus_name;
    }

    int bus_id{0};
    struct
    {
        uint8_t channel_id; // channel id
        uint8_t zone_id;    // zone id
        uint8_t app_type;   // application id
        uint8_t inst_id;    // instance id
    };
};

// bus relation, app connect other app with direct way or waiting sync message
class AFBusRelation
{
public:
    AFBusRelation() = default;

    uint8_t app_type{std::underlying_type<ARK_APP_TYPE>::type(ARK_APP_TYPE::ARK_APP_DEFAULT)};
    uint8_t target_app_type{std::underlying_type<ARK_APP_TYPE>::type(ARK_APP_TYPE::ARK_APP_DEFAULT)};
    bool connection_type{false};
};

class AFServerConfig
{
public:
    int self_id{0};
    uint32_t max_connection{0};
    uint8_t thread_num{0};
    AFEndpoint intranet_ep_;
    AFEndpoint public_ep_;
    // to add other fields
};

class AFProcConfig
{
public:
    std::map<std::string, std::string> hosts;                      // name -> intranet_ip
    std::map<std::string, ARK_APP_TYPE> app_name_types;            // app_name -> app_type
    std::map<ARK_APP_TYPE, std::string> app_type_names;            // app_type -> app_name
    std::map<ARK_APP_TYPE, std::vector<AFServerConfig>> instances; // app_type -> app_instances
};

} // namespace ark
