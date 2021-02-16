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

#include "mysql/include/AFMysqlPlugin.hpp"
#include "mysql/include/AFCMysqlModule.hpp"
#include "mysql/include/AFCMysqlDriverModule.hpp"
#include "mysql/include/AFCMysqlDriverManagerModule.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(AFMysqlPlugin)

void AFMysqlPlugin::Install()
{
    // install module
    ARK_REGISTER_MODULE(AFIMysqlModule, AFCMysqlModule);
    ARK_REGISTER_MODULE(AFIMysqlDriverModule, AFCMysqlDriverModule);
    ARK_REGISTER_MODULE(AFIMysqlDriverManagerModule, AFCMysqlDriverManagerModule);
}

void AFMysqlPlugin::Uninstall()
{
    // uninstall module
    ARK_UNREGISTER_MODULE(AFIMysqlDriverManagerModule, AFCMysqlDriverManagerModule);
    ARK_UNREGISTER_MODULE(AFIMysqlDriverModule, AFCMysqlDriverModule);
    ARK_UNREGISTER_MODULE(AFIMysqlModule, AFCMysqlModule);
}

} // namespace ark
