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

#include "base/AFSingleton.hpp"
#include "base/AFMap.hpp"
#include "base/AFArrayMap.hpp"
#include "base/AFDateTime.hpp"
#include "base/AFXml.hpp"
#include "base/AFDynLib.hpp"
#include "interface/AFIPlugin.hpp"
#include "interface/AFIModule.hpp"

namespace ark {

class ARK_EXPORT AFPluginManager final : public AFSingleton<AFPluginManager>
{
public:
    AFPluginManager()
        : timestamp_(AFDateTime::GetNowTime())
    {
    }

    bool Start()
    {
        ARK_ASSERT_RET_VAL(Init(), false);
        ARK_ASSERT_RET_VAL(PostInit(), false);
        ARK_ASSERT_RET_VAL(CheckConfig(), false);
        ARK_ASSERT_RET_VAL(PreUpdate(), false);

        return true;
    }

    bool Stop()
    {
        PreShut();
        Shut();

        return true;
    }

    bool Update()
    {
        timestamp_ = AFDateTime::GetNowTime();

        // Just loop the modules which have update function.
        for (const auto& iter : module_updates_)
        {
            AFIModule* pModule = iter.second;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->Update();
        }

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
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

    void AddModule(const std::string& module_name, AFIModule* module_ptr)
    {
        ARK_ASSERT_RET_NONE(FindModule(module_name) == nullptr);

        if (module_instances_.insert(module_name, module_ptr).second)
        {
            ordered_module_instances_.push_back(module_ptr);
        }
    }

    void RemoveModule(const std::string& module_name)
    {
        auto iter = module_instances_.find(module_name);
        if (iter == module_instances_.end())
        {
            return;
        }

        auto module_ptr = iter->second;
        module_instances_.erase(module_name);

        auto it = std::find(ordered_module_instances_.begin(), ordered_module_instances_.end(), module_ptr);
        if (it != ordered_module_instances_.end())
        {
            ordered_module_instances_.erase(it);
        }
    }

    bool AddUpdateModule(AFIModule* pModule)
    {
        ARK_ASSERT_RET_VAL(pModule != nullptr, false);
        return module_updates_.insert(pModule->Name(), pModule).second;
    }

    void RemoveUpdateModule(const std::string& module_name)
    {
        module_updates_.erase(module_name);
    }

    int BusID() const
    {
        return bus_id_;
    }

    void SetBusID(const int value)
    {
        bus_id_ = value;
    }

    const std::string& AppName() const
    {
        return app_name_;
    }

    void SetAppName(const std::string& value)
    {
        app_name_ = value;
    }

    int64_t GetNowTime() const
    {
        return timestamp_;
    }

    const std::string& GetResPath() const
    {
        return res_path_;
    }

    void SetPluginConf(const std::string& value)
    {
        ARK_ASSERT_RET_NONE(!value.empty());
        ARK_ASSERT_RET_NONE(value.find(".plugin") != string::npos);

        plugin_conf_path_ = value;
    }

    void SetLogPath(const std::string& value)
    {
        log_path_ = value;
    }

    const std::string& GetLogPath() const
    {
        return log_path_;
    }

protected:
    void Register(AFIPlugin* plugin)
    {
        std::string plugin_name = plugin->GetPluginName();

        if (!FindPlugin(plugin_name))
        {
            plugin->SetPluginManager(this);
            plugin_instances_.insert(plugin_name, plugin);
            plugin->Install();
        }
        else
        {
            ARK_ASSERT_NO_EFFECT(0);
        }
    }

    void Deregister(AFIPlugin* plugin)
    {
        ARK_ASSERT_RET_NONE(plugin != nullptr);

        auto find_plugin = FindPlugin(plugin->GetPluginName());
        ARK_ASSERT_RET_NONE(find_plugin != nullptr);

        plugin->Uninstall();
        plugin_instances_.erase(plugin->GetPluginName());
        ARK_DELETE(plugin);
    }

    AFIPlugin* FindPlugin(const std::string& plugin_name)
    {
        return plugin_instances_.find_value(plugin_name);
    }

    AFIModule* FindModule(const std::string& module_name)
    {
        auto iter = module_instances_.find(module_name);
        if (iter != module_instances_.end())
        {
            return iter->second;
        }
        else
        {
            return nullptr;
        }
    }

    bool Init()
    {
        // load plugin configuration
        ARK_ASSERT_RET_VAL(LoadPluginConf(), false);

        // load plugin dynamic libraries
        for (const auto& iter : ordered_plugin_names_)
        {
            bool ret = LoadPluginLibrary(iter);
            ARK_ASSERT_RET_VAL(ret, false);
        }

        // initialize all modules
        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->Init();
        }

        return true;
    }

    bool PostInit()
    {
        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->PostInit();
        }

        return true;
    }

    bool CheckConfig()
    {
        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->CheckConfig();
        }

