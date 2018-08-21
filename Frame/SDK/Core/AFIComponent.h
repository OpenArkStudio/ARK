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

#include "SDK/Core/AFGUID.hpp"
#include "SDK/Interface/AFIModule.h"

class AFIComponent : public AFIModule
{
public:

    AFIComponent() = delete;
    AFIComponent(AFGUID self, const std::string& strName)
    {
        mbHasInit = false;
        mbEnable = true;
        mSelf = self;
        mstrName = strName;
    }

    virtual ~AFIComponent() = default;

    template <typename T>
    ARK_SHARE_PTR<T> CreateNewInstance()
    {
        ARK_SHARE_PTR<AFIComponent> pComponent = CreateNewInstance();

        if (nullptr != pComponent)
        {
            if (TIsDerived<T, AFIComponent>::Result)
            {
                ARK_SHARE_PTR<T> pT = std::dynamic_pointer_cast<T>(pComponent);

                if (nullptr != pT)
                {
                    return pT;
                }
            }
        }

        return ARK_SHARE_PTR<T>();
    }

    virtual bool SetEnable(const bool bEnable)
    {
        return mbEnable;
    }

    virtual bool Enable()
    {
        return mbEnable;
    }

    virtual bool SetHasInit(const bool bEnable)
    {
        mbHasInit = bEnable;
        return mbHasInit;
    }

    virtual bool HasInit()
    {
        return mbHasInit;
    }

    virtual AFGUID Self()
    {
        return NULL_GUID;
    }

    virtual const std::string& GetComponentName() const
    {
        return mstrName;
    }

    virtual int OnASyncEvent(const AFGUID& self, const int event, std::string& arg)
    {
        return 0;
    }

protected:
    virtual ARK_SHARE_PTR<AFIComponent> CreateNewInstance()
    {
        return nullptr;
    };

private:
    bool mbEnable;
    bool mbHasInit;
    AFGUID mSelf;
    std::string mstrName;
};