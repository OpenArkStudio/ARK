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

#include "AFProxyLogicPlugin.h"
#include "AFCProxyLogicModule.h"
#include "AFCProxyNetServerModule.h"
#include "AFCProxyServerToGameModule.h"
#include "AFCProxyServerToWorldModule.h"

ARK_DLL_PLUGIN_ENTRY(AFProxyLogicPlugin)
ARK_DLL_PLUGIN_EXIT(AFProxyLogicPlugin)

//////////////////////////////////////////////////////////////////////////

int AFProxyLogicPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFProxyLogicPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFProxyLogicPlugin)
}

void AFProxyLogicPlugin::Install()
{
    RegisterModule<AFIProxyLogicModule, AFCProxyLogicModule>();
    RegisterModule<AFIProxyNetServerModule, AFCProxyNetServerModule>();
    RegisterModule<AFIProxyServerToGameModule, AFCProxyServerToGameModule>();
    RegisterModule<AFIProxyServerToWorldModule, AFCProxyServerToWorldModule>();
}

void AFProxyLogicPlugin::Uninstall()
{
    DeregisterModule<AFIProxyServerToGameModule, AFCProxyServerToGameModule>();
    DeregisterModule<AFIProxyServerToWorldModule, AFCProxyServerToWorldModule>();
    DeregisterModule<AFIProxyNetServerModule, AFCProxyNetServerModule>();
    DeregisterModule<AFIProxyLogicModule, AFCProxyLogicModule>();
}