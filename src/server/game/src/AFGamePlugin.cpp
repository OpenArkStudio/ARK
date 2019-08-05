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

#include "base/AFPluginManager.hpp"
#include "game/include/AFGamePlugin.hpp"
#include "game/include/AFCSceneProcessModule.hpp"
#include "game/include/AFCPropertyModule.hpp"
#include "game/include/AFCLevelModule.hpp"
#include "game/include/AFCPropertyConfigModule.hpp"
#include "game/include/AFCAccountModule.hpp"
#include "game/include/AFCGameNetModule.hpp"

namespace ark {

ARK_PLUGIN_DECLARE(AFGamePlugin)

//////////////////////////////////////////////////////////////////////////
int AFGamePlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFGamePlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFGamePlugin);
}

void AFGamePlugin::Install()
{
    RegisterModule<AFISceneProcessModule, AFCSceneProcessModule>();
    RegisterModule<AFIPropertyModule, AFCPropertyModule>();
    RegisterModule<AFILevelModule, AFCLevelModule>();
    RegisterModule<AFIPropertyConfigModule, AFCPropertyConfigModule>();
    RegisterModule<AFIAccountModule, AFCAccountModule>();
    RegisterModule<AFIGameNetModule, AFCGameNetModule>();
}

void AFGamePlugin::Uninstall()
{
    DeregisterModule<AFIGameNetModule, AFCGameNetModule>();
    DeregisterModule<AFIAccountModule, AFCAccountModule>();
    DeregisterModule<AFIPropertyConfigModule, AFCPropertyConfigModule>();
    DeregisterModule<AFILevelModule, AFCLevelModule>();
    DeregisterModule<AFIPropertyModule, AFCPropertyModule>();
    DeregisterModule<AFISceneProcessModule, AFCSceneProcessModule>();
}

} // namespace ark
