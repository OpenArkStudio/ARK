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

#include "SDK/Core/AFPlatform.hpp"
#include "SDK/Core/AFMap.hpp"
#include "SDK/Core/AFArrayMap.hpp"
#include "SDK/Interface/AFIModule.h"
#include "SDK/Interface/AFIPluginManager.h"

#define GET_CLASS_NAME(className) (typeid(className).name());

class AFIPluginManager;

class AFIPlugin : public AFIModule
{
public:
    virtual int GetPluginVersion() = 0;
    virtual const std::string GetPluginName() = 0;

    virtual void Install() = 0;

    virtual bool Init()
    {
        for (AFIModule* pModule = mxModules.First(); pModule != nullptr; pModule = mxModules.Next())
        {
            bool bRet = pModule->Init();
            ARK_ASSERT_CONTINUE(bRet);
        }

        return true;
    }

    virtual bool PostInit()
    {
        for (AFIModule* pModule = mxModules.First(); pModule != nullptr; pModule = mxModules.Next())
        {
            bool bRet = pModule->PostInit();
            ARK_ASSERT_CONTINUE(bRet);
        }

        return true;
    }

    virtual bool CheckConfig()
    {
        for (AFIModule* pModule = mxModules.First(); pModule != nullptr; pModule = mxModules.Next())
        {
            bool bRet = pModule->CheckConfig();
            ARK_ASSERT_CONTINUE(bRet);
        }

        return true;
    }

    virtual bool Update()
    {
        for (AFIModule* pModule = mxModulesUpdate.First(); pModule != nullptr; pModule = mxModulesUpdate.Next())
        {
            pModule->Update();
        }

        return true;
    }

    virtual bool PreShut()
    {
        for (AFIModule* pModule = mxModules.First(); pModule != nullptr; pModule = mxModules.Next())
        {
            bool bRet = pModule->PreShut();
            ARK_ASSERT_CONTINUE(bRet);
        }

        return true;
    }

    virtual bool Shut()
    {
        for (AFIModule* pModule = mxModules.First(); pModule != nullptr; pModule = mxModules.Next())
        {
            bool bRet = pModule->Shut();
            ARK_ASSERT_CONTINUE(bRet);
        }

        return true;
    }

    virtual void Uninstall() = 0;

    template<typename classBaseName, typename className>
    void  RegisterModule()
    {
        assert((std::is_base_of<AFIModule, classBaseName>::value));
        assert((std::is_base_of<classBaseName, className>::value));
        AFIModule* pRegisterModuleclassName = new className(pPluginManager);
        pRegisterModuleclassName->strName = typeid(classBaseName).name();
        pPluginManager->AddModule(typeid(classBaseName).name(), pRegisterModuleclassName);
        mxModules.AddElement(typeid(classBaseName).name(), pRegisterModuleclassName);
        if ((&className::Update != &AFIModule::Update))
        {
            mxModulesUpdate.AddElement(typeid(classBaseName).name(), pRegisterModuleclassName);
        }
    }

    template<typename classBaseName, typename className>
    void  UnRegisterModule()
    {
        AFIModule* pUnRegisterModuleclassName = dynamic_cast<AFIModule*>(pPluginManager->FindModule(typeid(classBaseName).name()));
        pPluginManager->RemoveModule(typeid(classBaseName).name());
        mxModules.RemoveElement(typeid(classBaseName).name());
        mxModulesUpdate.RemoveElement(typeid(classBaseName).name());
        delete pUnRegisterModuleclassName;
        pUnRegisterModuleclassName = NULL;
    }

protected:
    //All registered modules
    AFMap<std::string, AFIModule> mxModules;
    AFMap<std::string, AFIModule> mxModulesUpdate;
};