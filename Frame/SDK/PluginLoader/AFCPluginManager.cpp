/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "AFCPluginManager.h"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_iterators.hpp"
#include "RapidXML/rapidxml_print.hpp"
#include "RapidXML/rapidxml_utils.hpp"
#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Base/AFPlatform.hpp"

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32.lib" )
#endif

AFCPluginManager::AFCPluginManager() : AFIPluginManager()
{
    mnAppID = 0;
    mnInitTime = time(NULL);
    mnNowTime = mnInitTime;

    mstrConfigPath = "";
    mstrConfigName = "Plugin.xml";
}

AFCPluginManager::~AFCPluginManager()
{

}

inline bool AFCPluginManager::Init()
{
    if(!LoadPluginConfig())
    {
        return false;
    }

    for(auto it = mPluginNameMap.begin(); it != mPluginNameMap.end(); ++it)
    {
#ifdef ARK_DYNAMIC_PLUGIN
        bool bRet = LoadPluginLibrary(it->first);
        if(!bRet)
        {
            return false;
        }
#else
        bool bRet = LoadStaticPlugin(it->first);
        if(!bRet)
        {
            return false;
        }
#endif
    }

    size_t nPluginInstanceCount = mPluginInstanceMap.GetCount();
    for (int i = 0; i < nPluginInstanceCount; ++i)
    {
        AFIPlugin* pPlugin = mPluginInstanceMap[i];
        ARK_ASSERT_CONTINUE(NULL != pPlugin);

        pPlugin->Init();
    }

    return true;
}

bool AFCPluginManager::LoadPluginConfig()
{
    rapidxml::file<> fdoc(mstrConfigName.c_str());
    rapidxml::xml_document<>  doc;
    doc.parse<0>(fdoc.data());

    rapidxml::xml_node<>* pRoot = doc.first_node();
    for(rapidxml::xml_node<>* pPluginNode = pRoot->first_node("Plugin"); pPluginNode; pPluginNode = pPluginNode->next_sibling("Plugin"))
    {
        const char* strPluginName = pPluginNode->first_attribute("Name")->value();
        const char* strMain = pPluginNode->first_attribute("Main")->value();

        mPluginNameMap.insert(std::make_pair(strPluginName, true));
    }

    rapidxml::xml_node<>* pPluginAppNode = pRoot->first_node("APPID");
    if(!pPluginAppNode)
    {
        ARK_ASSERT(0, "There are no App ID", __FILE__, __FUNCTION__);
        return false;
    }

    const char* strAppID = pPluginAppNode->first_attribute("Name")->value();
    if(!strAppID)
    {
        ARK_ASSERT(0, "There are no App ID", __FILE__, __FUNCTION__);
        return false;
    }

    if(!ARK_FROM_STR(strAppID, mnAppID))
    {
        ARK_ASSERT(0, "App ID Convert Error", __FILE__, __FUNCTION__);
        return false;
    }

    rapidxml::xml_node<>* pPluginConfigPathNode = pRoot->first_node("ConfigPath");
    if(!pPluginConfigPathNode)
    {
        ARK_ASSERT(0, "There are no ConfigPath", __FILE__, __FUNCTION__);
        return false;
    }

    if(NULL == pPluginConfigPathNode->first_attribute("Name"))
    {
        ARK_ASSERT(0, "There are no ConfigPath.Name", __FILE__, __FUNCTION__);
        return false;
    }

    mstrConfigPath = pPluginConfigPathNode->first_attribute("Name")->value();

    return true;
}

bool AFCPluginManager::LoadStaticPlugin(const std::string& strPluginDLLName)
{
    //PluginNameList::iterator it = mPluginNameList.begin();
    //for (it; it != mPluginNameList.end(); it++)
    //{
    //    const std::string& strPluginName = *it;
    //    CREATE_PLUGIN( this, strPluginName );
    //}

    //CREATE_PLUGIN(this, AFKernelPlugin)
    //CREATE_PLUGIN(this, AFConfigPlugin)

    return false;
}

void AFCPluginManager::Registered(AFIPlugin* plugin)
{
    std::string strPluginName = plugin->GetPluginName();
    ARK_ASSERT_RET_NONE(NULL == FindPlugin(strPluginName));

    bool bFind = false;
    for (auto it = mPluginNameMap.begin(); it != mPluginNameMap.end(); ++it)
    {
        if (strPluginName == it->first)
        {
            bFind = true;
            break;
        }
    }

    if (bFind)
    {
        mPluginInstanceMap.AddElement(strPluginName, plugin);
        plugin->Install();
    }
}

