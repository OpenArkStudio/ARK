/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "base/AFNoncopyable.hpp"
#include "base/AFLogger.hpp"
#include "base/AFDynLib.hpp"
#include "interface/AFIPlugin.hpp"
#include "interface/AFIModule.hpp"

namespace ark {

class AFPluginManager final : private AFNoncopyable
{
public:
    static auto instance() -> AFPluginManager*
    {
        static AFPluginManager* instance_ = nullptr;
        if (instance_ == nullptr)
        {
            instance_ = ARK_NEW AFPluginManager();
        }

        return instance_;
    }

    auto Start(int64_t const timestamp, std::string const& lib_dir, std::string const& conf_dir,
        std::unordered_map<std::string, std::string> const& plugins) -> bool
    {
        timestamp_ = timestamp;

        plugin_library_dir_ = lib_dir;
        plugin_conf_dir_ = conf_dir;
        plugins_ = plugins;

        ARK_ASSERT_RET_VAL(Init(), false);
        ARK_ASSERT_RET_VAL(PostInit(), false);
        ARK_ASSERT_RET_VAL(CheckConfig(), false);
        ARK_ASSERT_RET_VAL(PreUpdate(), false);

        return true;
    }

    auto Stop(const int64_t timestamp) -> bool
    {
        timestamp_ = timestamp;

        PreShut();
        Shut();

        return true;
    }

    auto Update(const int64_t timestamp) -> bool
    {
        timestamp_ = timestamp;

        // Just loop the modules which have update function.
        for (const auto& iter : module_updates_)
        {
            AFIModule* pModule = iter.second;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->Update();
        }

        return true;
    }

    template<typename DerivedModule>
    auto FindModule() -> DerivedModule*
    {
        AFIModule* pLogicModule = FindModule(GET_CLASS_NAME(DerivedModule));
        ARK_ASSERT_RET_VAL(pLogicModule != nullptr, nullptr);

        using derived_type =
            typename std::enable_if<std::is_base_of<AFIModule, DerivedModule>::value, DerivedModule>::type;

        auto derived_module = dynamic_cast<derived_type*>(pLogicModule);
        ARK_ASSERT_RET_VAL(derived_module != nullptr, nullptr);

        return derived_module;
    }

    template<typename PLUGIN_TYPE>
    void Register(std::string const& plugin_name)
    {
        AFIPlugin* pNewPlugin = ARK_NEW PLUGIN_TYPE();
        std::string runtime_plugin_name = GET_CLASS_NAME(PLUGIN_TYPE);
        Register(pNewPlugin, plugin_name, runtime_plugin_name);
    }

    template<typename PLUGIN_TYPE>
    void Unregister()
    {
        //! Delete plugin memory in Unregister
        Unregister(GET_CLASS_NAME(PLUGIN_TYPE));
    }

    auto AddModule(const std::string& module_name, AFIModule* module_ptr) -> bool
    {
        ARK_ASSERT_RET_VAL(!module_name.empty(), false);
        ARK_ASSERT_RET_VAL(FindModule(module_name) == nullptr, false);
        return module_instances_.insert(std::make_pair(module_name, module_ptr)).second;
    }

    void RemoveModule(const std::string& module_name)
    {
        auto iter = module_instances_.find(module_name);
        if (iter == module_instances_.end())
        {
            return;
        }

        module_instances_.erase(module_name);
    }

    auto AddUpdateModule(AFIModule* pModule) -> bool
    {
        ARK_ASSERT_RET_VAL(pModule != nullptr, false);
        return module_updates_.insert(std::make_pair(pModule->GetName(), pModule)).second;
    }

    void RemoveUpdateModule(const std::string& module_name)
    {
        module_updates_.erase(module_name);
    }

    auto GetNowTime() const -> int64_t
    {
        return timestamp_;
    }

    auto GetBusID() const -> bus_id_t
    {
        return bus_id_;
    }

    void SetBusID(const bus_id_t value)
    {
        bus_id_ = value;
    }

