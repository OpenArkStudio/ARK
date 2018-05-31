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

#pragma once

#include "SDK/Core/Base/AFPlatform.hpp"
#include "SDK/Core/Base/AFMap.h"
#include "SDK/Core/Base/AFList.h"
#include "SDK/Core/Base/AFDefine.h"
#include "SDK/Core/Base/AFCDataList.h"
#include "SDK/Core/Base/AFVector3.hpp"
#include "SDK/Core/AFDataTable.h"

class AFIPluginManager;

class AFIModule
{
public:
    AFIModule()
        : pPluginManager(NULL)
        , mbReloading(false)
    {
    }

    virtual ~AFIModule() {}

    virtual bool Init()
    {
        return true;
    }

    virtual bool PostInit()
    {
        return true;
    }

    virtual bool CheckConfig()
    {
        return true;
    }

    virtual bool PreUpdate()
    {
        return true;
    }

    virtual bool Update()
    {
        return true;
    }

    virtual bool PreShut()
    {
        return true;
    }

    virtual bool Shut()
    {
        return true;
    }

    virtual bool StartReLoadState()
    {
        mbReloading = true;
        return true;
    }

    virtual bool EndReLoadState()
    {
        mbReloading = false;
        return true;
    }

    virtual AFIPluginManager* GetPluginManager() const
    {
        return pPluginManager;
    }

    void SetPluginManager(AFIPluginManager* p) { pPluginManager = p; }

    bool Loading() const
    {
        return mbReloading;
    }

    std::string strName;

protected:
    AFIPluginManager* pPluginManager;

private:
    bool mbReloading;
};