void AFCPluginManager::UnRegistered(AFIPlugin* plugin)
{
    AFIPlugin* pPlugin = mPluginInstanceMap.GetElement(plugin->GetPluginName());
    if(NULL != pPlugin)
    {
        pPlugin->Uninstall();
        delete pPlugin;
        pPlugin = NULL;
        mPluginInstanceMap.RemoveElement(pPlugin->GetPluginName());
    }
}

AFIPlugin* AFCPluginManager::FindPlugin(const std::string& strPluginName)
{
    return mPluginInstanceMap.GetElement(strPluginName);
}

bool AFCPluginManager::Execute()
{
    mnNowTime = time(NULL);

    bool bRet = true;
    size_t nPluginInstanceCount = mPluginInstanceMap.GetCount();
    for (size_t i = 0; i < nPluginInstanceCount; ++i)
    {
        AFIPlugin* pPlugin = mPluginInstanceMap[i];
        ARK_ASSERT_CONTINUE(NULL != pPlugin);
        
        bRet &= pPlugin->Execute();
    }

    return bRet;
}

inline int AFCPluginManager::AppID() const
{
    return mnAppID;
}

inline int64_t AFCPluginManager::GetInitTime() const
{
    return mnInitTime;
}

inline int64_t AFCPluginManager::GetNowTime() const
{
    return mnNowTime;
}

inline const std::string & AFCPluginManager::GetConfigPath() const
{
    return mstrConfigPath;
}

void AFCPluginManager::SetConfigName(const std::string & strFileName)
{
    if(strFileName.empty())
    {
        return;
    }

    if(strFileName.find(".xml") == string::npos)
    {
        return;
    }

    mstrConfigName = strFileName;
}

void AFCPluginManager::AddModule(const std::string& strModuleName, AFIModule* pModule)
{
    ARK_ASSERT_RET_NONE(NULL == FindModule(strModuleName));

    mModuleInstanceMap.AddElement(strModuleName, pModule);
}

void AFCPluginManager::RemoveModule(const std::string& strModuleName)
{
    mModuleInstanceMap.RemoveElement(strModuleName);
}

AFIModule* AFCPluginManager::FindModule(const std::string& strModuleName)
{
    std::string strSubModuleName = strModuleName;

#if ARK_PLATFORM == PLATFORM_WIN
    std::size_t position = strSubModuleName.find(" ");
    if(string::npos != position)
    {
        strSubModuleName = strSubModuleName.substr(position + 1, strSubModuleName.length());
    }
#else
    for(int i = 0; i < strSubModuleName.length(); i++)
    {
        std::string s = strSubModuleName.substr(0, i + 1);
        int n = atof(s.c_str());
        if(strSubModuleName.length() == i + 1 + n)
        {
            strSubModuleName = strSubModuleName.substr(i + 1, strSubModuleName.length());
            break;
        }
    }
#endif

    return mModuleInstanceMap.GetElement(strSubModuleName);
}

bool AFCPluginManager::AfterInit()
{
    size_t nPluginInstanceCount = mPluginInstanceMap.GetCount();
    for(size_t i = 0; i < nPluginInstanceCount; ++i)
    {
        AFIPlugin* pPlugin = mPluginInstanceMap[i];
        ARK_ASSERT_CONTINUE(NULL != pPlugin);

        pPlugin->AfterInit();
    }

    return true;
}

bool AFCPluginManager::CheckConfig()
{
    size_t nPluginInstanceCount = mPluginInstanceMap.GetCount();
    for (size_t i = 0; i < nPluginInstanceCount; ++i)
    {
        AFIPlugin* pPlugin = mPluginInstanceMap[i];
        ARK_ASSERT_CONTINUE(NULL != pPlugin);

        pPlugin->CheckConfig();
    }

    return true;
}

bool AFCPluginManager::BeforeShut()
{
    size_t nPluginInstanceCount = mPluginInstanceMap.GetCount();
    for (size_t i = 0; i < nPluginInstanceCount; ++i)
    {
        AFIPlugin* pPlugin = mPluginInstanceMap[i];
        ARK_ASSERT_CONTINUE(NULL != pPlugin);

        pPlugin->BeforeShut();
    }

    return true;
}

