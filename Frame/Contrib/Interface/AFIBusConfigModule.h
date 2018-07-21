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

union AFBusAddr
{
    int _bus_id;
    struct
    {
        uint8_t _channel_id;    //渠道id
        uint8_t _zone_id;       //小区id
        uint8_t _proc_id;       //进程id
        uint8_t _inst_id;       //进程实例id
    };
};

class AFIBusConfigModule : public AFIModule
{
public:
    enum EConnectionType
    {
        EConnectionType_WAIT    = 0,//wait for high level server sync other service
        EConnectionType_DIRECT  = 1,//connect target server directly
    };

    virtual bool GetBusRelation(const std::string& name, const int& id, std::string& server, int& port) = 0;
    virtual bool GetBusServer(const std::string& name, const int& id, std::string& ip, int& port) = 0;
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