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

#define REGISTER_MODULE(pManager, classBaseName, className)             \
    assert((std::is_base_of<AFIModule, classBaseName>::value));         \
    assert((std::is_base_of<classBaseName, className>::value));         \
    AFIModule* pRegisterModule##className = new className(pManager);    \
    pRegisterModule##className->strName = typeid(classBaseName).name(); \
    pManager->AddModule(typeid(classBaseName).name(), pRegisterModule##className);    \
    mxModules.AddElement(typeid(classBaseName).name(), pRegisterModule##className);

#define UNREGISTER_MODULE(pManager, classBaseName, className)           \
    AFIModule* pUnRegisterModule##className = dynamic_cast<AFIModule*>(pManager->FindModule(typeid(classBaseName).name()));     \
    pManager->RemoveModule(typeid(classBaseName).name());               \
    mxModules.RemoveElement(typeid(classBaseName).name());              \
    delete pUnRegisterModule##className;                                \
    pUnRegisterModule##className = NULL;                                \


#define CREATE_PLUGIN(pManager, className)                              \
    AFIPlugin* pCreatePlugin##className = new className(pManager);      \
    pManager->Registered(pCreatePlugin##className);

#define DESTROY_PLUGIN(pManager, className) pManager->UnRegistered(pManager->FindPlugin((#className)));

#define GET_CLASS_NAME(className) (#className);

/*
#define REGISTER_COMPONENT(pManager, className)  AFIComponent* pRegisterComponent##className= new className(pManager); \
    pRegisterComponent##className->strName = (#className); \
    pManager->AddComponent( (#className), pRegisterComponent##className );

#define UNREGISTER_COMPONENT(pManager, className) AFIComponent* pRegisterComponent##className =  \
        dynamic_cast<AFIComponent*>( pManager->FindComponent( (#className) ) ); pManager->RemoveComponent( (#className) ); delete pRegisterComponent##className;
*/

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
        for (AFIModule* pModule = mxModules.First(); pModule != nullptr; pModule = mxModules.Next())
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

protected:
    //All registered modules
    AFMap<std::string, AFIModule> mxModules;
};