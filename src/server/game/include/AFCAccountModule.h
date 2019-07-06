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

#include "proto/AFProtoCPP.hpp"
#include "interface/AFIPluginManager.h"
#include "kernel/interface/AFIKernelModule.h"
#include "kernel/interface/AFIConfigModule.h"
#include "utility/interface/AFIGUIDModule.h"
#include "game/interface/AFIAccountModule.h"

namespace ark {

class AFCAccountModule : public AFIAccountModule
{
public:
    bool Init() override;

    virtual bool GetRoleList(const std::string &strAccount, AFMsg::AckRoleLiteInfoList &xAckRoleLiteInfoList);
    virtual bool CreateRole(const std::string &strAccount, AFMsg::AckRoleLiteInfoList &xAckRoleLiteInfoList, const AFIDataList &varList);
    virtual bool DeleteRole(const std::string &strAccount, AFMsg::AckRoleLiteInfoList &xAckRoleLiteInfoList);

protected:
    int OnLoadRoleBeginEvent(const AFGUID &object, const int nEventID, const AFIDataList &var);

    int OnLoadRoleFinalEvent(const AFGUID &object, const int nEventID, const AFIDataList &var);

    int OnCreateRoleEvent(const AFGUID &object, const int nEventID, const AFIDataList &var);

    int OnDeleteRoleEvent(const AFGUID &object, const int nEventID, const AFIDataList &var);

    int OnAcountDisConnectEvent(const AFGUID &object, const int nEventID, const AFIDataList &var);

private:
    AFIKernelModule *m_pKernelModule;
    AFIConfigModule *m_pElementInfoModule;
    AFIGUIDModule *m_pUUIDModule;
};

} // namespace ark