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
#include "game/interface/AFIAttributeModule.hpp"
#include "game/interface/AFILevelModule.hpp"

namespace ark {

class AFCAttributeModule : public AFIAttributeModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;

    virtual int RefreshBaseAttribute(const AFGUID& self);

    virtual int GetAttributeValue(
        const AFGUID& self, const std::string& strPropertyName, const ArkAttributeGroup eGroupType);
    virtual int SetAttributeValue(const AFGUID& self, const std::string& strPropertyName,
        const ArkAttributeGroup eGroupType, const int32_t nValue);

    virtual int AddAttributeValue(const AFGUID& self, const std::string& strPropertyName,
        const ArkAttributeGroup eGroupType, const int32_t nValue);
    virtual int SubAttributeValue(const AFGUID& self, const std::string& strPropertyName,
        const ArkAttributeGroup eGroupType, const int32_t nValue);

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
        const AFGUID& self, const std::string& class_name, const ArkEntityEvent eClassEvent, const AFIDataList& var);

    int OnObjectLevelEvent(
        const AFGUID& self, const std::string& strPropertyName, const uint32_t index, const AFIData& oldVar, const AFIData& newVar);

    int OnAttributeTableEvent(
        const AFGUID& self, const TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);

    const std::string& ColToAttributeName(const size_t nCol);
    int AttributeNameToCol(const std::string& strClassName);

private:
    AFIKernelModule* m_pKernelModule;
    AFIPropertyConfigModule* m_pPropertyConfigModule;
    AFIConfigModule* m_pConfigModule;
    AFIClassMetaModule* m_pClassModule;
    AFILevelModule* m_pLevelModule;
    std::map<std::string, int> name2col_;
    std::vector<std::string> col2names_;
};

} // namespace ark