bool AFCPluginManager::Shut()
{
    size_t nPluginInstanceCount = mPluginInstanceMap.GetCount();
    for (size_t i = 0; i < nPluginInstanceCount; ++i)
    {
        AFIPlugin* pPlugin = mPluginInstanceMap[i];
        ARK_ASSERT_CONTINUE(NULL != pPlugin);

        pPlugin->Shut();
    }

    for(auto it = mPluginNameMap.begin(); it != mPluginNameMap.end(); it++)
    {
#ifdef ARK_DYNAMIC_PLUGIN
        bool bRet = UnLoadPluginLibrary(it->first);
#else
        bool bRet = UnLoadStaticPlugin(it->first);
#endif
    }

    mPluginInstanceMap.Clear();
    mPluginNameMap.clear();
    return true;
}

bool AFCPluginManager::LoadPluginLibrary(const std::string& strPluginDLLName)
{
    AFCDynLib* pDynLib = mPluginLibMap.GetElement(strPluginDLLName);
    if (NULL != pDynLib)
    {
        return false;
    }

    AFCDynLib* pLib = new AFCDynLib(strPluginDLLName);
    bool bLoad = pLib->Load();

    if(bLoad)
    {
        mPluginLibMap.AddElement(strPluginDLLName, pLib);

        DLL_START_PLUGIN_FUNC pFunc = (DLL_START_PLUGIN_FUNC)pLib->GetSymbol("DllStartPlugin");
        if(!pFunc)
        {
            std::cout << "Find function DllStartPlugin Failed in [" << pLib->GetName() << "]" << std::endl;
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
        if(error)
        {
            std::cout << stderr << " Load shared lib[" << pLib->GetName() << "] failed, ErrorNo. = [" << error << "]" << std::endl;
            std::cout << "Load [" << pLib->GetName() << "] failed" << std::endl;
            assert(0);
            return false;
        }
#elif ARK_PLATFORM == PLATFORM_WIN
        std::cout << stderr << " Load DLL[" << pLib->GetName() << "] failed, ErrorNo. = [" << GetLastError() << "]" << std::endl;
        std::cout << "Load [" << pLib->GetName() << "] failed" << std::endl;
        assert(0);
        return false;
#endif // ARK_PLATFORM
    }
}

bool AFCPluginManager::UnLoadPluginLibrary(const std::string& strPluginDLLName)
{
    AFCDynLib* pDynLib = mPluginLibMap.GetElement(strPluginDLLName);
    if (NULL == pDynLib)
    {
        return false;
    }

    DLL_STOP_PLUGIN_FUNC pFunc = (DLL_STOP_PLUGIN_FUNC)pDynLib->GetSymbol("DllStopPlugin");

    if(pFunc)
    {
        pFunc(this);
    }

    pDynLib->UnLoad();

    delete pDynLib;
    pDynLib = NULL;
    mPluginLibMap.RemoveElement(strPluginDLLName);
    return true;
}

bool AFCPluginManager::UnLoadStaticPlugin(const std::string & strPluginDLLName)
{
    //DESTROY_PLUGIN(this, AFConfigPlugin)
    //DESTROY_PLUGIN(this, AFKernelPlugin)
    return false;
}

bool AFCPluginManager::StartReLoadState()
{
    AFIModule::StartReLoadState();

    size_t nPluginInstanceCount = mPluginInstanceMap.GetCount();
    for (size_t i = 0; i < nPluginInstanceCount; ++i)
    {
        AFIPlugin* pPlugin = mPluginInstanceMap[i];
        ARK_ASSERT_CONTINUE(NULL != pPlugin);

        pPlugin->StartReLoadState();
    }

    return true;
}

bool AFCPluginManager::EndReLoadState()
{
    size_t nPluginInstanceCount = mPluginInstanceMap.GetCount();
    for (size_t i = 0; i < nPluginInstanceCount; ++i)
    {
        AFIPlugin* pPlugin = mPluginInstanceMap[i];
        ARK_ASSERT_CONTINUE(NULL != pPlugin);

        pPlugin->EndReLoadState();
    }

    return AFIModule::EndReLoadState();
}
