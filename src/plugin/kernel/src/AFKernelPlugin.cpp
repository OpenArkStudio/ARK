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

#include "kernel/include/AFKernelPlugin.hpp"
#include "kernel/include/AFCKernelModule.hpp"
#include "kernel/include/AFCClassMetaModule.hpp"
#include "kernel/include/AFCConfigModule.hpp"
#include "kernel/include/AFCMapModule.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(AFKernelPlugin)

void AFKernelPlugin::Install()
{
    ARK_REGISTER_MODULE(AFIClassMetaModule, AFCClassMetaModule);
    ARK_REGISTER_MODULE(AFIConfigModule, AFCConfigModule);
    ARK_REGISTER_MODULE(AFIKernelModule, AFCKernelModule);
    ARK_REGISTER_MODULE(AFIMapModule, AFCMapModule);
}

void AFKernelPlugin::Uninstall()
{
    ARK_DEREGISTER_MODULE(AFIMapModule, AFCMapModule);
    ARK_DEREGISTER_MODULE(AFIKernelModule, AFCKernelModule);
    ARK_DEREGISTER_MODULE(AFIConfigModule, AFCConfigModule);
    ARK_DEREGISTER_MODULE(AFIClassMetaModule, AFCClassMetaModule);
}

} // namespace ark
