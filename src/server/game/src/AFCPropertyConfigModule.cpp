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

#include "game/include/AFCPropertyConfigModule.hpp"

namespace ark {

bool AFCPropertyConfigModule::Init()
{
    m_pKernelModule = FindModule<AFIKernelModule>();

    return true;
}

bool AFCPropertyConfigModule::PostInit()
{
    Load();
    return true;
}

int AFCPropertyConfigModule::CalculateBaseValue(const int nJob, const int nLevel, const std::string& strProperty)
{
    auto xPropertyMap = mhtCoefficienData.find_value(nJob);

    if (xPropertyMap != nullptr)
    {
        auto xRefPropertyIDName = xPropertyMap->find_value(nLevel);

        if (xRefPropertyIDName != nullptr)
        {
            // TODO : config id type is int
            //auto pConfig = m_pKernelModule->GetStaticEntity(*xRefPropertyIDName);
            //if (!pConfig)
            //{
            //    return 0;
            //}

            //return pConfig->GetInt32(strProperty);
        }
    }

    return 0;
}

void AFCPropertyConfigModule::Load()
{
    // std::shared_ptr<AFIMetaClass> pLogicClass = m_pClassModule->GetMetaClass(AFConfigMetaInitAttr::self_name());
    // if (nullptr == pLogicClass)
    //{
    //    return;
    //}

    // AFList<std::string>& xList = pLogicClass->GetConfigNameList();
    // for (auto iter : xList)
    //{
    //    auto& config_id = iter;
    //    std::shared_ptr<AFIDataNodeManager> pNodeManager = m_pConfigModule->GetNodeManager(config_id);

    //    if (pNodeManager == nullptr)
    //    {
    //        continue;
    //    }

    //    int nJob = m_pConfigModule->GetNodeInt(config_id, ark::InitProperty::Job());
    //    int nLevel = m_pConfigModule->GetNodeInt(config_id, ark::InitProperty::Level());
    //    std::string strEffectData = m_pConfigModule->GetNodeString(config_id, ark::InitProperty::EffectData());

    //    std::shared_ptr<AFMapEx<int, std::string>> xPropertyMap = mhtCoefficienData.find_value(nJob);
    //    if (xPropertyMap == nullptr)
    //    {
    //        xPropertyMap = std::make_shared<AFMapEx<int, std::string>>();
    //        mhtCoefficienData.insert(nJob, xPropertyMap);

    //        std::shared_ptr<std::string> xRefPropertyIDName = xPropertyMap->find_value(nLevel);
    //        if (xRefPropertyIDName == nullptr)
    //        {
    //            xRefPropertyIDName = std::make_shared<std::string>(strEffectData);
    //            xPropertyMap->insert(nLevel, xRefPropertyIDName);
    //        }
    //    }
    //}
}

bool AFCPropertyConfigModule::LegalLevel(const int nJob, const int nLevel)
{
    auto xPropertyMap = mhtCoefficienData.find_value(nJob);
    if (xPropertyMap != nullptr)
    {
        auto xRefPropertyIDName = xPropertyMap->find_value(nLevel);

        if (xRefPropertyIDName != nullptr)
        {
            return true;
        }
    }

    return false;
}

} // namespace ark
