/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "utility/include/AFUtilityPlugin.hpp"
#include "utility/include/AFCGUIDModule.hpp"
#include "utility/include/AFCTimerModule.hpp"
#include "utility/include/AFCScheduleModule.hpp"
#include "utility/include/AFCCompressModule.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(AFUtilityPlugin)

void AFUtilityPlugin::Install()
{
    ARK_REGISTER_MODULE(AFIGUIDModule, AFCGUIDModule);
    ARK_REGISTER_MODULE(AFITimerModule, AFCTimerModule);
    ARK_REGISTER_MODULE(AFIScheduleModule, AFCScheduleModule);
    ARK_REGISTER_MODULE(AFICompressModule, AFCCompressModule);
}

void AFUtilityPlugin::Uninstall()
{
    ARK_UNREGISTER_MODULE(AFICompressModule, AFCCompressModule);
    ARK_UNREGISTER_MODULE(AFIScheduleModule, AFCScheduleModule);
    ARK_UNREGISTER_MODULE(AFITimerModule, AFCTimerModule);
    ARK_UNREGISTER_MODULE(AFIGUIDModule, AFCGUIDModule);
}

} // namespace ark
