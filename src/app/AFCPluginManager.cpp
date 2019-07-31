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

#include "base/AFPlatform.hpp"
#include "base/AFDateTime.hpp"
#include "base/AFXml.hpp"
#include "interface/AFIPlugin.hpp"
#include "AFCPluginManager.hpp"

namespace ark {

AFCPluginManager::AFCPluginManager()
    : AFIPluginManager()
{
    timestamp_ = AFDateTime::GetNowTime();
}

bool AFCPluginManager::Start()
{
    ARK_ASSERT_RET_VAL(Init(), false);
    ARK_ASSERT_RET_VAL(PostInit(), false);
    ARK_ASSERT_RET_VAL(CheckConfig(), false);
    ARK_ASSERT_RET_VAL(PreUpdate(), false);

    return true;
}

bool AFCPluginManager::Stop()
{
    AFCPluginManager::get()->PreShut();
    AFCPluginManager::get()->Shut();

    return true;
}

inline bool AFCPluginManager::Init()
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

bool AFCPluginManager::LoadPluginConf()
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

void AFCPluginManager::Register(AFIPlugin* plugin)
{
    std::string strPluginName = plugin->GetPluginName();

    if (!FindPlugin(strPluginName))
    {
        plugin->SetPluginManager(this);
        plugin_instances_.insert(strPluginName, plugin);
        plugin->Install();
    }
    else
    {
        ARK_ASSERT_NO_EFFECT(0);
    }
}

void AFCPluginManager::Deregister(AFIPlugin* plugin)
{
    ARK_ASSERT_RET_NONE(plugin != nullptr);

    auto find_plugin = FindPlugin(plugin->GetPluginName());
    ARK_ASSERT_RET_NONE(find_plugin != nullptr);

    plugin->Uninstall();
    plugin_instances_.erase(plugin->GetPluginName());
    ARK_DELETE(plugin);
}

AFIPlugin* AFCPluginManager::FindPlugin(const std::string& plugin_name)
{
    return plugin_instances_.find_value(plugin_name);
}

inline int AFCPluginManager::BusID() const
{
    return bus_id_;
}

const std::string& AFCPluginManager::AppName() const
{
    return app_name_;
}

inline int64_t AFCPluginManager::GetNowTime() const
{
    return timestamp_;
}

inline const std::string& AFCPluginManager::GetResPath() const
{
    return res_path_;
}

void AFCPluginManager::SetPluginConf(const std::string& file_path)
{
    ARK_ASSERT_RET_NONE(!file_path.empty());
    ARK_ASSERT_RET_NONE(file_path.find(".plugin") != string::npos);

    plugin_conf_path_ = file_path;
}

void AFCPluginManager::SetLogPath(const std::string& log_path)
{
    log_path_ = log_path;
}

const std::string& AFCPluginManager::GetLogPath() const
{
    return log_path_;
}

void AFCPluginManager::SetBusID(const int app_id)
{
    bus_id_ = app_id;
}

void AFCPluginManager::SetAppName(const std::string& app_name)
{
    app_name_ = app_name;
}

void AFCPluginManager::AddModule(const std::string& module_name, AFIModule* module_ptr)
{
    ARK_ASSERT_RET_NONE(FindModule(module_name) == nullptr);

    if (module_instances_.insert(module_name, module_ptr).second)
    {
        ordered_module_instances_.push_back(module_ptr);
    }
}

void AFCPluginManager::RemoveModule(const std::string& module_name)
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

AFIModule* AFCPluginManager::FindModule(const std::string& module_name)
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

bool AFCPluginManager::AddUpdateModule(AFIModule* pModule)
{
    if (pModule == nullptr)
    {
        return false;
    }

    return module_updates_.insert(pModule->Name(), pModule).second;
}

void AFCPluginManager::RemoveUpdateModule(const std::string& module_name)
{
    module_updates_.erase(module_name);
}

bool AFCPluginManager::PostInit()
{
    for (const auto& iter : ordered_module_instances_)
    {
        AFIModule* pModule = iter;
        ARK_ASSERT_CONTINUE(pModule != nullptr);

        pModule->PostInit();
    }

    return true;
}

bool AFCPluginManager::CheckConfig()
{
    for (const auto& iter : ordered_module_instances_)
    {
        AFIModule* pModule = iter;
        ARK_ASSERT_CONTINUE(pModule != nullptr);

        pModule->CheckConfig();
    }

    return true;
}

bool AFCPluginManager::PreUpdate()
{
    for (const auto& iter : ordered_module_instances_)
    {
        AFIModule* pModule = iter;
        ARK_ASSERT_CONTINUE(pModule != nullptr);

        pModule->PreUpdate();
    }

    return true;
}

bool AFCPluginManager::Update()
{
    timestamp_ = AFDateTime::GetNowTime();

    // Just loop the module which have update function
    for (const auto& iter : module_updates_)
    {
        AFIModule* pModule = iter.second;
        ARK_ASSERT_CONTINUE(pModule != nullptr);

        pModule->Update();
    }

    return true;
}

bool AFCPluginManager::PreShut()
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

bool AFCPluginManager::Shut()
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

bool AFCPluginManager::LoadPluginLibrary(const std::string& plugin_name)
{
    auto iter = plugin_libs_.find(plugin_name);
    ARK_ASSERT_RET_VAL(iter == plugin_libs_.end(), false);

    AFCDynLib* pLib = ARK_NEW AFCDynLib(plugin_name);
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
            CONSOLE_LOG << stderr << " Load shared library[" << pLib->GetName() << "] failed, ErrorNo=[" << error << "]"
                        << std::endl;
            CONSOLE_LOG << "Load [" << pLib->GetName() << "] failed" << std::endl;
            assert(0);
            return false;
        }
#endif
    }

    return true;
}

bool AFCPluginManager::UnloadPluginLibrary(const std::string& plugin_name)
{
    auto iter = plugin_libs_.find(plugin_name);
    ARK_ASSERT_RET_VAL(iter != plugin_libs_.end(), false);

    AFCDynLib* pDynLib = iter->second;
    ARK_ASSERT_RET_VAL(pDynLib == nullptr, false);

    auto func = (DLL_EXIT_PLUGIN_FUNC)pDynLib->GetSymbol("DllExitPlugin");
    ARK_ASSERT_RET_VAL(func != nullptr, false);
    func(this);

    pDynLib->UnLoad();
    ARK_DELETE(pDynLib);

    plugin_libs_.erase(plugin_name);
    return true;
}

} // namespace ark
