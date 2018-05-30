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

#include "AFPlatform.hpp"

template <typename T>
class AFSingleton
{
public:
    AFSingleton(void)
    {
        assert(!instance_);
#if ARK_PLATFORM == PLATFORM_WIN && _MSC_VER < 1200
        int offset = (int)(T*)1 - (int)(AFSingleton<T>*)(T*)1;
        instance_ = (T*)((int)this + offset);
#else
        instance_ = static_cast<T*>(this);
#endif
    }

    ~AFSingleton()
    {
        assert(instance_);
        instance_ = 0;
    }

    static T& GetInstance()
    {
        if(nullptr == instance_)
        {
            instance_ = new T;
        }

        assert(instance_);
        return (*instance_);
    }

    static T* GetInstancePtr()
    {
        if(nullptr == instance_)
        {
            instance_ = new T;
        }

        assert(instance_);
        return instance_;
    }

    void ReleaseInstance()
    {
        if(nullptr != instance_)
        {
            delete instance_;
            instance_ = nullptr;
        }
    }

private:
    static T* instance_;
};

template <typename T> T* AFSingleton<T>::instance_ = NULL;

