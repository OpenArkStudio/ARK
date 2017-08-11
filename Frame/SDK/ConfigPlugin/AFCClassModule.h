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

#include "AFCElementModule.h"
#include "RapidXML/rapidxml.hpp"
#include "SDK/Base/AFMap.h"
#include "SDK/Base/AFList.h"
#include "SDK/Base/AFCDataList.h"
#include "SDK/Core/AFCRecord.h"
#include "SDK/Base/AFList.h"
#include "SDK/Core/AFCPropertyMgr.h"
#include "SDK/Core/AFCRecordManager.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIPluginManager.h"

class AFCClass : public AFIClass
{
public:

    AFCClass(const std::string& strClassName)
    {
        m_pParentClass = NULL;
        mstrClassName = strClassName;

        m_pPropertyManager = ARK_SHARE_PTR<AFIPropertyMgr>(ARK_NEW AFCPropertyMgr(NULL_GUID));
        m_pRecordManager = ARK_SHARE_PTR<AFIRecordManager>(ARK_NEW AFCRecordManager(NULL_GUID));
    }

    virtual ~AFCClass()
    {
        ClearAll();
    }

    virtual ARK_SHARE_PTR<AFIPropertyMgr> GetPropertyManager()
    {
        return m_pPropertyManager;
    }

    virtual ARK_SHARE_PTR<AFIRecordManager> GetRecordManager()
    {
        return m_pRecordManager;
    }

    virtual bool AddClassCallBack(const CLASS_EVENT_FUNCTOR_PTR& cb)
    {
        return mxClassEventInfo.Add(cb);
    }

    virtual bool DoEvent(const AFGUID& objectID, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& valueList)
    {
        CLASS_EVENT_FUNCTOR_PTR cb;
        bool bRet = mxClassEventInfo.First(cb);
        while(bRet)
        {
            (*cb)(objectID, mstrClassName, eClassEvent,  valueList);

            bRet = mxClassEventInfo.Next(cb);
        }

        return true;
    }

    void SetParent(ARK_SHARE_PTR<AFIClass> pClass)
    {
        m_pParentClass = pClass;
    }

    ARK_SHARE_PTR<AFIClass> GetParent()
    {
        return m_pParentClass;
    }

    void SetTypeName(const char* strType)
    {
        mstrType = strType;
    }

    const std::string& GetTypeName()
    {
        return mstrType;
    }

    const std::string& GetClassName()
    {
        return mstrClassName;
    }

    const bool AddConfigName(std::string& strConfigName)
    {
        mxConfigList.Add(strConfigName);

        return true;
    }

    NFList<std::string>& GetConfigNameList()
    {
        return mxConfigList;
    }

    void SetInstancePath(const std::string& strPath)
    {
        mstrClassInstancePath = strPath;
    }

    const std::string& GetInstancePath()
    {
        return mstrClassInstancePath;
    }

private:
    ARK_SHARE_PTR<AFIPropertyMgr> m_pPropertyManager;
    ARK_SHARE_PTR<AFIRecordManager> m_pRecordManager;

    ARK_SHARE_PTR<AFIClass> m_pParentClass;
    std::string mstrType;
    std::string mstrClassName;
    std::string mstrClassInstancePath;

    NFList<std::string> mxConfigList;

    NFList<CLASS_EVENT_FUNCTOR_PTR> mxClassEventInfo;
};

class AFCClassModule
    : public AFIClassModule
{
public:
    AFCClassModule(AFIPluginManager* p);
    virtual ~AFCClassModule();

    virtual bool Init();
    virtual bool Shut();

    virtual bool Load();
    virtual bool Save();
    virtual bool Clear();

    virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb);
    virtual bool DoEvent(const AFGUID& objectID, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& valueList);

    virtual ARK_SHARE_PTR<AFIPropertyMgr> GetClassPropertyManager(const std::string& strClassName);
    virtual ARK_SHARE_PTR<AFIRecordManager> GetClassRecordManager(const std::string& strClassName);
    //virtual ARK_SHARE_PTR<AFIComponentManager> GetClassComponentManager(const std::string& strClassName);

    virtual bool AddClass(const std::string& strClassName, const std::string& strParentName);

protected:
    virtual int ComputerType(const char* pstrTypeName, AFIData& var);
    virtual bool AddPropertys(rapidxml::xml_node<>* pPropertyRootNode, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddRecords(rapidxml::xml_node<>* pRecordRootNode, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddComponents(rapidxml::xml_node<>* pRecordRootNode, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddClassInclude(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddClass(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIClass> pClass);

    //加载此节点的类
    virtual bool Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIClass> pParentClass);

protected:
    AFIElementModule* m_pElementModule;

    std::string msConfigFileName;
};