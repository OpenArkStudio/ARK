/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

namespace ark
{
    //will modify to enum class with C++ mordern standard
    enum ARK_APP_TYPE
    {
        ARK_APP_DEFAULT         = 0,    //none
        //cluster level
        ARK_APP_MASTER          = 1,    //master
        ARK_APP_DIR             = 2,    //dir
        ARK_APP_LOG             = 3,    //log
        ARK_APP_ROUTER          = 4,    //router, world & cluster middle layer
        ARK_PROC_OSS            = 5,    //oss
        //ARK_PROC_CLUTER_RANK    = 6,    //cluster rank
        //ARK_PROC_CLUSTER_MAIL   = 7,    //cluster mail
        //ARK_PROC_CLUSTER_PUB    = 8,    //cluster public

        ARK_APP_CLUSTER_MAX     = 99,   //max of cluster
        //world level
        ARK_APP_WORLD           = 100,  //world
        ARK_APP_GAME            = 101,  //game
        ARK_APP_LOGIN           = 102,  //login
        ARK_APP_PROXY           = 103,  //proxy
        ARK_APP_DB              = 104,  //dbproxy
        //ARK_APP_RANK           = 105,  //rank
        //ARK_APP_PUB            = 106,  //public
        //ARK_APP_CS_PROXY       = 107,  //cs_proxy, produce cross-server things

        ARK_APP_WORLD_MAX       = 199,  //max of world

        //db
        ARK_APP_REDIS           = 200,  //redis server
        ARK_APP_MYSQL           = 201,  //mysql server

        ARK_APP_MAX             = 255,  //max of all processes
    };

}