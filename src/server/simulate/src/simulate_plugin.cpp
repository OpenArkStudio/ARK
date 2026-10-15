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

#include "simulate/include/simulate_module_impl.hpp"
#include "simulate/include/simulate_net_module_impl.hpp"
#include "simulate/include/simulate_plugin.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(simulate_plugin)

void simulate_plugin::install() {
    // install module
    ARK_REGISTER_MODULE(simulate_net_module, simulate_net_module_impl);
    ARK_REGISTER_MODULE(simulate_module, simulate_module_impl);
}

void simulate_plugin::uninstall() {
    // uninstall module
    ARK_UNREGISTER_MODULE(simulate_module, simulate_module_impl);
    ARK_UNREGISTER_MODULE(simulate_net_module, simulate_net_module_impl);
}

} // namespace ark
