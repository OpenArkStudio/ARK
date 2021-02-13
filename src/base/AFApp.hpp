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

namespace ark {

enum class ARK_APP_TYPE : uint8_t
{
    ARK_APP_DEFAULT, // none
    ARK_APP_MASTER,  // master //cluster level start
    ARK_APP_ROUTER,  // router, world & cluster middle layer
    //ARK_APP_OSS,              // oss
    //ARK_APP_DIR,              // dir
    //ARK_APP_LOG,              // log
    //ARK_APP_CLUTER_RANK,      //cluster rank
    //ARK_APP_CLUSTER_MAIL,     //cluster mail
    //ARK_PROC_CLUSTER_PUB,     //cluster public
    ARK_APP_CLUSTER_MAX = 29, // max of cluster
    ARK_APP_WORLD,            // world // zone level start
    ARK_APP_GAME,             // game
    ARK_APP_LOGIN,            // login
    ARK_APP_PROXY,            // proxy
    ARK_APP_DB,               // db-proxy
    //ARK_APP_RANK,             // rank
    //ARK_APP_PUB,              // public
    //ARK_APP_CS_PROXY,         // cs_proxy, produce cross-server things
    ARK_APP_WORLD_MAX = 199, // max of world
    ARK_APP_REDIS,           // Redis server // others start
    ARK_APP_MYSQL,           // MySQL server
    ARK_APP_MAX = 255,       // max of all processes
};

} // namespace ark
