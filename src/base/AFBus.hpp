/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "base/AFPlatform.hpp"
#include "base/AFMacros.hpp"
#include "base/AFApp.hpp"
#include "base/AFSocketFunc.hpp"
#include "base/AFStringUtils.hpp"

namespace ark {

// bus address, like IP address, 8.8.8.8
class AFBusAddr
{
public:
    bus_id_t bus_id{ 0 };

    uint8_t channel_id; // channel id
    uint8_t zone_id;    // zone id
    uint8_t app_type;   // application id
    uint8_t inst_id;    // instance id

public:
    AFBusAddr() = default;

    explicit AFBusAddr(const int id)
        : bus_id(id)
    {
        InitFromBusID();
    }

    explicit AFBusAddr(const uint8_t c_id, const uint8_t z_id, const uint8_t p_id, const uint8_t i_id)
        : channel_id(c_id)
        , zone_id(z_id)
        , app_type(p_id)
        , inst_id(i_id)
    {
        InitToBusID();
    }

    inline AFBusAddr& operator=(const int rhs)
    {
        this->bus_id = rhs;
        InitFromBusID();
        return *this;
    }

    inline std::string ToString()
    {
        std::string bus_name = ARK_FORMAT("{}.{}.{}.{}", channel_id, zone_id, app_type, inst_id);
        return bus_name;
    }

    bool FromString(const std::string& bus_name)
    {
        ARK_ASSERT_RET_VAL(!bus_name.empty(), false);

        std::vector<std::string> slices;
        AFStringUtils::split(slices, bus_name, '.');
        ARK_ASSERT_RET_VAL(slices.size() >= 4, false);

        channel_id = ARK_LEXICAL_CAST<uint8_t>(slices[0]);
        zone_id = ARK_LEXICAL_CAST<uint8_t>(slices[1]);
        app_type = ARK_LEXICAL_CAST<uint8_t>(slices[2]);
        inst_id = ARK_LEXICAL_CAST<uint8_t>(slices[3]);

        InitToBusID();

        return true;
    }

private:
    void InitToBusID()
    {
        bus_id = (bus_id_t(channel_id) << 24) + (bus_id_t(zone_id) << 16) + (bus_id_t(app_type) << 8) + bus_id_t(inst_id);
    }

    void InitFromBusID()
    {
        inst_id = uint8_t(bus_id & 0x000000FF);
        app_type = uint8_t((bus_id & 0x0000FF00) >> 8);
        zone_id = uint8_t((bus_id & 0x00FF0000) >> 16);
        channel_id = uint8_t((bus_id & 0xFF000000) >> 24);
    }
};

// bus relation, app connect other app with direct way or waiting sync message
class AFBusRelation
{
public:
    uint8_t app_type{std::underlying_type<ARK_APP_TYPE>::type(ARK_APP_TYPE::ARK_APP_DEFAULT)};
    uint8_t target_app_type{std::underlying_type<ARK_APP_TYPE>::type(ARK_APP_TYPE::ARK_APP_DEFAULT)};
    bool connection_type{false};
};

class AFProcConfig
{
public:
    bus_id_t bus_id{0};
    uint32_t max_connection{0};
    uint8_t thread_num{0};
    AFEndpoint intranet_ep;
    AFEndpoint server_ep;
    // to add other fields
};

class AFRegCenter
{
public:
    std::string ip{};
    uint16_t port{8500};
    std::string service_name{};
    std::string check_interval{"10s"};
    std::string check_timeout{"3s"};
};

class AFAppConfig
{
public:
    AFRegCenter reg_center; // register center

    std::map<std::string, ARK_APP_TYPE> name2types; // app_name -> app_type
    std::map<ARK_APP_TYPE, std::string> type2names; // app_type -> app_name

    std::map<ARK_APP_TYPE, std::vector<ARK_APP_TYPE>> connection_relations; // app_type -> target_types

    AFProcConfig self_proc; // self process info

    std::map<bus_id_t, AFProcConfig> other_proc_list;
};

} // namespace ark
