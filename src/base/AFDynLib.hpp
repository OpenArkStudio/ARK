/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFMacros.hpp"

namespace ark {

class AFDynLib
{
public:
    explicit AFDynLib(const std::string& lib_name)
        : name_(lib_name)
    {
#ifdef ARK_RUN_MODE_DEBUG
        name_.append("_d");
#endif

#ifdef ARK_PLATFORM_WIN
        name_.append(".dll");
#elif defined ARK_PLATFORM_LINUX
        name_.append(".so");
#elif defined ARK_PLATFORM_DARWIN
        name_.append(".so");
#endif

        CONSOLE_LOG << "LoadPlugin: " << name_ << std::endl;
    }

    bool Load(const std::string& path)
    {
        std::string strLibPath = path + name_;
        lib_inst_ = (DYNLIB_HANDLE)DYNLIB_LOAD(strLibPath.c_str());

        return lib_inst_ != nullptr;
    }

    bool UnLoad()
    {
        DYNLIB_UNLOAD(lib_inst_);
        return true;
    }

    // Get the name of the library
    const std::string& GetName(void) const
    {
        return name_;
    }

    DYNLIB_HANDLE GetSymbol(const std::string& symbol_name)
    {
        return (DYNLIB_HANDLE)DYNLIB_GETSYM(lib_inst_, symbol_name.c_str());
    }

private:
    std::string name_{};
    DYNLIB_HANDLE lib_inst_{nullptr};
};

} // namespace ark