        return true;
    }

    bool PreUpdate()
    {
        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->PreUpdate();
        }

        return true;
    }

    bool PreShut()
    {
        timestamp_ = AFDateTime::GetNowTime();

        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->PreShut();
        }

        return true;
    }

    bool Shut()
    {
        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            ARK_ASSERT_CONTINUE(pModule != nullptr);

            pModule->Shut();
        }

        for (auto it : plugin_names_)
        {
            UnloadPluginLibrary(it.first);
        }

        plugin_instances_.clear();
        plugin_names_.clear();
        return true;
    }

    bool LoadPluginConf()
    {
        AFXml xml_doc(plugin_conf_path_);

        auto root_node = xml_doc.GetRootNode();
        ARK_ASSERT_RET_VAL(root_node.IsValid(), false);

        auto plugins_node = root_node.FindNode("plugins");
        ARK_ASSERT_RET_VAL(plugins_node.IsValid(), false);
        plugin_path_ = plugins_node.GetString("path");
        ARK_ASSERT_RET_VAL(!plugin_path_.empty(), false);

        for (auto plugin_node = plugins_node.FindNode("plugin"); plugin_node.IsValid(); plugin_node.NextNode())
        {
            std::string plugin_name = plugin_node.GetString("name");
            if (plugin_names_.insert(std::make_pair(plugin_name, true)).second)
            {
                ordered_plugin_names_.emplace_back(plugin_name);
            }
        }

        auto res_node = root_node.FindNode("res");
        ARK_ASSERT_RET_VAL(res_node.IsValid(), false);
        res_path_ = res_node.GetString("path");
        ARK_ASSERT_RET_VAL(!res_path_.empty(), false);

        return true;
    }

    bool LoadPluginLibrary(const std::string& plugin_name)
    {
        auto iter = plugin_libs_.find(plugin_name);
        ARK_ASSERT_RET_VAL(iter == plugin_libs_.end(), false);

        AFDynLib* pLib = ARK_NEW AFDynLib(plugin_name);
        ARK_ASSERT_RET_VAL(pLib != nullptr, false);

        bool load_ret = pLib->Load(plugin_path_);
        if (load_ret)
        {
            plugin_libs_.insert(plugin_name, pLib);
            auto func = (DLL_ENTRY_PLUGIN_FUNC)pLib->GetSymbol("DllEntryPlugin");
            ARK_ASSERT_RET_VAL(func != nullptr, false);
            func(this);

            return true;
        }
        else
        {
#ifdef ARK_PLATFORM_WIN
            CONSOLE_LOG << "Load dynamic library[" << pLib->GetName() << "] failed, ErrorNo=[" << GetLastError() << "]"
                        << std::endl;
            CONSOLE_LOG << "Load [" << pLib->GetName() << "] failed" << std::endl;
            assert(0);
            return false;
#else
            char* error = dlerror();
            if (error)
            {
                CONSOLE_LOG << stderr << " Load shared library[" << pLib->GetName() << "] failed, ErrorNo=[" << error
                            << "]" << std::endl;
                CONSOLE_LOG << "Load [" << pLib->GetName() << "] failed" << std::endl;
                assert(0);
                return false;
            }
#endif
        }

        return true;
    }

    bool UnloadPluginLibrary(const std::string& plugin_name)
    {
        auto iter = plugin_libs_.find(plugin_name);
        ARK_ASSERT_RET_VAL(iter != plugin_libs_.end(), false);

        AFDynLib* pDynLib = iter->second;
        ARK_ASSERT_RET_VAL(pDynLib == nullptr, false);

        auto func = (DLL_EXIT_PLUGIN_FUNC)pDynLib->GetSymbol("DllExitPlugin");
        ARK_ASSERT_RET_VAL(func != nullptr, false);
        func(this);

        pDynLib->UnLoad();
        ARK_DELETE(pDynLib);

        plugin_libs_.erase(plugin_name);
        return true;
    }

private:
    // Bus id
    int bus_id_{0};
    // Current time(ms)
    int64_t timestamp_{0};
    // plugin so/dll file path
    std::string plugin_path_{};
    // Resource path
    std::string res_path_{};
    // app.plugin file path
    std::string plugin_conf_path_{};
    // app name
    std::string app_name_{};
    // log output path
    std::string log_path_{};

    using DLL_ENTRY_PLUGIN_FUNC = void (*)(AFPluginManager* p);
    using DLL_EXIT_PLUGIN_FUNC = void (*)(AFPluginManager* p);

    std::map<std::string, bool> plugin_names_;
    std::vector<std::string> ordered_plugin_names_; // order
    AFMap<std::string, AFDynLib> plugin_libs_;
    AFMap<std::string, AFIPlugin> plugin_instances_;
    AFMap<std::string, AFIModule> module_instances_;
    std::vector<AFIModule*> ordered_module_instances_; // order

    AFMap<std::string, AFIModule> module_updates_;
};

} // namespace ark
