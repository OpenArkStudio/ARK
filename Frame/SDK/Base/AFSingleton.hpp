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

#ifndef AF_MACROS_H
#define AF_MACROS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "AFPlatform.hpp"

template <typename T>
class Singleton
{
public:
    Singleton(void)
    {
        assert(!instance_);
#if ARK_PLATFORM == PLATFORM_WIN && _MSC_VER < 1200
        int offset = (int)(T*)1 - (int)(Singleton<T>*)(T*)1;
        instance_ = (T*)((int)this + offset);
#else
        instance_ = static_cast<T*>(this);
#endif
    }

    ~Singleton()
    {
        assert(instance_);
        instance_ = 0;
    }

    static T& GetInstance()
    {
        assert(instance_);
        return (*instance_);
    }

    static GetInstancePtr()
    {
        assert(instance_);
        return instance_;
    }
protected:
    static T* instance_;
};

#endif // AF_MACROS_H