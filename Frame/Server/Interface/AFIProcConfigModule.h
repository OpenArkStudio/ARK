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

class AFHostConfig
{
public:
    std::string private_ip;
    std::vector<std::string> public_ips;
};

class AFServerConfig
{
public:
    uint8_t inst_id;
    int max_connection;
    uint8_t thread_num;
    std::string host;
    uint16_t port;
    //to add other fields
};

class AFProcConfig
{
public:
    std::map<std::string, AFHostConfig> hosts;
    std::map<std::string, ARK_PROCESS_TYPE> server_types;
    std::map<ARK_PROCESS_TYPE, std::string> server_names;
    std::map<ARK_PROCESS_TYPE, std::vector<AFServerConfig>> servers;
};

class AFIProcConfigModule : public AFIModule
{
public:
    virtual ~AFIProcConfigModule() = default;

    virtual const std::string& GetProcName(const ARK_PROCESS_TYPE& type) = 0;
    virtual const ARK_PROCESS_TYPE GetProcType(const std::string& name) = 0;
    virtual bool GetProcServerInfo(const ARK_PROCESS_TYPE& type, uint8_t inst_id, AFServerConfig& server_config) = 0;
    virtual bool GetProcHostInfo(const ARK_PROCESS_TYPE& type, uint8_t inst_id, AFHostConfig& host_config) = 0;
};