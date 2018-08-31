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

#include "SDK/Core/AFSingleton.hpp"
#include "SDK/Core/AFMap.hpp"
#include "SDK/Core/AFArrayMap.hpp"
#include "SDK/Interface/AFIPluginManager.h"
#include "AFCDynLib.h"

class AFCPluginManager : public AFIPluginManager, public AFSingleton<AFCPluginManager>
{
public:
    AFCPluginManager();

    bool Init() override;
    bool PostInit() override;
    bool CheckConfig() override;
    bool PreUpdate() override;
    bool Update() override;
    bool PreShut() override;
    bool Shut() override;

    //////////////////////////////////////////////////////////////////////////

    void Register(AFIPlugin* pPlugin) override;
    void Deregister(AFIPlugin* pPlugin) override;

    bool StartReLoadState() override;
    bool EndReLoadState() override;
    //////////////////////////////////////////////////////////////////////////

    AFIPlugin* FindPlugin(const std::string& strPluginName) override;

    void AddModule(const std::string& strModuleName, AFIModule* pModule) override;
    void RemoveModule(const std::string& strModuleName) override;
    virtual AFIModule* FindModule(const std::string& strModuleName) override;

    int BusID() const override;
    virtual void SetBusID(const int app_id) override;

    const std::string& AppName() const override;
    void SetAppName(const std::string& app_name) override;

    int64_t GetNowTime() const override;

    const std::string& GetResPath() const override;

    void SetPluginConf(const std::string& strFileName) override;

protected:
    bool LoadPluginConf();

    bool LoadPluginLibrary(const std::string& strPluginDLLName);
    bool UnLoadPluginLibrary(const std::string& strPluginDLLName);

private:
    int mnBusID{0};
    int64_t mnNowTime{0};
    std::string mstrPluginPath{""};
    std::string mstrResPath{""};
    std::string mstrPluginConf{""};
    std::string mstrAppName{""};

    typedef void(*DLL_START_PLUGIN_FUNC)(AFIPluginManager* pm);
    typedef void(*DLL_STOP_PLUGIN_FUNC)(AFIPluginManager* pm);

    std::map<std::string, bool> mxPluginNameMap;
    AFMap<std::string, AFCDynLib> mxPluginLibMap;
    AFMap<std::string, AFIPlugin> mxPluginInstanceMap;
    AFMap<std::string, AFIModule> mxModuleInstanceMap;
};