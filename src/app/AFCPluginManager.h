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
#include "interface/AFIPluginManager.h"
#include "AFCDynLib.h"

namespace ark {

class ARK_EXPORT AFCPluginManager : public AFIPluginManager, public AFSingleton<AFCPluginManager>
{
public:
    AFCPluginManager();

    bool Start() override;
    bool Stop() override;
    bool Update() override;

    //////////////////////////////////////////////////////////////////////////

    void Register(AFIPlugin* pPlugin) override;
    void Deregister(AFIPlugin* pPlugin) override;
    //////////////////////////////////////////////////////////////////////////

    AFIPlugin* FindPlugin(const std::string& plugin_name) override;

    void AddModule(const std::string& module_name, AFIModule* pModule) override;
    void RemoveModule(const std::string& strModuleName) override;
    virtual AFIModule* FindModule(const std::string& strModuleName) override;

    bool AddUpdateModule(AFIModule* pModule) override;
    void RemoveUpdateModule(const std::string& module_name) override;

    int BusID() const override;
    void SetBusID(const int app_id) override;

    const std::string& AppName() const override;
    void SetAppName(const std::string& app_name) override;

    int64_t GetNowTime() const override;

    const std::string& GetResPath() const override;

    void SetPluginConf(const std::string& file_path) override;

    void SetLogPath(const std::string& log_path) override;
    const std::string& GetLogPath() const override;

protected:
    bool Init();
    bool PostInit();
    bool CheckConfig();
    bool PreUpdate();
    bool PreShut();
    bool Shut();

    bool LoadPluginConf();

    bool LoadPluginLibrary(const std::string& strPluginDLLName);
    bool UnloadPluginLibrary(const std::string& plugin_name);

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

    using DLL_ENTRY_PLUGIN_FUNC = void (*)(AFIPluginManager* p);
    using DLL_EXIT_PLUGIN_FUNC = void (*)(AFIPluginManager* p);

    std::map<std::string, bool> plugin_names_;
    std::vector<std::string> ordered_plugin_names_; // order
    AFMap<std::string, AFCDynLib> plugin_libs_;
    AFMap<std::string, AFIPlugin> plugin_instances_;
    AFMap<std::string, AFIModule> module_instances_;
    std::vector<AFIModule*> ordered_module_instances_; // order

    AFMap<std::string, AFIModule> module_updates_;
};

} // namespace ark
