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

#include "SDK/Core/AFCDataList.hpp"
#include "SDK/Core/AFDataTable.h"
#include "SDK/Core/AFArrayMap.hpp"
#include "SDK/Core/AFCDataNodeManager.h"
#include "SDK/Core/AFCDataTableManager.h"
#include "SDK/Interface/AFIConfigModule.h"
#include "SDK/Interface/AFIClassModule.h"

class AFCClass;

class ElementConfigInfo
{
public:
    ElementConfigInfo()
    {
        m_pNodeManager = std::make_shared<AFCDataNodeManager>(NULL_GUID);
        m_pTableManager = std::make_shared<AFCDataTableManager>(NULL_GUID);
    }

    virtual ~ElementConfigInfo()
    {
    }

    ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager()
    {
        return m_pNodeManager;
    }

    ARK_SHARE_PTR<AFIDataTableManager> GetTableManager()
    {
        return m_pTableManager;
    }

private:
    ARK_SHARE_PTR<AFIDataNodeManager> m_pNodeManager;
    ARK_SHARE_PTR<AFIDataTableManager> m_pTableManager;
};

class AFCConfigModule : public AFIConfigModule
{
public:
    explicit AFCConfigModule(AFIPluginManager* p);
    virtual ~AFCConfigModule() = default;

    virtual bool Init();
    virtual bool Shut();

    virtual bool Load();
    virtual bool Save();
    virtual bool Clear();

    virtual bool ExistConfig(const std::string& strConfigName);
    virtual bool ExistConfig(const std::string& strClassName, const std::string& strConfigName);

    virtual ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager(const std::string& strConfigName);
    virtual ARK_SHARE_PTR<AFIDataTableManager> GetTableManager(const std::string& strConfigName);

    virtual bool GetNodeBool(const std::string& strConfigName, const std::string& strDataNodeName);
    virtual int32_t GetNodeInt(const std::string& strConfigName, const std::string& strDataNodeName);
    virtual int64_t GetNodeInt64(const std::string& strConfigName, const std::string& strDataNodeName);
    virtual float GetNodeFloat(const std::string& strConfigName, const std::string& strDataNodeName);
    virtual double GetNodeDouble(const std::string& strConfigName, const std::string& strDataNodeName);
    virtual const char* GetNodeString(const std::string& strConfigName, const std::string& strDataNodeName);

protected:
    AFDataNode* GetNode(const std::string& strConfigName, const std::string& strDataNodeName);
    bool Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIClass> pLogicClass);

private:
    AFIClassModule* m_pClassModule{nullptr};
    bool mbLoaded{false};
    AFArrayMap<std::string, ElementConfigInfo> mxElementConfigMap;
};