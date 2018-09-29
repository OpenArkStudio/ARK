/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "AFCPropertyConfigModule.h"
#include "SDK/Interface/AFIPluginManager.h"

namespace ark
{

    bool AFCPropertyConfigModule::Init()
    {
        m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
        m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();

        return true;
    }

    bool AFCPropertyConfigModule::PostInit()
    {
        Load();
        return true;
    }

    int AFCPropertyConfigModule::CalculateBaseValue(const int nJob, const int nLevel, const std::string& strProperty)
    {
        ARK_SHARE_PTR <AFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);

        if (xPropertyMap)
        {
            ARK_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);

            if (xRefPropertyIDName)
            {
                return m_pConfigModule->GetNodeInt(*xRefPropertyIDName, strProperty);
            }
        }

        return 0;
    }

    void AFCPropertyConfigModule::Load()
    {
        ARK_SHARE_PTR<AFIClass> pLogicClass = m_pClassModule->GetElement(ark::InitProperty::ThisName());

        if (nullptr != pLogicClass)
        {
            AFList<std::string>& xList = pLogicClass->GetConfigNameList();
            std::string strData;
            bool bRet = xList.First(strData);

            while (bRet)
            {
                ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager = m_pConfigModule->GetNodeManager(strData);

                if (pNodeManager != nullptr)
                {
                    int nJob = m_pConfigModule->GetNodeInt(strData, ark::InitProperty::Job());
                    int nLevel = m_pConfigModule->GetNodeInt(strData, ark::InitProperty::Level());
                    std::string strEffectData = m_pConfigModule->GetNodeString(strData, ark::InitProperty::EffectData());

                    ARK_SHARE_PTR <AFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);

                    if (!xPropertyMap)
                    {
                        xPropertyMap = std::make_shared<AFMapEx<int, std::string>>();
                        mhtCoefficienData.AddElement(nJob, xPropertyMap);

                        ARK_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);

                        if (!xRefPropertyIDName)
                        {
                            xRefPropertyIDName = std::make_shared<std::string>(strEffectData);
                            xPropertyMap->AddElement(nLevel, xRefPropertyIDName);
                        }
                    }
                }

                bRet = xList.Next(strData);
            }
        }
    }

    bool AFCPropertyConfigModule::LegalLevel(const int nJob, const int nLevel)
    {
        ARK_SHARE_PTR <AFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);

        if (xPropertyMap)
        {
            ARK_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);

            if (xRefPropertyIDName)
            {
                return true;
            }
        }


        return false;
    }

}