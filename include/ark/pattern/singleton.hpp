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

#include "ark/macros.hpp"
#include "ark/noncopyable.hpp"
#include "ark/platform.hpp"

namespace ark {

template<typename T>
class singleton : public noncopyable
{
public:
    static T* instance() {
        std::call_once(once_, &singleton<T>::init);
        return instance_;
    }

    static T& get_instance() {
        return *instance();
    }

    static void shutdown() {
        ARK_DELETE(instance_);
    }

    // We can pass a pointer to init it manually.
    static void set_instance(T* single) {
        instance_ = single;
    }

private:
    static void init() {
        if (instance_ == nullptr)
        {
            instance_ = ARK_NEW T();
        }
    }

    static std::once_flag once_;
    static T* instance_;
};

template<typename T>
std::once_flag singleton<T>::once_;

template<typename T>
T* singleton<T>::instance_ = nullptr;

} // namespace ark
