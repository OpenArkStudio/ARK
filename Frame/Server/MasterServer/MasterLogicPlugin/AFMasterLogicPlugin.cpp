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

#include "AFCMasterModule.h"
#include "AFMasterLogicPlugin.h"

#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTitle("MasterServer -- ArkGame");
#endif

    CREATE_PLUGIN(pm, AFMasterLogicPlugin)
}

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFMasterLogicPlugin)
}

#endif
//////////////////////////////////////////////////////////////////////////

int AFMasterLogicPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFMasterLogicPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFMasterLogicPlugin)
}

void AFMasterLogicPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIMasterModule, AFCMasterModule)
}

void AFMasterLogicPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIMasterModule, AFCMasterModule)
}