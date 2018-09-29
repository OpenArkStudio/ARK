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

#include "SDK/Core/AFMap.hpp"
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFINetServerManagerModule.h"
#include "SDK/Interface/AFINetServerService.h"

namespace ark
{

    class AFCNetServerManagerModule : public AFINetServerManagerModule
    {
    public:
        explicit AFCNetServerManagerModule() = default;

        bool Init() override;
        bool Update() override;
        bool Shut() override;

        int CreateServer() override;
        AFINetServerService* GetSelfNetServer() override;

    private:
        AFMap<int, AFINetServerService> _net_servers;

        AFIBusModule* m_pBusModule;
        AFILogModule* m_pLogModule;
    };

}