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

#include "redis_sample_module.hpp"
#include "redis_sample_plugin.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(redis_sample_plugin)

void redis_sample_plugin::install() {
    ARK_REGISTER_MODULE(redis_sample_module, redis_sample_module);
}

void redis_sample_plugin::uninstall() {
    ARK_DEREGISTER_MODULE(redis_sample_module, redis_sample_module);
}

} // namespace ark
