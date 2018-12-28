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

#include "AFCDynLib.h"

namespace ark
{

    bool AFCDynLib::Load(std::string const& path)
    {
        std::string strLibPath = path + name_;
        lib_inst_ = (DYNLIB_HANDLE)DYNLIB_LOAD(strLibPath.c_str());

        return lib_inst_ != nullptr;
    }

    bool AFCDynLib::UnLoad()
    {
        DYNLIB_UNLOAD(lib_inst_);
        return true;
    }

    void* AFCDynLib::GetSymbol(const char* szProcName)
    {
        return (DYNLIB_HANDLE)DYNLIB_GETSYM(lib_inst_, szProcName);
    }

}