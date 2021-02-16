/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "base/AFPlatform.hpp"
#include "base/AFMacros.hpp"
#include "base/AFNoncopyable.hpp"

namespace ark {

template<typename T>
class AFSingleton : public AFNoncopyable
{
public:
    static T* instance()
    {
        std::call_once(once_, &AFSingleton<T>::Init);
        return instance_;
    }

    static T& get_instance()
    {
        return *instance();
    }

    static void ShutDown()
    {
        ARK_DELETE(instance_);
    }

    // We can pass a pointer to init it manually.
    static void SetInstance(T* single)
    {
        instance_ = single;
    }

private:
    static void Init()
    {
        if (instance_ == nullptr)
        {
            instance_ = ARK_NEW T();
        }
    }

    static std::once_flag once_;
    static T* instance_;
};

template<typename T>
std::once_flag AFSingleton<T>::once_;

template<typename T>
T* AFSingleton<T>::instance_ = nullptr;

} // namespace ark
