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
    virtual void Uninstall() = 0;

    template<typename classBaseName, typename className>
    void  RegisterModule()
    {
        assert((std::is_base_of<AFIModule, classBaseName>::value));
        assert((std::is_base_of<classBaseName, className>::value));
        AFIModule* pRegModuleName = ARK_NEW className(pPluginManager);
        pRegModuleName->strName = typeid(classBaseName).name();
        pPluginManager->AddModule(typeid(classBaseName).name(), pRegModuleName);
        mxModules.AddElement(typeid(classBaseName).name(), pRegModuleName);
#if ARK_PLATFORM == PLATFORM_WIN
        if ((&className::Update != &AFIModule::Update))
#else
        AFIModule base;
        bool (AFIModule::*mfp)() = &AFIModule::Update;
        bool (className::*child_mfp)() = &className::Update;
        void* base_update_mfp = (void*)(base.*mfp);
        void* derived_update_mfp = (void*)(static_cast<className*>(pRegModuleName)->*child_mfp);
        if (base_update_mfp == derived_update_mfp)
#endif
        {
            mxModulesUpdates.AddElement(typeid(classBaseName).name(), pRegModuleName);
        }
    }

    template<typename classBaseName, typename className>
    void  DeregisterModule()
    {
        AFIModule* pDeregModuleName = dynamic_cast<AFIModule*>(pPluginManager->FindModule(typeid(classBaseName).name()));
        pPluginManager->RemoveModule(typeid(classBaseName).name());
        mxModules.RemoveElement(typeid(classBaseName).name());
        mxModulesUpdates.RemoveElement(typeid(classBaseName).name());
        ARK_DELETE(pDeregModuleName);
    }

protected:
    //All registered modules
    AFMap<std::string, AFIModule> mxModules;
    AFMap<std::string, AFIModule> mxModulesUpdates;
};