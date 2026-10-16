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

#include "ark/utility/compress_module_impl.hpp"
#include "ark/utility/guid_module_impl.hpp"
#include "ark/utility/schedule_module_impl.hpp"
#include "ark/utility/timer_module_impl.hpp"
#include "ark/utility/utility_plugin.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(utility_plugin)

void utility_plugin::install() {
    ARK_REGISTER_MODULE(guid_module, guid_module_impl);
    ARK_REGISTER_MODULE(timer_module, timer_module_impl);
    ARK_REGISTER_MODULE(schedule_module, schedule_module_impl);
    ARK_REGISTER_MODULE(compress_module, compress_module_impl);
}

void utility_plugin::uninstall() {
    ARK_UNREGISTER_MODULE(compress_module, compress_module_impl);
    ARK_UNREGISTER_MODULE(schedule_module, schedule_module_impl);
    ARK_UNREGISTER_MODULE(timer_module, timer_module_impl);
    ARK_UNREGISTER_MODULE(guid_module, guid_module_impl);
}

} // namespace ark
