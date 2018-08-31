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

#include "AFNetPlugin.h"
#include "Server/Interface/AFINetClientModule.hpp"
#include "AFCNetClientManagerModule.h"

bool AFCNetClientManagerModule::Update()
{
    for (bool bRet = mmCluster.Begin(); bRet; bRet = mmCluster.Increase())
    {
        const auto& pCluster = mmCluster.GetCurrentData();
        if (pCluster)
        {
            pCluster->Update();
        }
    }

    return true;
}

AFINetClientModule* AFCNetClientManagerModule::CreateClusterClientModule(const size_t nClusterTypeID)
{
    AFINetClientModule* pCluster = mmCluster.GetElement(nClusterTypeID);
    if (pCluster != nullptr)
    {
        //Please check why create client and client is not null
        ARK_ASSERT_RET_VAL(0, nullptr);
        return nullptr;
    }

    pCluster = ARK_NEW AFINetClientModule(pPluginManager);
    pCluster->Init();

    return pCluster;
}

bool AFCNetClientManagerModule::RemoveClusterClientModule(const size_t nClusterTypeID)
{
    AFINetClientModule* pCluster = mmCluster.GetElement(nClusterTypeID);
    if (!pCluster)
    {
        return false;
    }

    pCluster->BeforeShut();
    delete pCluster;
    mmCluster.RemoveElement(nClusterTypeID);
    return true;
}

AFINetClientModule*  AFCNetClientManagerModule::GetClusterClientModule(const size_t nClusterTypeID)
{
    return mmCluster.GetElement(nClusterTypeID);
}