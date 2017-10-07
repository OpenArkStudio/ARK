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

#include "AFCPropertyConfigModule.h"
#include "SDK/Interface/AFIPluginManager.h"

bool AFCPropertyConfigModule::Init()
{
    return true;
}

bool AFCPropertyConfigModule::Shut()
{
    return true;
}

bool AFCPropertyConfigModule::Execute()
{
    return true;
}

bool AFCPropertyConfigModule::AfterInit()
{
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();

    Load();

    return true;
}

int AFCPropertyConfigModule::CalculateBaseValue(const int nJob, const int nLevel, const std::string& strProperty)
{
    ARK_SHARE_PTR <AFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);
    if(xPropertyMap)
    {
        ARK_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);
        if(xRefPropertyIDName)
        {
            return m_pElementModule->GetPropertyInt(*xRefPropertyIDName, strProperty);
        }
    }

    return 0;
}

void AFCPropertyConfigModule::Load()
{
    ARK_SHARE_PTR<AFIClass> pLogicClass = m_pClassModule->GetElement(NFrame::InitProperty::ThisName());
    if(nullptr != pLogicClass)
    {
        NFList<std::string>& xList = pLogicClass->GetConfigNameList();
        std::string strData;
        bool bRet = xList.First(strData);
        while(bRet)
        {
            ARK_SHARE_PTR<AFIPropertyMgr> pPropertyManager = m_pElementModule->GetPropertyManager(strData);
            if(nullptr != pPropertyManager)
            {
                int nJob = m_pElementModule->GetPropertyInt(strData, NFrame::InitProperty::Job());
                int nLevel = m_pElementModule->GetPropertyInt(strData, NFrame::InitProperty::Level());
                std::string strEffectData = m_pElementModule->GetPropertyString(strData, NFrame::InitProperty::EffectData());

                ARK_SHARE_PTR <AFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);
                if(!xPropertyMap)
                {
                    xPropertyMap = ARK_SHARE_PTR<AFMapEx<int, std::string>>(ARK_NEW AFMapEx<int, std::string>());
                    mhtCoefficienData.AddElement(nJob, xPropertyMap);

                    ARK_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);
                    if(!xRefPropertyIDName)
                    {
                        xRefPropertyIDName = ARK_SHARE_PTR<std::string>(ARK_NEW std::string(strEffectData));
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
    if(xPropertyMap)
    {
        ARK_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);
        if(xRefPropertyIDName)
        {
            return true;
        }
    }


    return false;
}

