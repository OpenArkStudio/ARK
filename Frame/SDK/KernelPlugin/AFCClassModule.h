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

#pragma once

#include "AFCElementModule.h"
#include "RapidXML/rapidxml.hpp"
#include "SDK/Core/Base/AFList.h"
#include "SDK/Core/Base/AFCDataList.h"
#include "SDK/Core/AFCDataNodeManager.h"
#include "SDK/Core/AFCDataTableManager.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFILogModule.h"

class AFCClass : public AFIClass
{
public:
    explicit AFCClass(const std::string& strClassName) :
        m_pParentClass(nullptr),
        mstrType(NULL_STR),
        mstrClassName(strClassName),
        mstrClassInstancePath(NULL_STR)

    {
        m_pNodeManager = std::make_shared<AFCDataNodeManager>(NULL_GUID);
        m_pTableManager = std::make_shared<AFCDataTableManager>(NULL_GUID);
    }

    virtual ~AFCClass()
    {
        ClearAll();
    }

    virtual ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager()
    {
        return m_pNodeManager;
    }

    virtual ARK_SHARE_PTR<AFIDataTableManager> GetTableManager()
    {
        return m_pTableManager;
    }

    virtual bool AddClassCallBack(const CLASS_EVENT_FUNCTOR_PTR& cb)
    {
        return mxClassEventInfo.Add(cb);
    }

    virtual bool DoEvent(const AFGUID& objectID, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& valueList)
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

    bool AddConfigName(std::string& strConfigName)
    {
        return mxConfigList.Add(strConfigName);
    }

    AFList<std::string>& GetConfigNameList()
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
    ARK_SHARE_PTR<AFIDataNodeManager> m_pNodeManager;
    ARK_SHARE_PTR<AFIDataTableManager> m_pTableManager;

    ARK_SHARE_PTR<AFIClass> m_pParentClass;
    std::string mstrType;
    std::string mstrClassName;
    std::string mstrClassInstancePath;

    AFList<std::string> mxConfigList;

    AFList<CLASS_EVENT_FUNCTOR_PTR> mxClassEventInfo;
};

class AFCClassModule : public AFIClassModule
{
public:
    explicit AFCClassModule(AFIPluginManager* p);
    virtual ~AFCClassModule();

    virtual bool Init();
    virtual bool Shut();

    virtual bool Load();
    virtual bool Clear();

    virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb);
    virtual bool DoEvent(const AFGUID& objectID, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& valueList);

    virtual ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager(const std::string& strClassName);
    virtual ARK_SHARE_PTR<AFIDataTableManager> GetTableManager(const std::string& strClassName);
    virtual bool InitDataNodeManager(const std::string& strClassName, ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager);
    virtual bool InitDataTableManager(const std::string& strClassName, ARK_SHARE_PTR<AFIDataTableManager> pTableManager);
    virtual bool AddClass(const std::string& strClassName, const std::string& strParentName);

protected:
    virtual int ComputerType(const char* pstrTypeName, AFIData& var);
    virtual bool AddNodes(rapidxml::xml_node<>* pNodeRootNode, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddTables(rapidxml::xml_node<>* pTableRootNode, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddComponents(rapidxml::xml_node<>* pComponentRootNode, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddClassInclude(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIClass> pClass);
    virtual bool AddClass(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIClass> pClass);

    virtual bool Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIClass> pParentClass);

private:
    AFIElementModule* m_pElementModule;
    AFILogModule* m_pLogModule;
    std::string msConfigFileName;
};