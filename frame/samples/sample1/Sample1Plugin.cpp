/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
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

#include "Sample1Plugin.h"
#include "Sample1Module.h"

namespace ark
{

    ARK_DLL_PLUGIN_ENTRY(Sample1Plugin)
    ARK_DLL_PLUGIN_EXIT(Sample1Plugin)

    //////////////////////////////////////////////////////////////////////////

    int Sample1Plugin::GetPluginVersion()
    {
        return 0;
    }

    const std::string Sample1Plugin::GetPluginName()
    {
        return GET_CLASS_NAME(Sample1Plugin)
    }

    void Sample1Plugin::Install()
    {
        RegisterModule<Sample1Module, Sample1Module>();
    }

    void Sample1Plugin::Uninstall()
    {
        DeregisterModule<Sample1Module, Sample1Module>();
    }

}