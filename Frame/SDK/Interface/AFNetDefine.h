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
#include "SDK/Core/AFMacros.hpp"

ARK_CONSTEXPR static const int ARK_TCP_RECV_BUFFER_SIZE = 10 * 1024 * 1024; //10M
ARK_CONSTEXPR static const int ARK_HTTP_RECV_BUFFER_SIZE = 1024 * 1024; //1M

ARK_CONSTEXPR static const std::chrono::seconds ARK_NET_HEART_TIME = std::chrono::seconds(30);//30s
