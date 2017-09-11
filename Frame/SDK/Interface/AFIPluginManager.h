/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file  	AFIPluginManager.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFIPluginManager                                                  *
*****************************************************************************/
#pragma once

#include "AFIModule.h"

#define FIND_MODULE(classBaseName, className)  \
    assert((TIsDerived<classBaseName, AFIModule>::Result));

class AFIPlugin;

class AFIPluginManager : public AFIModule
{
public:
    AFIPluginManager()
    {

    }

    template <typename T>
    T* FindModule()
    {
        AFIModule* pLogicModule = FindModule(typeid(T).name());
        if (pLogicModule)
        {
            if (!TIsDerived<T, AFIModule>::Result)
            {
                return NULL;
            }

            T* pT = dynamic_cast<T*>(pLogicModule);
            assert(NULL != pT);

            return pT;
        }
        assert(NULL);
        return NULL;
    }

    virtual void Registered(AFIPlugin* plugin) = 0;

    virtual void UnRegistered(AFIPlugin* plugin) = 0;

    virtual AFIPlugin* FindPlugin(const std::string& strPluginName) = 0;

    virtual void AddModule(const std::string& strModuleName, AFIModule* pModule) = 0;

    virtual void RemoveModule(const std::string& strModuleName) = 0;

    virtual AFIModule* FindModule(const std::string& strModuleName) = 0;

    virtual int AppID() const = 0;
    virtual int64_t GetInitTime() const = 0;
    virtual int64_t GetNowTime() const = 0;
    virtual const std::string& GetConfigPath() const = 0;
    virtual void SetConfigName(const std::string& strFileName) = 0;
};

