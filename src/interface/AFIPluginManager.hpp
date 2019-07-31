/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "base/AFMacros.hpp"

namespace ark {

#define ARK_PLUGIN_DECLARE(plugin_name)                                                                                \
    ARK_EXPORT_FUNC void DllEntryPlugin(AFIPluginManager* pPluginManager)                                              \
    {                                                                                                                  \
        pPluginManager->Register<plugin_name>();                                                                       \
    }                                                                                                                  \
    ARK_EXPORT_FUNC void DllExitPlugin(AFIPluginManager* pPluginManager)                                               \
    {                                                                                                                  \
        pPluginManager->Deregister<plugin_name>();                                                                     \
    }

class AFIModule;
class AFIPlugin;

class ARK_EXPORT AFIPluginManager // : public AFIModule
{
public:
    AFIPluginManager() = default;
    virtual ~AFIPluginManager() = default;

    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual bool Update() = 0;

    template<typename DerivedModule>
    DerivedModule* FindModule()
    {
        AFIModule* pLogicModule = FindModule(GET_CLASS_NAME(DerivedModule));
        ARK_ASSERT_RET_VAL(pLogicModule != nullptr, nullptr);

        if (!std::is_base_of<AFIModule, DerivedModule>::value)
        {
            return nullptr;
        }

        DerivedModule* derived_module = dynamic_cast<DerivedModule*>(pLogicModule);
        ARK_ASSERT_RET_VAL(derived_module != nullptr, nullptr);

        return derived_module;
    }

    template<typename PLUGIN_TYPE>
    void Register()
    {
        AFIPlugin* pNewPlugin = ARK_NEW PLUGIN_TYPE();
        Register(pNewPlugin);
    }

    template<typename PLUGIN_TYPE>
    void Deregister()
    {
        //! Delete plugin memory in Deregister
        Deregister(FindPlugin(GET_CLASS_NAME(PLUGIN_TYPE)));
    }

    virtual void Register(AFIPlugin* plugin) = 0;
    virtual void Deregister(AFIPlugin* plugin) = 0;

    virtual AFIPlugin* FindPlugin(const std::string& strPluginName) = 0;

    virtual void AddModule(const std::string& module_name, AFIModule* pModule) = 0;
    virtual void RemoveModule(const std::string& module_name) = 0;

    virtual AFIModule* FindModule(const std::string& module_name) = 0;
    virtual void RemoveUpdateModule(const std::string& module_name) = 0;

    // add the module with update functions.
    virtual bool AddUpdateModule(AFIModule* pModule) = 0;

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

} // namespace ark