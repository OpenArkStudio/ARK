/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file  	AFKernelPlugin.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFKernelPlugin                                                  *
*****************************************************************************/
#include "AFKernelPlugin.h"
#include "AFCKernelModule.h"
#include "AFCSceneModule.h"
#include "AFCClassModule.h"
#include "AFCElementModule.h"

#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, AFKernelPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFKernelPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int AFKernelPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFKernelPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFKernelPlugin)
}

void AFKernelPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIClassModule, AFCClassModule)
    REGISTER_MODULE(pPluginManager, AFIElementModule, AFCElementModule)
    REGISTER_MODULE(pPluginManager, AFISceneModule, AFCSceneModule)
    REGISTER_MODULE(pPluginManager, AFIKernelModule, AFCKernelModule)
}

void AFKernelPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIKernelModule, AFCKernelModule)
    UNREGISTER_MODULE(pPluginManager, AFISceneModule, AFCSceneModule)
    UNREGISTER_MODULE(pPluginManager, AFIElementModule, AFCElementModule)
    UNREGISTER_MODULE(pPluginManager, AFIClassModule, AFCClassModule)
}
