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


#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "base/AFPlatform.hpp"
#include "base/AFDateTime.hpp"
#include "interface/AFIPlugin.h"
#include "AFCPluginManager.h"

namespace ark
{

    AFCPluginManager::AFCPluginManager() : AFIPluginManager()
    {
        timestamp_ = AFDateTime::GetNowTime();
    }

    inline bool AFCPluginManager::Init()
    {
        //load plugin configuration
        if (!LoadPluginConf())
        {
            return false;
        }

        //load plugin dynamic libraries
        for (const auto& iter : ordered_plugin_names_)
        {
            bool bRet = LoadPluginLibrary(iter);

            if (!bRet)
            {
                return false;
            }
        }

        //initialize all modules
        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->Init();
            }
        }

        return true;
    }

    bool AFCPluginManager::LoadPluginConf()
    {
        rapidxml::file<> fdoc(plugin_conf_path_.c_str());
        rapidxml::xml_document<>  doc;
        doc.parse<0>(fdoc.data());

        rapidxml::xml_node<>* pRoot = doc.first_node();
        rapidxml::xml_node<>* pPluginsNode = pRoot->first_node("plugins");
        if (pPluginsNode == nullptr)
        {
            return false;
        }

        if (pPluginsNode->first_attribute("path") == nullptr)
        {
            ARK_ASSERT(0, "There are no ConfigPath.Name", __FILE__, __FUNCTION__);
            return false;
        }

        plugin_path_ = pPluginsNode->first_attribute("path")->value();

        for (rapidxml::xml_node<>* pPluginNode = pPluginsNode->first_node("plugin"); pPluginNode != nullptr; pPluginNode = pPluginNode->next_sibling("plugin"))
        {
            const char* strPluginName = pPluginNode->first_attribute("name")->value();
            if (plugin_names_.insert(std::make_pair(strPluginName, true)).second)
            {
                ordered_plugin_names_.emplace_back(strPluginName);
            }
        }

        rapidxml::xml_node<>* pResNode = pRoot->first_node("res");

        if (!pResNode)
        {
            ARK_ASSERT(0, "There is no res node", __FILE__, __FUNCTION__);
            return false;
        }

        if (pResNode->first_attribute("path") == nullptr)
        {
            ARK_ASSERT(0, "There is no res.path", __FILE__, __FUNCTION__);
            return false;
        }

        res_path_ = pResNode->first_attribute("path")->value();

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
        if (find_plugin == nullptr)
        {
            return;
        }

        plugin->Uninstall();
        plugin_instances_.erase(plugin->GetPluginName());
        ARK_DELETE(plugin);
    }

    AFIPlugin* AFCPluginManager::FindPlugin(const std::string& plugin_name)
    {
        return plugin_instances_.find(plugin_name)->second;
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
        if (file_path.empty())
        {
            return;
        }

        if (file_path.find(".plugin") == string::npos)
        {
            return;
        }

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

    bool AFCPluginManager::PostInit()
    {
        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->PostInit();
            }
        }

        return true;
    }

    bool AFCPluginManager::CheckConfig()
    {
        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->CheckConfig();
            }
        }

        return true;
    }

    bool AFCPluginManager::PreUpdate()
    {
        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->PreUpdate();
            }
        }

        return true;
    }

    bool AFCPluginManager::Update()
    {
        timestamp_ = AFDateTime::GetNowTime();

        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->Update();
            }
        }

        return true;
    }

    bool AFCPluginManager::PreShut()
    {
        timestamp_ = AFDateTime::GetNowTime();

        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->PreShut();
            }
        }

        return true;
    }

    bool AFCPluginManager::Shut()
    {
        for (const auto& iter : ordered_module_instances_)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->Shut();
            }
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
        if (iter == plugin_libs_.end())
        {
            return false;
        }

        AFCDynLib* pDynLib = iter->second;
        if (pDynLib != nullptr)
        {
            return false;
        }

        AFCDynLib* pLib = ARK_NEW AFCDynLib(plugin_name);
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
#if ARK_PLATFORM == PLATFORM_UNIX
            char* error = dlerror();
            if (error)
            {
                CONSOLE_LOG << stderr << " Load shared library[" << pLib->GetName() << "] failed, ErrorNo. = [" << error << "]" << std::endl;
                CONSOLE_LOG << "Load [" << pLib->GetName() << "] failed" << std::endl;
                assert(0);
                return false;
            }
#elif ARK_PLATFORM == PLATFORM_WIN
            CONSOLE_LOG << "Load dynamic library[" << pLib->GetName() << "] failed, ErrorNo. = [" << GetLastError() << "]" << std::endl;
            CONSOLE_LOG << "Load [" << pLib->GetName() << "] failed" << std::endl;
            assert(0);
            return false;
#endif // ARK_PLATFORM
        }

        return true;
    }

    bool AFCPluginManager::UnloadPluginLibrary(const std::string& plugin_name)
    {
        auto iter = plugin_libs_.find(plugin_name);
        if (iter == plugin_libs_.end())
        {
            return false;
        }

        AFCDynLib* pDynLib = iter->second;
        if (pDynLib == nullptr)
        {
            return false;
        }

        auto func = (DLL_EXIT_PLUGIN_FUNC)pDynLib->GetSymbol("DllExitPlugin");
        ARK_ASSERT_RET_VAL(func != nullptr, false);
        func(this);

        pDynLib->UnLoad();
        ARK_DELETE(pDynLib);

        plugin_libs_.erase(plugin_name);
        return true;
    }

    //bool AFCPluginManager::StartReLoadState()
    //{
    //    AFIModule::StartReLoadState();

    //    for (AFIPlugin* pPlugin = plugin_instances_.First(); pPlugin != nullptr; pPlugin = plugin_instances_.Next())
    //    {
    //        pPlugin->StartReLoadState();
    //    }

    //    return true;
    //}

    //bool AFCPluginManager::EndReLoadState()
    //{
    //    for (AFIPlugin* pPlugin = plugin_instances_.First(); pPlugin != nullptr; pPlugin = plugin_instances_.Next())
    //    {
    //        pPlugin->EndReLoadState();
    //    }

    //    return AFIModule::EndReLoadState();
    //}
}