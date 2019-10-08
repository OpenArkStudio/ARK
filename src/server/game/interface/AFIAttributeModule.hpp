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

#include "interface/AFIModule.hpp"

namespace ark {

class AFIAttributeModule : public AFIModule
{
public:
    enum class AttributeGroup
    {
        APG_CAREER,       // career attribute
        APG_EFFECT_VALUE, // the basic attribute effect 2rd attribute(calculate by formula)
        APG_REBORN_ADD,   // reborn attribute
        APG_EQUIP,        // equip attribute
        APG_EQUIP_AWARD,  // equip suit attribute
        APG_STATIC_BUFF,  // permanent buff(talent or something else)
        APG_RUNTIME_BUFF, // dynamic buff

        APG_ALL,
    };

    virtual int RefreshBaseAttribute(const AFGUID& self) = 0;
    virtual int GetAttributeValue(
        const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type) = 0;
    virtual int SetAttributeValue(
        const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type, const int32_t value) = 0;
    virtual int AddAttributeValue(
        const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type, const int32_t value) = 0;
    virtual int SubAttributeValue(
        const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type, const int32_t value) = 0;

    virtual bool FullHPMP(const AFGUID& self) = 0;
    virtual bool AddHP(const AFGUID& self, const int32_t& value) = 0;
    virtual bool ConsumeHP(const AFGUID& self, const int32_t& value) = 0;
    virtual bool EnoughHP(const AFGUID& self, const int32_t& value) = 0;

    virtual bool AddMP(const AFGUID& self, const int32_t& value) = 0;
    virtual bool ConsumeMP(const AFGUID& self, const int32_t& value) = 0;
    virtual bool EnoughMP(const AFGUID& self, const int32_t& value) = 0;

    virtual bool AddMoney(const AFGUID& self, const int32_t& value) = 0;
    virtual bool ConsumeMoney(const AFGUID& self, const int32_t& value) = 0;
    virtual bool EnoughMoney(const AFGUID& self, const int32_t& value) = 0;

    virtual bool AddDiamond(const AFGUID& self, const int32_t& value) = 0;
    virtual bool ConsumeDiamond(const AFGUID& self, const int32_t& value) = 0;
    virtual bool EnoughDiamond(const AFGUID& self, const int32_t& value) = 0;
};

} // namespace ark
