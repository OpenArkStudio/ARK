/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "interface/AFIModule.h"

namespace ark
{

class AFIProxyNetModule : public AFIModule
{
public:
    virtual int Transpond(const AFNetMsg *msg) = 0;
    virtual int EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID) = 0;
    virtual int SendToPlayerClient(
        const int nMsgID, const char *msg, const uint32_t nLen, const AFGUID &nClientID, const AFGUID &nPlayer) = 0;

    virtual bool VerifyConnectData(const std::string &strAccount, const std::string &strKey) = 0;
};

} // namespace ark