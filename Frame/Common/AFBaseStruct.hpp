/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

#include "Common/AFApp.hpp"
#include "SDK/Core/AFPlatform.hpp"
#include "SDK/Core/AFMacros.hpp"
#include "SDK/Core/AFMisc.hpp"
#include "SDK/Core/AFSocketFunc.hpp"

namespace ark
{

    //bus address, like IP address, 8.8.8.8
    union AFBusAddr
    {
        AFBusAddr() = default;

        explicit AFBusAddr(const int id)
        {
            bus_id = id;
        }

        explicit AFBusAddr(const uint8_t c_id, const uint8_t z_id, const uint8_t p_id, const uint8_t i_id) :
            channel_id(c_id),
            zone_id(z_id),
            proc_id(p_id),
            inst_id(i_id)
        {
        }

        inline AFBusAddr& operator=(const int rhs)
        {
            this->bus_id = rhs;
            return *this;
        }

        inline std::string ToString()
        {
            std::string bus_name = ARK_FORMAT("{}.{}.{}.{}", channel_id, zone_id, proc_id, inst_id);
            return bus_name;
        }

        int bus_id{ 0 };
        struct
        {
            uint8_t channel_id;    //渠道id
            uint8_t zone_id;       //小区id
            uint8_t proc_id;       //进程id
            uint8_t inst_id;       //进程实例id
        };
    };

    //bus relation, proc connect other proc with direct way or waiting sync msg
    class AFBusRelation
    {
    public:
        AFBusRelation() = default;

        uint8_t app_type{ ARK_APP_DEFAULT };
        uint8_t target_app_type{ ARK_APP_DEFAULT };
        bool connection_type{ false };
    };

    //
    class AFServerConfig
    {
    public:
        int self_id{ 0 };
        int max_connection{ 0 };
        uint8_t thread_num{ 0 };
        AFEndpoint local_ep_;
        AFEndpoint public_ep_;
        //to add other fields
    };

    class AFProcConfig
    {
    public:
        std::map<std::string, std::string> hosts;   //name -> private_ip
        std::map<std::string, uint8_t> proc_types;  //proc_name -> proc_id
        std::map<uint8_t, std::string> proc_names;  //proc_id -> proc_name
        std::map<uint8_t, std::vector<AFServerConfig>> instances;//proc_id -> proc_instances
    };

}