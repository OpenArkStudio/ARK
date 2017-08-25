/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @FileName  AFCElementModule.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFCElementModule                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFCElementModule                                                  *
*****************************************************************************/
#pragma once

#include <map>
#include <string>
#include <iostream>
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_iterators.hpp"
#include "RapidXML/rapidxml_print.hpp"
#include "RapidXML/rapidxml_utils.hpp"
#include "SDK/Base/AFMap.h"
#include "SDK/Base/AFList.h"
#include "SDK/Base/AFCDataList.h"
#include "SDK/Core/AFCRecord.h"
#include "SDK/Core/AFCPropertyMgr.h"
#include "SDK/Core/AFCRecordManager.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIClassModule.h"

class AFCClass;

class ElementConfigInfo
{
public:
    ElementConfigInfo()
    {
        m_pPropertyManager = ARK_SHARE_PTR<AFIPropertyMgr>(ARK_NEW AFCPropertyMgr(NULL_GUID));
        m_pRecordManager = ARK_SHARE_PTR<AFIRecordManager>(ARK_NEW AFCRecordManager(NULL_GUID));
    }

    virtual ~ElementConfigInfo()
    {
    }

    ARK_SHARE_PTR<AFIPropertyMgr> GetPropertyManager()
    {
        return m_pPropertyManager;
    }

    ARK_SHARE_PTR<AFIRecordManager> GetRecordManager()
    {
        return m_pRecordManager;
    }

protected:
    ARK_SHARE_PTR<AFIPropertyMgr> m_pPropertyManager;
    ARK_SHARE_PTR<AFIRecordManager> m_pRecordManager;
};

class AFCElementModule
    : public AFIElementModule,
      AFMapEx<std::string, ElementConfigInfo>
{
public:
    AFCElementModule(AFIPluginManager* p);
    virtual ~AFCElementModule();

    virtual bool Init();
    virtual bool Shut();

    virtual bool AfterInit();
    virtual bool BeforeShut();
    virtual bool Execute();

    virtual bool Load();
    virtual bool Save();
    virtual bool Clear();

    virtual bool ExistElement(const std::string& strConfigName);
    virtual bool ExistElement(const std::string& strClassName, const std::string& strConfigName);

    virtual ARK_SHARE_PTR<AFIPropertyMgr> GetPropertyManager(const std::string& strConfigName);
    virtual ARK_SHARE_PTR<AFIRecordManager> GetRecordManager(const std::string& strConfigName);

    virtual bool GetPropertyBool(const std::string& strConfigName, const std::string& strPropertyName);
    virtual int32_t GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName);
    virtual int64_t GetPropertyInt64(const std::string& strConfigName, const std::string& strPropertyName);
    virtual float GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName);
    virtual double GetPropertyDouble(const std::string& strConfigName, const std::string& strPropertyName);
    virtual const char*  GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName);

protected:
    virtual AFProperty* GetProperty(const std::string& strConfigName, const std::string& strPropertyName);

    virtual bool Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIClass> pLogicClass);

    virtual bool LegalNumber(const char* str);
protected:
    AFIClassModule* m_pClassModule;
    bool mbLoaded;
};
