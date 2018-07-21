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

#include "SDK/Core/AFPlatform.hpp"

enum ARK_PROCESS_TYPE
{
    ARK_PROC_NONE           = 0,    //none
    //cluster level
    ARK_PROC_MASTER         = 1,    //master
    ARK_PROC_DIR            = 2,    //dir
    ARK_PROC_LOG            = 3,    //log
    ARK_PROC_ROUTER         = 4,    //router, world & cluster middle layer
    //ARK_PROC_IDIP           = 5,    //idip
    //ARK_PROC_CLUTER_RANK    = 6,    //cluster rank
    //ARK_PROC_CLUSTER_MAIL   = 7,    //cluster mail
    //ARK_PROC_CLUSTER_PUB    = 8,    //cluster public

    ARK_PROC_CLUSTER_MAX    = 99,   //max of cluster
    //world level
    ARK_PROC_WORLD          = 100,  //world
    ARK_PROC_GAME           = 101,  //game
    ARK_PROC_LOGIN          = 102,  //login
    ARK_PROC_PROXY          = 103,  //proxy
    ARK_PROC_DB             = 104,  //db proxy
    //ARK_PROC_RANK           = 105,  //rank
    //ARK_PROC_PUB            = 106,  //public
    //ARK_PROC_CS_PROXY       = 107,  //cs_proxy, produce cross-server things

    ARK_PROC_WORLD_MAX      = 199,  //max of world

    //db
    ARK_PROC_REDIS          = 200,  //redis server
    ARK_PROC_MYSQL          = 201,  //mysql server

    ARK_PROC_MAX            = 255,  //max of all processes
};