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

enum ARK_SERVER_TYPE
{
    ARK_ST_NONE = 0,        //none
    //cluster
    ARK_ST_MASTER = 1,      //master
    ARK_ST_DIR = 2,         //dir
    ARK_ST_LOG = 3,         //log
    ARK_ST_ROUTER = 4,      //router, world & cluster middle layer
    ARK_IDIP = 5,           //idip
    //ARK_CLUTER_RANK         = 6,    //cluster rank
    //ARK_CLUSTER_MAIL        = 7,    //cluster mail
    //ARK_CLUSTER_PUB         = 8,    //cluster public

    //world
    ARK_ST_WORLD = 20,      //world
    ARK_ST_GAME = 21,       //game
    ARK_ST_LOGIN = 22,      //login
    ARK_ST_PROXY = 23,      //proxy
    ARK_ST_DB = 24,         //db proxy
    //ARK_ST_RANK = 25,       //rank
    //ARK_ST_PUB = 26,        //public
    //ARK_ST_CS_PROXY = 27,   //cs_proxy, produce cross-server things

    //db
    ARK_ST_REDIS = 100,    //
    ARK_ST_MYSQL = 101,    //
};