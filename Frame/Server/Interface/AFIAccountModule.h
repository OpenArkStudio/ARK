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

#include "Common/AFProtoCPP.hpp"
#include "SDK/Interface/AFIModule.h"

class AFIAccountModule
    : public AFIModule
{
public:
    virtual bool GetRoleList(const std::string& strAccount, AFMsg::AckRoleLiteInfoList& xAckRoleLiteInfoList) = 0;
    virtual bool CreateRole(const std::string& strAccount, AFMsg::AckRoleLiteInfoList& xAckRoleLiteInfoList, const AFIDataList& varList) = 0;
    virtual bool DeleteRole(const std::string& strAccount, AFMsg::AckRoleLiteInfoList& xAckRoleLiteInfoList) = 0;
};
