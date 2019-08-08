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

#include "base/AFPluginManager.hpp"
#include "proto/AFProtoCPP.hpp"
#include "interface/AFIModule.hpp"
#include "kernel/interface/AFIKernelModule.hpp"
#include "kernel/interface/AFIConfigModule.hpp"
#include "utility/interface/AFIGUIDModule.hpp"
#include "game/interface/AFIAccountModule.hpp"

namespace ark {

class AFCAccountModule : public AFIAccountModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;

    virtual bool GetRoleList(const std::string& strAccount, AFMsg::AckRoleLiteInfoList& xAckRoleLiteInfoList);
    virtual bool CreateRole(
        const std::string& strAccount, AFMsg::AckRoleLiteInfoList& xAckRoleLiteInfoList, const AFIDataList& varList);
    virtual bool DeleteRole(const std::string& strAccount, AFMsg::AckRoleLiteInfoList& xAckRoleLiteInfoList);

protected:
    int OnLoadRoleBeginEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnLoadRoleFinalEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnCreateRoleEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnDeleteRoleEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnAcountDisConnectEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

private:
    AFIKernelModule* m_pKernelModule;
    AFIConfigModule* m_pElementInfoModule;
    AFIGUIDModule* m_pUUIDModule;
};

} // namespace ark
