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

#include "bus/include/AFBusPlugin.hpp"
#include "bus/include/AFCBusModule.hpp"
#include "bus/include/AFCMsgModule.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(AFBusPlugin)

void AFBusPlugin::Install()
{
    ARK_REGISTER_MODULE(AFIBusModule, AFCBusModule);
    ARK_REGISTER_MODULE(AFIMsgModule, AFCMsgModule);
}

void AFBusPlugin::Uninstall()
{
    ARK_DEREGISTER_MODULE(AFIMsgModule, AFCMsgModule);
    ARK_DEREGISTER_MODULE(AFIBusModule, AFCBusModule);
}

} // namespace ark
