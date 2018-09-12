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

#include "AFIModule.h"
#include "SDK/Core/AFMemAlloc.hpp"

class AFIPlugin;

#define ARK_DLL_PLUGIN_ENTRY(plugin_name)                           \
ARK_EXPORT void DllStartPlugin(AFIPluginManager* pPluginManager)    \
{                                                                   \
    AFMemAlloc::InitPool();                                         \
    AFMemAlloc::Start();                                            \
    pPluginManager->Register<plugin_name>();                        \
}

#define ARK_DLL_PLUGIN_EXIT(plugin_name)                            \
ARK_EXPORT void DllStopPlugin(AFIPluginManager* pPluginManager)     \
{                                                                   \
    pPluginManager->Deregister<plugin_name>();                      \
}

class AFIPluginManager : public AFIModule
{
public:
    AFIPluginManager() = default;
    virtual ~AFIPluginManager() = default;

    template <typename T>
    T* FindModule()
    {
        AFIModule* pLogicModule = FindModule(typeid(T).name());

        if (pLogicModule)
        {
            if (!std::is_base_of<AFIModule, T>::value)
            {
                return nullptr;
            }

            T* pT = dynamic_cast<T*>(pLogicModule);
            ARK_ASSERT_RET_VAL(pT != nullptr, nullptr);

            return pT;
        }

        ARK_ASSERT_NO_EFFECT(0);
        return nullptr;
    }

    template<typename T>
    void Register()
    {
        AFIPlugin* pCreatePluginclassName = new T(this);
        Register(pCreatePluginclassName);
    }

    template<typename T>
    void Deregister()
    {
        Deregister(FindPlugin(typeid(T).name()));
    }

    virtual void Register(AFIPlugin* plugin) = 0;
    virtual void Deregister(AFIPlugin* plugin) = 0;

    virtual AFIPlugin* FindPlugin(const std::string& strPluginName) = 0;

    virtual void AddModule(const std::string& strModuleName, AFIModule* pModule) = 0;
    virtual void RemoveModule(const std::string& strModuleName) = 0;
    virtual AFIModule* FindModule(const std::string& strModuleName) = 0;

    virtual int BusID() const = 0;
    virtual void SetBusID(const int app_id) = 0;

    virtual const std::string& AppName() const = 0;
    virtual void SetAppName(const std::string& app_name) = 0;

    virtual int64_t GetNowTime() const = 0;
    virtual const std::string& GetResPath() const = 0;

    virtual void SetPluginConf(const std::string& strFileName) = 0;

    virtual void SetLogPath(const std::string& log_path) = 0;
    virtual const std::string& GetLogPath() const = 0;
};