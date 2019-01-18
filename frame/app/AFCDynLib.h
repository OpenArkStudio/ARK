/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "base/AFMacros.hpp"

namespace ark
{

    class AFCDynLib
    {
    public:
        explicit AFCDynLib(const std::string& strName) :
            name_(strName)
        {
#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
            name_.append("_d");
#endif

#if ARK_PLATFORM == PLATFORM_WIN
            name_.append(".dll");
#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == AF_PLATFORM_ANDROID
            name_.append(".so");
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == AF_PLATFORM_APPLE_IOS
            name_.append(".so");
#endif

            CONSOLE_LOG << "LoadPlugin: " << name_ << std::endl;
        }

        ~AFCDynLib() = default;

        bool Load(std::string const& path);
        bool UnLoad();

        //Get the name of the library
        const std::string& GetName(void) const
        {
            return name_;
        }

        void* GetSymbol(const char* szProcName);

    private:
        std::string name_;

        DYNLIB_HANDLE lib_inst_;
    };

}