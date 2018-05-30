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

#include "AFCDynLib.h"
#include "SDK/Core/Base/AFSingleton.hpp"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Core/Base/AFArrayMap.hpp"

class AFCPluginManager : public AFIPluginManager, public AFSingleton<AFCPluginManager>
{
public:
    AFCPluginManager();
    virtual ~AFCPluginManager();

    virtual bool Init();

    virtual bool PostInit();

    virtual bool CheckConfig();

    virtual bool PreShut();

    virtual bool Shut();

    //////////////////////////////////////////////////////////////////////////

    virtual void Registered(AFIPlugin* pPlugin);

    virtual void UnRegistered(AFIPlugin* pPlugin);

    virtual bool StartReLoadState();

    virtual bool EndReLoadState();
    //////////////////////////////////////////////////////////////////////////

    virtual AFIPlugin* FindPlugin(const std::string& strPluginName);

    virtual void AddModule(const std::string& strModuleName, AFIModule* pModule);

    virtual void RemoveModule(const std::string& strModuleName);

    virtual AFIModule* FindModule(const std::string& strModuleName);

    virtual bool Update();

    virtual int AppID() const;
    virtual const std::string& AppName() const;

    virtual int64_t GetInitTime() const;

    virtual int64_t GetNowTime() const;

    virtual const std::string& GetConfigPath() const;

    virtual void SetConfigName(const std::string& strFileName);

    virtual void SetAppID(const int app_id);

    virtual void SetAppName(const std::string& app_name);

protected:
    bool LoadPluginConfig();

    bool LoadStaticPlugin(const std::string& strPluginDLLName);
    bool LoadPluginLibrary(const std::string& strPluginDLLName);
    bool UnLoadPluginLibrary(const std::string& strPluginDLLName);
    bool UnLoadStaticPlugin(const std::string& strPluginDLLName);

private:
    int mnAppID;
    int64_t mnInitTime;
    int64_t mnNowTime;
    std::string mstrConfigPath;
    std::string mstrConfigName;
    std::string mstrAppName;

    typedef void(*DLL_START_PLUGIN_FUNC)(AFIPluginManager* pm/*, MemoryPool* pMalloc*/);
    typedef void(*DLL_STOP_PLUGIN_FUNC)(AFIPluginManager* pm);

    std::map<std::string, bool> mxPluginNameMap;
    AFMap<std::string, AFCDynLib> mxPluginLibMap;
    AFMap<std::string, AFIPlugin> mxPluginInstanceMap;
    AFMap<std::string, AFIModule> mxModuleInstanceMap;
};