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
#include "kernel/interface/AFIKernelModule.hpp"
#include "kernel/interface/AFIConfigModule.hpp"
#include "kernel/interface/AFIClassMetaModule.hpp"
#include "game/interface/AFIPropertyConfigModule.hpp"
#include "game/interface/AFIPropertyModule.hpp"
#include "game/interface/AFILevelModule.hpp"

namespace ark {

class AFCPropertyModule : public AFIPropertyModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;

    virtual int RefreshBaseProperty(const AFGUID& self);

    virtual int GetPropertyValue(
        const AFGUID& self, const std::string& strPropertyName, const ArkPropertyGroup eGroupType);
    virtual int SetPropertyValue(const AFGUID& self, const std::string& strPropertyName,
        const ArkPropertyGroup eGroupType, const int32_t nValue);

    virtual int AddPropertyValue(const AFGUID& self, const std::string& strPropertyName,
        const ArkPropertyGroup eGroupType, const int32_t nValue);
    virtual int SubPropertyValue(const AFGUID& self, const std::string& strPropertyName,
        const ArkPropertyGroup eGroupType, const int32_t nValue);

    virtual bool FullHPMP(const AFGUID& self);
    virtual bool AddHP(const AFGUID& self, const int32_t& nValue);
    virtual bool ConsumeHP(const AFGUID& self, const int32_t& nValue);
    virtual bool EnoughHP(const AFGUID& self, const int32_t& nValue);

    virtual bool AddMP(const AFGUID& self, const int32_t& nValue);
    virtual bool ConsumeMP(const AFGUID& self, const int32_t& nValue);
    virtual bool EnoughMP(const AFGUID& self, const int32_t& nValue);

    virtual bool AddMoney(const AFGUID& self, const int32_t& nValue);
    virtual bool ConsumeMoney(const AFGUID& self, const int32_t& nValue);
    virtual bool EnoughMoney(const AFGUID& self, const int32_t& nValue);

    virtual bool AddDiamond(const AFGUID& self, const int32_t& nValue);
    virtual bool ConsumeDiamond(const AFGUID& self, const int32_t& nValue);
    virtual bool EnoughDiamond(const AFGUID& self, const int32_t& nValue);

protected:
    int OnObjectClassEvent(
        const AFGUID& self, const std::string& strClassName, const ArkEntityEvent eClassEvent, const AFIDataList& var);

    int OnObjectLevelEvent(
        const AFGUID& self, const std::string& strPropertyName, const uint32_t index, const AFIData& oldVar, const AFIData& newVar);

    int OnPropertyTableEvent(
        const AFGUID& self, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);

    const std::string& ColToPropertyName(const size_t nCol);
    int PropertyNameToCol(const std::string& strClassName);

private:
    AFIKernelModule* m_pKernelModule;
    AFIPropertyConfigModule* m_pPropertyConfigModule;
    AFIConfigModule* m_pConfigModule;
    AFIClassMetaModule* m_pClassModule;
    AFILevelModule* m_pLevelModule;
    std::map<std::string, int> mNameToCol;
    std::vector<std::string> mColToName;
};

} // namespace ark
