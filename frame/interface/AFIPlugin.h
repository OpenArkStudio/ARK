/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
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

#include "base/AFPlatform.hpp"
#include "base/AFMap.hpp"
#include "base/AFArrayMap.hpp"
#include "AFIModule.h"
#include "AFIPluginManager.h"

namespace ark
{

#define GET_CLASS_NAME(className) (typeid(className).name());

    class AFIPlugin : public AFIModule
    {
    public:
        virtual int GetPluginVersion() = 0;
        virtual const std::string GetPluginName() = 0;

        virtual void Install() = 0;
        virtual void Uninstall() = 0;

        template<typename classBaseName, typename className>
        void RegisterModule()
        {
            assert((std::is_base_of<AFIModule, classBaseName>::value));
            assert((std::is_base_of<classBaseName, className>::value));
            AFIModule* pRegModuleName = ARK_NEW className();
            pRegModuleName->SetPluginManager(pPluginManager);
            pRegModuleName->name_ = typeid(classBaseName).name();
            pPluginManager->AddModule(pRegModuleName->name_, pRegModuleName);
            modules_.insert(pRegModuleName->name_, pRegModuleName);
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
                module_updates_.insert(pRegModuleName->name_, pRegModuleName);
            }
        }

        template<typename classBaseName, typename className>
        void DeregisterModule()
        {
            std::string name = typeid(classBaseName).name();
            AFIModule* pDeregModuleName = dynamic_cast<AFIModule*>(pPluginManager->FindModule(name));
            pPluginManager->RemoveModule(name);
            modules_.erase(typeid(classBaseName).name());
            module_updates_.erase(name);
            ARK_DELETE(pDeregModuleName);
        }

    protected:
        //All registered modules
        AFMap<std::string, AFIModule> modules_;
        AFMap<std::string, AFIModule> module_updates_;
    };

}