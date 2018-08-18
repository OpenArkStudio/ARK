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

#include "AFGameLogicPlugin.h"
#include "AFCGameServerModule.h"
#include "AFCSceneProcessModule.h"
#include "AFCPropertyModule.h"
#include "AFCLevelModule.h"
#include "AFCPropertyConfigModule.h"
#include "AFCAccountModule.h"

#ifdef ARK_DYNAMIC_PLUGIN

ARK_DLL_PLUGIN_ENTRY(AFGameLogicPlugin)
ARK_DLL_PLUGIN_EXIT(AFGameLogicPlugin)

#endif
//////////////////////////////////////////////////////////////////////////

int AFGameLogicPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFGameLogicPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFGameLogicPlugin)
}

void AFGameLogicPlugin::Install()
{
    RegisterModule< AFIGameServerModule, AFCGameServerModule>();
    RegisterModule< AFISceneProcessModule, AFCSceneProcessModule>();
    RegisterModule< AFIPropertyModule, AFCPropertyModule>();
    RegisterModule< AFILevelModule, AFCLevelModule>();
    RegisterModule< AFIPropertyConfigModule, AFCPropertyConfigModule>();
    RegisterModule< AFIAccountModule, AFCAccountModule>();
}

void AFGameLogicPlugin::Uninstall()
{
    UnRegisterModule<AFIAccountModule, AFCAccountModule>();
    UnRegisterModule<AFIPropertyConfigModule, AFCPropertyConfigModule>();
    UnRegisterModule<AFILevelModule, AFCLevelModule>();
    UnRegisterModule<AFIPropertyModule, AFCPropertyModule>();
    UnRegisterModule<AFISceneProcessModule, AFCSceneProcessModule>();
    UnRegisterModule<AFIGameServerModule, AFCGameServerModule>();
}

