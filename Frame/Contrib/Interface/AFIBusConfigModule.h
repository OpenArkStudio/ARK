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

class AFIBusConfigModule : public AFIModule
{
public:
    virtual bool GetBusRelation(const std::string& name, const int& id, std::string& server, int& port) = 0;
    virtual bool GetBusServer(const std::string& name, const int& id, std::string& ip, int& port) = 0;
};

//<bus_relation>
//    <relations>
//      <relation name = "mail_proxy" server = "mail_master" id = "100" count = "4" / >
//      <relation name = "mail_shard" server = "mail_master" id = "200" count = "2" / > <!--2001 2002 -->
//    </relations>
//    <servers>
//      <server name = "mail_master" ip = "127.0.0.1" port = "9001" machine = "xxxx" / >
//    </servers>
//</bus_relation>
//
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
//    ÍâÍø          ÄÚÍø                    ÔËÎ¬Íø         ÔËÎ¬Íø
//    ||
//    ||==== > platform->project_idip
//    game1 game.pid.9.1.1.1->yyyy
//    game2 game.pid.9.1.1.2->xxxx
//-- >