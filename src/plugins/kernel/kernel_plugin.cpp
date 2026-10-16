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

#include "ark/kernel/class_meta_module_impl.hpp"
#include "ark/kernel/config_module_impl.hpp"
#include "ark/kernel/kernel_module_impl.hpp"
#include "ark/kernel/kernel_plugin.hpp"
#include "ark/kernel/map_module_impl.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(kernel_plugin)

void kernel_plugin::install() {
    ARK_REGISTER_MODULE(class_meta_module, class_meta_module_impl);
    ARK_REGISTER_MODULE(config_module, config_module_impl);
    ARK_REGISTER_MODULE(kernel_module, kernel_module_impl);
    ARK_REGISTER_MODULE(map_module, map_module_impl);
}

void kernel_plugin::uninstall() {
    ARK_UNREGISTER_MODULE(map_module, map_module_impl);
    ARK_UNREGISTER_MODULE(kernel_module, kernel_module_impl);
    ARK_UNREGISTER_MODULE(config_module, config_module_impl);
    ARK_UNREGISTER_MODULE(class_meta_module, class_meta_module_impl);
}

} // namespace ark
