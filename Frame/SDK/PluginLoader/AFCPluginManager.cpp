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


#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "SDK/Core/AFPlatform.hpp"
#include "SDK/Core/AFDateTime.hpp"
#include "SDK/Interface/AFIPlugin.h"
#include "AFCPluginManager.h"

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32.lib" )
#endif

AFCPluginManager::AFCPluginManager() : AFIPluginManager()
{
    mnNowTime = AFDateTime::GetNowTime();
}

inline bool AFCPluginManager::Init()
{
    if (!LoadPluginConf())
    {
        return false;
    }

    for (auto it = mxPluginNameMap.begin(); it != mxPluginNameMap.end(); ++it)
    {
        bool bRet = LoadPluginLibrary(it->first);

        if (!bRet)
        {
            return false;
        }
    }

    for (AFIPlugin* pPlugin = mxPluginInstanceMap.First(); pPlugin != nullptr; pPlugin = mxPluginInstanceMap.Next())
    {
        pPlugin->Init();
    }

    return true;
}

bool AFCPluginManager::LoadPluginConf()
{
    rapidxml::file<> fdoc(mstrPluginConf.c_str());
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

    mstrPluginPath = pPluginsNode->first_attribute("path")->value();

    for (rapidxml::xml_node<>* pPluginNode = pPluginsNode->first_node("plugin"); pPluginNode != nullptr; pPluginNode = pPluginNode->next_sibling("plugin"))
    {
        const char* strPluginName = pPluginNode->first_attribute("name")->value();
        mxPluginNameMap.insert(std::make_pair(strPluginName, true));
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

    mstrResPath = pResNode->first_attribute("path")->value();

    return true;
}

void AFCPluginManager::Register(AFIPlugin* plugin)
{
    std::string strPluginName = plugin->GetPluginName();

    if (!FindPlugin(strPluginName))
    {
        mxPluginInstanceMap.AddElement(strPluginName, plugin);
        plugin->Install();
    }
    else
    {
        ARK_ASSERT_NO_EFFECT(0);
    }
}

void AFCPluginManager::Deregister(AFIPlugin* plugin)
{
    AFIPlugin* pPlugin = mxPluginInstanceMap.GetElement(plugin->GetPluginName());

    if (pPlugin != nullptr)
    {
        pPlugin->Uninstall();
        mxPluginInstanceMap.RemoveElement(pPlugin->GetPluginName());
        ARK_DELETE(pPlugin);
    }
}

AFIPlugin* AFCPluginManager::FindPlugin(const std::string& strPluginName)
{
    return mxPluginInstanceMap.GetElement(strPluginName);
}

inline int AFCPluginManager::BusID() const
{
    return mnBusID;
}

const std::string& AFCPluginManager::AppName() const
{
    return mstrAppName;
}

inline int64_t AFCPluginManager::GetNowTime() const
{
    return mnNowTime;
}

inline const std::string& AFCPluginManager::GetResPath() const
{
    return mstrResPath;
}

void AFCPluginManager::SetPluginConf(const std::string& strFileName)
{
    if (strFileName.empty())
    {
        return;
    }

    if (strFileName.find(".xml") == string::npos)
    {
        return;
    }

    mstrPluginConf = strFileName;
}

void AFCPluginManager::SetBusID(const int app_id)
{
    mnBusID = app_id;
}

void AFCPluginManager::SetAppName(const std::string& app_name)
{
    mstrAppName = app_name;
}

void AFCPluginManager::AddModule(const std::string& strModuleName, AFIModule* pModule)
{
    ARK_ASSERT_RET_NONE(FindModule(strModuleName) == nullptr);

    mxModuleInstanceMap.AddElement(strModuleName, pModule);
}

void AFCPluginManager::RemoveModule(const std::string& strModuleName)
{
    mxModuleInstanceMap.RemoveElement(strModuleName);
}

AFIModule* AFCPluginManager::FindModule(const std::string& strModuleName)
{
    return mxModuleInstanceMap.GetElement(strModuleName);
}

bool AFCPluginManager::PostInit()
{
    for (AFIPlugin* pPlugin = mxPluginInstanceMap.First(); pPlugin != nullptr; pPlugin = mxPluginInstanceMap.Next())
    {
        pPlugin->PostInit();
    }

    return true;
}

bool AFCPluginManager::CheckConfig()
{
    for (AFIPlugin* pPlugin = mxPluginInstanceMap.First(); pPlugin != nullptr; pPlugin = mxPluginInstanceMap.Next())
    {
        pPlugin->CheckConfig();
    }

    return true;
}

bool AFCPluginManager::PreUpdate()
{
    for (AFIPlugin* pPlugin = mxPluginInstanceMap.First(); pPlugin != nullptr; pPlugin = mxPluginInstanceMap.Next())
    {
        pPlugin->PreUpdate();
    }

    return true;
}

bool AFCPluginManager::Update()
{
    mnNowTime = AFDateTime::GetNowTime();

    for (AFIPlugin* pPlugin = mxPluginInstanceMap.First(); pPlugin != nullptr; pPlugin = mxPluginInstanceMap.Next())
    {
        pPlugin->Update();
    }

    return true;
}

bool AFCPluginManager::PreShut()
{
    for (AFIPlugin* pPlugin = mxPluginInstanceMap.First(); pPlugin != nullptr; pPlugin = mxPluginInstanceMap.Next())
    {
        pPlugin->PreShut();
    }

    return true;
}

bool AFCPluginManager::Shut()
{
    for (AFIPlugin* pPlugin = mxPluginInstanceMap.First(); pPlugin != nullptr; pPlugin = mxPluginInstanceMap.Next())
    {
        pPlugin->Shut();
    }

    for (auto it : mxPluginNameMap)
    {
        UnLoadPluginLibrary(it.first);
    }

    mxPluginInstanceMap.ClearAll();
    mxPluginNameMap.clear();
    return true;
}

bool AFCPluginManager::LoadPluginLibrary(const std::string& strPluginDLLName)
{
    AFCDynLib* pDynLib = mxPluginLibMap.GetElement(strPluginDLLName);
    if (pDynLib != nullptr)
    {
        return false;
    }

    AFCDynLib* pLib = new AFCDynLib(strPluginDLLName);
    bool bLoad = pLib->Load(mstrPluginPath);
    if (bLoad)
    {
        mxPluginLibMap.AddElement(strPluginDLLName, pLib);
        DLL_START_PLUGIN_FUNC pFunc = (DLL_START_PLUGIN_FUNC)pLib->GetSymbol("DllStartPlugin");
        if (pFunc == nullptr)
        {
            CONSOLE_LOG << "Find function DllStartPlugin Failed in [" << pLib->GetName() << "]" << std::endl;
            assert(0);
            return false;
        }

        pFunc(this);

        return true;
    }
    else
    {
#if ARK_PLATFORM == PLATFORM_UNIX
        char* error = dlerror();
        if (error)
        {
            CONSOLE_LOG << stderr << " Load shared library[" << pLib->GetName() << "] failed, ErrorNO = [" << error << "]" << std::endl;
            CONSOLE_LOG << "Load [" << pLib->GetName() << "] failed" << std::endl;
            assert(0);
            return false;
        }

#elif ARK_PLATFORM == PLATFORM_WIN
        CONSOLE_LOG << stderr << " Load dynamic library[" << pLib->GetName() << "] failed, ErrorNO = [" << GetLastError() << "]" << std::endl;
        CONSOLE_LOG << "Load [" << pLib->GetName() << "] failed" << std::endl;
        assert(0);
        return false;
#endif // ARK_PLATFORM
    }
}

bool AFCPluginManager::UnLoadPluginLibrary(const std::string& strPluginDLLName)
{
    AFCDynLib* pDynLib = mxPluginLibMap.GetElement(strPluginDLLName);
    if (pDynLib == nullptr)
    {
        return false;
    }

    DLL_STOP_PLUGIN_FUNC pFunc = (DLL_STOP_PLUGIN_FUNC)pDynLib->GetSymbol("DllStopPlugin");
    if (pFunc != nullptr)
    {
        pFunc(this);
    }

    pDynLib->UnLoad();

    ARK_DELETE(pDynLib);
    mxPluginLibMap.RemoveElement(strPluginDLLName);
    return true;
}

bool AFCPluginManager::StartReLoadState()
{
    AFIModule::StartReLoadState();

    for (AFIPlugin* pPlugin = mxPluginInstanceMap.First(); pPlugin != nullptr; pPlugin = mxPluginInstanceMap.Next())
    {
        pPlugin->StartReLoadState();
    }

    return true;
}

bool AFCPluginManager::EndReLoadState()
{
    for (AFIPlugin* pPlugin = mxPluginInstanceMap.First(); pPlugin != nullptr; pPlugin = mxPluginInstanceMap.Next())
    {
        pPlugin->EndReLoadState();
    }

    return AFIModule::EndReLoadState();
}