    auto GetAppName() const -> const std::string&
    {
        return app_name_;
    }

    void SetAppName(const std::string& value)
    {
        app_name_ = value;
    }

    void SetAppConf(const std::string& value)
    {
        ARK_ASSERT_RET_NONE(!value.empty());
        ARK_ASSERT_RET_NONE(value.find(".app.conf") != std::string::npos);

        app_conf_path_ = value;
    }

    auto GetAppConf() const -> std::string const&
    {
        return app_conf_path_;
    }

    auto GetPluginConf(std::string const& plugin_name) -> std::string const&
    {
        const static std::string null_str;
        ARK_ASSERT_RET_VAL(!plugin_name.empty(), null_str);
        auto iter = plugins_.find(plugin_name);
        return iter != plugins_.end() ? iter->second : null_str;
    }

    template<typename PLUGIN_TYPE>
    auto GetPluginConf() -> const std::string&
    {
        auto plugin = FindPlugin<PLUGIN_TYPE>();
        ARK_ASSERT_RET_VAL(plugin != nullptr, NULL_STR);

        return plugin->GetPluginConf();
    }

protected:
    void Register(AFIPlugin* plugin, std::string const& plugin_name, std::string const& runtime_plugin_name)
    {
        // e.g. plugin_name = class ark::AFKernelPlugin
        // It's a runtime name, so we cannot use it to find the plugin.conf file.
        //std::string plugin_name = plugin->GetPluginName();

        ARK_ASSERT_RET_NONE(FindPlugin(plugin_name) == nullptr);

        auto iter = plugins_.find(plugin_name);
        ARK_ASSERT_NO_EFFECT(iter != plugins_.end());

        plugin->SetPluginManager(this);
        // set plugin self configuration
        plugin->SetPluginConf(plugin_conf_dir_ + iter->second);
        // Install modules
        plugin->Install();
        // manage this plugin
        plugin_instances_.insert(std::make_pair(runtime_plugin_name, plugin));
    }

    void Unregister(const std::string& plugin_name)
    {
        ARK_ASSERT_RET_NONE(!plugin_name.empty());

        auto plugin = FindPlugin(plugin_name);
        ARK_ASSERT_RET_NONE(plugin != nullptr);

        plugin->Uninstall();
        plugin_instances_.erase(plugin_name);
        ARK_DELETE(plugin);
    }

    auto FindPlugin(const std::string& plugin_name) -> AFIPlugin*
    {
        auto iter = plugin_instances_.find(plugin_name);
        if (iter != plugin_instances_.end())
        {
            return iter->second;
        }
        else
        {
            return nullptr;
        }
    }

    template<typename PLUGIN_TYPE>
    auto FindPlugin() -> AFIPlugin*
    {
        using derived_type = typename std::enable_if<std::is_base_of<AFIPlugin, PLUGIN_TYPE>::value, PLUGIN_TYPE>::type;
        auto plugin_name = GET_CLASS_NAME(derived_type);
        auto iter = plugin_instances_.find(plugin_name);
        if (iter != plugin_instances_.end())
        {
            return iter->second;
        }
        else
        {
            return nullptr;
        }
    }

    auto FindModule(const std::string& module_name) -> AFIModule*
    {
        auto iter = module_instances_.find(module_name);
        return ((iter != module_instances_.end()) ? iter->second : nullptr);
    }

    auto Init() -> bool
    {
        // load plugin dynamic libraries
        for (auto const& iter : plugins_)
        {
            bool ret = LoadPluginLibrary(iter.first);
            ARK_ASSERT_RET_VAL(ret, false);
        }

        // initialize all modules
        for (auto const& iter : module_instances_)
        {
            AFIModule* pModule = iter.second;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->Init();
        }

        return true;
    }

    auto PostInit() -> bool
    {
        for (auto const& iter : module_instances_)
        {
            AFIModule* pModule = iter.second;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->PostInit();
        }

        return true;
    }

