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

#include "SDK/Interface/AFIModule.h"

namespace ark
{

    class AFIPropertyModule : public AFIModule
    {
    public:
        enum ArkPropertyGroup
        {
            APG_JOBLEVEL,        //职业等级基础
            APG_EFFECTVALUE,     //一级属性影响二级属性的部分(直接用公式计算，其他部分不用动)
            APG_REBIRTH_ADD,     //重生奖励
            APG_EQUIP,           //装备不会影响体质之类的导致2次计算(直接不要力量体质什么的)
            APG_EQUIP_AWARD,     //套装奖励
            APG_STATIC_BUFF,     //永久类型的BUFF，天赋之类的，或者吃的永久药水

            APG_RUNTIME_BUFF,    //动态BUFF

            APG_ALL,
        };


        virtual int RefreshBaseProperty(const AFGUID& self) = 0;
        virtual int GetPropertyValue(const AFGUID& self, const std::string& strPropertyName, const ArkPropertyGroup eGroupType) = 0;
        virtual int SetPropertyValue(const AFGUID& self, const std::string& strPropertyName, const ArkPropertyGroup eGroupType, const int nValue) = 0;
        virtual int AddPropertyValue(const AFGUID& self, const std::string& strPropertyName, const ArkPropertyGroup eGroupType, const int nValue) = 0;
        virtual int SubPropertyValue(const AFGUID& self, const std::string& strPropertyName, const ArkPropertyGroup eGroupType, const int nValue) = 0;

        virtual bool FullHPMP(const AFGUID& self) = 0;
        virtual bool AddHP(const AFGUID& self, const int64_t& nValue) = 0;
        virtual bool ConsumeHP(const AFGUID& self, const int64_t& nValue) = 0;
        virtual bool EnoughHP(const AFGUID& self, const int64_t& nValue) = 0;

        virtual bool AddMP(const AFGUID& self, const int64_t& nValue) = 0;
        virtual bool ConsumeMP(const AFGUID& self, const int64_t& nValue) = 0;
        virtual bool EnoughMP(const AFGUID& self, const int64_t& nValue) = 0;

        virtual bool FullSP(const AFGUID& self) = 0;
        virtual bool AddSP(const AFGUID& self, const int64_t& nValue) = 0;
        virtual bool ConsumeSP(const AFGUID& self, const int64_t& nValue) = 0;
        virtual bool EnoughSP(const AFGUID& self, const int64_t& nValue) = 0;

        virtual bool AddMoney(const AFGUID& self, const int64_t& nValue) = 0;
        virtual bool ConsumeMoney(const AFGUID& self, const int64_t& nValue) = 0;
        virtual bool EnoughMoney(const AFGUID& self, const int64_t& nValue) = 0;

        virtual bool AddDiamond(const AFGUID& self, const int64_t& nValue) = 0;
        virtual bool ConsumeDiamond(const AFGUID& self, const int64_t& nValue) = 0;
        virtual bool EnoughDiamond(const AFGUID& self, const int64_t& nValue) = 0;
    };

}