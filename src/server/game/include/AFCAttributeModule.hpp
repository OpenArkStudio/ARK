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
#include "game/interface/AFIPropertyConfigModule.hpp"
#include "game/interface/AFIAttributeModule.hpp"
#include "game/interface/AFILevelModule.hpp"

namespace ark {

class AFCAttributeModule final : public AFIAttributeModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;

    virtual int RefreshBaseAttribute(const AFGUID& self);

    virtual int GetAttributeValue(
        const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type);
    virtual int SetAttributeValue(
        const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type, const int32_t value);

    virtual int AddAttributeValue(
        const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type, const int32_t value);
    virtual int SubAttributeValue(
        const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type, const int32_t value);

    virtual bool FullHPMP(const AFGUID& self);
    virtual bool AddHP(const AFGUID& self, const int32_t& value);
    virtual bool ConsumeHP(const AFGUID& self, const int32_t& value);
    virtual bool EnoughHP(const AFGUID& self, const int32_t& value);

    virtual bool AddMP(const AFGUID& self, const int32_t& value);
    virtual bool ConsumeMP(const AFGUID& self, const int32_t& value);
    virtual bool EnoughMP(const AFGUID& self, const int32_t& value);

    virtual bool AddMoney(const AFGUID& self, const int32_t& value);
    virtual bool ConsumeMoney(const AFGUID& self, const int32_t& value);
    virtual bool EnoughMoney(const AFGUID& self, const int32_t& value);

    virtual bool AddDiamond(const AFGUID& self, const int32_t& value);
    virtual bool ConsumeDiamond(const AFGUID& self, const int32_t& value);
    virtual bool EnoughDiamond(const AFGUID& self, const int32_t& value);

protected:
    int OnObjectClassEvent(
        const AFGUID& self, const std::string& class_name, const ArkEntityEvent event, const AFIDataList& var);

    int OnObjectLevelEvent(
        const AFGUID& self, const std::string& strPropertyName, const uint32_t index, const AFIData& old_var, const AFIData& new_var);

    int OnAttributeTableEvent(
        const AFGUID& self, const TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);

    const std::string& ColToAttributeName(const size_t col);
    int AttributeNameToCol(const std::string& class_name);

private:
    AFIKernelModule* m_pKernelModule;
    AFIPropertyConfigModule* m_pPropertyConfigModule;
    AFILevelModule* m_pLevelModule;

    std::map<std::string, int> name2col_;
    std::vector<std::string> col2names_;
};

} // namespace ark