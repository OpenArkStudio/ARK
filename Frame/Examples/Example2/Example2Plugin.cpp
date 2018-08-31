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

#include "Example2Plugin.h"
#include "Example2Module.h"

ARK_DLL_PLUGIN_ENTRY(Example2Plugin)
ARK_DLL_PLUGIN_EXIT(Example2Plugin)

//////////////////////////////////////////////////////////////////////////

int Example2Plugin::GetPluginVersion()
{
    return 0;
}

const std::string Example2Plugin::GetPluginName()
{
    return GET_CLASS_NAME(Example2Plugin)
}

void Example2Plugin::Install()
{
    RegisterModule<Example2Module, Example2Module>();
}

void Example2Plugin::Uninstall()
{
    DeregisterModule<Example2Module, Example2Module>();
}