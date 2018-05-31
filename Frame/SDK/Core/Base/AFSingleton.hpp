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
#include "AFSpinLock.hpp"

template <class Derived>
class AFSingleton
{
public:
    static inline Derived& GetInstance()
    {
        return *GetInstancePtr();
    }

    static inline Derived* GetInstancePtr()
    {
        static Derived *instancePointer = CreateInstance();
        return instancePointer;
    }

protected:
    using Access = AFSingleton<Derived>;

    AFSingleton(void) = default;
    AFSingleton(AFSingleton const &) = default;
    AFSingleton(AFSingleton &&) = default;
    AFSingleton &operator=(AFSingleton const &) = default;
    AFSingleton &operator=(AFSingleton &&) = default;
    virtual ~AFSingleton(void) = default;

private:
    static Derived* InstancePointer;
    static AFSpinLock Lock;

    static inline Derived *CreateInstance()
    {
        if (AFSingleton::InstancePointer == nullptr)
        {
            std::lock_guard<decltype(AFSingleton::Lock)> lock(AFSingleton::Lock);
            if (AFSingleton::InstancePointer == nullptr)
            {
                void *data = static_cast<void *>(GetData());
                new (data) Derived();
                AFSingleton::InstancePointer = reinterpret_cast<Derived *>(data);
                std::atexit(&AFSingleton::DestroyInstance);
            }
        }

        return AFSingleton::InstancePointer;
    }

    static inline void DestroyInstance(void)
    {
        reinterpret_cast<Derived *>(GetData())->~Derived();
    }

    static inline unsigned char *GetData(void)
    {
        static unsigned char data[sizeof(Derived)];
        return data;
    }
};

template <class Derived>
Derived *AFSingleton<Derived>::InstancePointer = nullptr;

template <class Derived>
AFSpinLock AFSingleton<Derived>::Lock;