    auto CheckConfig() -> bool
    {
        for (auto const& iter : module_instances_)
        {
            AFIModule* pModule = iter.second;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->CheckConfig();
        }

        return true;
    }

    auto PreUpdate() -> bool
    {
        for (auto const& iter : module_instances_)
        {
            AFIModule* pModule = iter.second;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->PreUpdate();
        }

        return true;
    }

    auto PreShut() -> bool
    {
        for (auto const& iter : module_instances_)
        {
            AFIModule* pModule = iter.second;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->PreShut();
        }

        return true;
    }

    auto Shut() -> bool
    {
        for (auto const& iter : module_instances_)
        {
            AFIModule* pModule = iter.second;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->Shut();
        }

        for (auto& it : plugins_)
        {
            UnloadPluginLibrary(it.first);
        }

        for (auto& iter : plugin_libs_)
        {
            iter.second->UnLoad();
            ARK_DELETE(iter.second);
        }

        return true;
    }

    auto LoadPluginLibrary(const std::string& plugin_name) -> bool
    {
        auto iter = plugin_libs_.find(plugin_name);
        ARK_ASSERT_RET_VAL(iter == plugin_libs_.end(), false);

        auto* pLib = ARK_NEW AFDynLib(plugin_name);
        ARK_ASSERT_RET_VAL(pLib != nullptr, false);

        bool load_ret = pLib->Load(plugin_library_dir_);
        if (load_ret)
        {
            plugin_libs_.insert(std::make_pair(plugin_name, pLib));
            auto func = reinterpret_cast<DLL_ENTRY_PLUGIN_FUNC>(pLib->GetSymbol("DllEntryPlugin"));
            ARK_ASSERT_RET_VAL(func != nullptr, false);
            func(this, plugin_name, AFLogger::instance());

            return true;
        }
        else
        {
#ifdef ARK_PLATFORM_WIN
            auto error = GetLastError();
#else
            auto error = dlerror();
#endif
            ARK_LOG_ERROR("Load shared library[{}] failed, ErrorNo[{}]", pLib->GetName(), error);
            ARK_ASSERT_RET_VAL(0, false);
        }

        return true;
    }

    auto UnloadPluginLibrary(const std::string& plugin_name) -> bool
    {
        auto iter = plugin_libs_.find(plugin_name);
        ARK_ASSERT_RET_VAL(iter != plugin_libs_.end(), false);

        AFDynLib* pDynLib = iter->second;
        ARK_ASSERT_RET_VAL(pDynLib != nullptr, false);

        auto func = reinterpret_cast<DLL_EXIT_PLUGIN_FUNC>(pDynLib->GetSymbol("DllExitPlugin"));
        ARK_ASSERT_RET_VAL(func != nullptr, false);
        func(this);

        return true;
    }

private:
    // Bus id
    int bus_id_{0};
    // Current time(ms)
    int64_t timestamp_{0};
    // plugin so/dll file dir
    std::string plugin_library_dir_{};
    // xx.plugin.conf file path
    std::string plugin_conf_dir_{};
    // xx.app.conf file path
    std::string app_conf_path_{};
    // app name
    std::string app_name_{"ark"};

    using DLL_ENTRY_PLUGIN_FUNC = void (*)(AFPluginManager*, std::string const&, AFLogger*);
    using DLL_EXIT_PLUGIN_FUNC = void (*)(AFPluginManager*);

    // plugin_code_name -> plugin_config
    std::unordered_map<std::string, std::string> plugins_;
    // plugin_code_name -> AFDynLib*
    std::unordered_map<std::string, AFDynLib*> plugin_libs_;
    // plugin_code_name -> AFIPlugin*
    std::unordered_map<std::string, AFIPlugin*> plugin_instances_;
    // module_runtime_name -> AFIModule*
    std::unordered_map<std::string, AFIModule*> module_instances_;
    // Only include the module with self Update function. module_runtime_name -> AFIModule*
    std::unordered_map<std::string, AFIModule*> module_updates_;
};

} // namespace ark
