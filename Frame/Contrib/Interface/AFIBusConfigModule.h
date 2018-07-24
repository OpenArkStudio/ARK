/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "SDK/Interface/AFIModule.h"
#include "Server/Interface/AFApp.hpp"
#include "AFIProcConfigModule.h"

union AFBusAddr
{
    AFBusAddr() = default;

    explicit AFBusAddr(const int id)
    {
        bus_id = id;
    }

    explicit AFBusAddr(const uint8_t c_id, const uint8_t z_id, const uint8_t p_id, const uint8_t i_id)
    {
        channel_id = c_id;
        zone_id = z_id;
        proc_id = p_id;
        inst_id = i_id;
    }

    inline AFBusAddr& operator=(int rhs)
    {
        this->bus_id = rhs;
        return *this;
    }

    int bus_id;
    struct
    {
        uint8_t channel_id;    //渠道id
        uint8_t zone_id;       //小区id
        uint8_t proc_id;       //进程id
        uint8_t inst_id;       //进程实例id
    };
};

class AFBusRelation
{
public:
    AFBusRelation() :
        proc_type(ARK_PROC_NONE),
        target_proc_type(ARK_PROC_NONE),
        connection_type(ARK_CONNECTION_NONE)
    {
    }

    ARK_PROCESS_TYPE proc_type;
    ARK_PROCESS_TYPE target_proc_type;
    ARK_CONNECTION_TYPE connection_type;
};

class AFIBusConfigModule : public AFIModule
{
public:
    virtual bool GetBusRelation(const ARK_PROCESS_TYPE& target_type, const int& inst_id, ARK_CONNECTION_TYPE& connect_type, AFHostConfig& host_config) = 0;
    virtual bool GetBusServer(const ARK_PROCESS_TYPE& type, const uint8_t inst_id, AFHostConfig& host_config) = 0;

    virtual const ARK_PROCESS_TYPE GetSelfProcType() = 0;
    virtual const AFBusAddr GetSelfBusID() = 0;
};

//<!--
//    config - path = "bus_relation.xml" id = "1001" machine1 = "1001A@xxx.com" machine2 = "1001B@xxx.com"
//    config launch 1001
//    Game OK
//    Login Ok
//
//    web tools
//    1.0.0.1
//    server 1.0.0.1
//    1001
//    A
//    B
//
//
//    web tool->web server ====ws====> config_tool->config_agent  version
//    外网          内网                    运维网         运维网
//    ||
//    ||==== > platform->project_idip
//    game1 game.pid.9.1.1.1->yyyy
//    game2 game.pid.9.1.1.2->xxxx
//-- >