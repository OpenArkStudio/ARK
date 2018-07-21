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

#include "AFCProcModule.h"

#define DECLARE_ARK_PROC_NAME(type_name) \
    do  \
    {   \
        std::string name = #type_name;  \
        name = name.substr(strlen("ARK_PROC_"), name.length() - strlen("ARK_PROC_")); \
        name = AFMisc::ToLower(name);   \
        mxProcNames[type_name] = name;  \
    } while (0);

AFCProcModule::AFCProcModule(AFIPluginManager* p)
{
    pPluginManager = p;
    InitProcNames();
    std::cout << "test" << std::endl;
}

const std::string& AFCProcModule::GetProcName(const ARK_PROCESS_TYPE& type)
{
    auto iter = mxProcNames.find(type);
    if (iter != mxProcNames.end())
    {
        return iter->second;
    }
    else
    {
        ARK_ASSERT_NO_EFFECT(0);
        return NULL_STR;
    }
}

bool AFCProcModule::InitProcNames()
{
    //same as ARK_PROCESS_TYPE order

    //cluster
    DECLARE_ARK_PROC_NAME(ARK_PROC_MASTER);
    DECLARE_ARK_PROC_NAME(ARK_PROC_DIR);
    DECLARE_ARK_PROC_NAME(ARK_PROC_LOG);
    DECLARE_ARK_PROC_NAME(ARK_PROC_ROUTER);
    //to add

    //world
    DECLARE_ARK_PROC_NAME(ARK_PROC_WORLD);
    DECLARE_ARK_PROC_NAME(ARK_PROC_GAME);
    DECLARE_ARK_PROC_NAME(ARK_PROC_LOGIN);
    DECLARE_ARK_PROC_NAME(ARK_PROC_PROXY);
    DECLARE_ARK_PROC_NAME(ARK_PROC_DB);
    //to add

    return true;
}
