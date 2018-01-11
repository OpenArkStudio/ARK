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

#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFILogModule.h"

class HelloWorld3Module
    : public AFIModule
{
public:
    HelloWorld3Module(AFIPluginManager* p): mLastTime(0)
        , m_pKernelModule(nullptr)
        , m_pElementModule(nullptr)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool AfterInit();

    virtual bool Execute();

    virtual bool BeforeShut();
    virtual bool Shut();

protected:
    int OnEvent(const AFGUID & self, const int event, const AFIDataList & arg);
    int OnClassCallBackEvent(const AFGUID & self, const std::string & strClassName, const CLASS_OBJECT_EVENT event, const AFIDataList & arg);
    int OnPropertyCallBackEvent(const AFGUID & self, const std::string & strProperty, const AFIData & oldVarList, const AFIData & newVarList);
    int OnPropertyStrCallBackEvent(const AFGUID & self, const std::string & strProperty, const AFIData & oldVarList, const AFIData & newVarList);

    int OnHeartBeat(const AFGUID& self, const std::string& strHeartBeat, const int64_t nTime, const int nCount);

protected:
    int64_t mLastTime;
protected:
    AFIKernelModule* m_pKernelModule;
    AFIElementModule* m_pElementModule;
    AFILogModule* m_pLogModule;
};