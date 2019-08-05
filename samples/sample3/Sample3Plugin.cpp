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

#include "base/AFPluginManager.hpp"
#include "Sample3Plugin.h"
#include "Sample3Module.h"

namespace ark {

ARK_PLUGIN_DECLARE(Sample3Plugin)

int Sample3Plugin::GetPluginVersion()
{
    return 0;
}

const std::string Sample3Plugin::GetPluginName()
{
    return GET_CLASS_NAME(Sample3Plugin);
}

void Sample3Plugin::Install()
{
    RegisterModule<Sample3Module, Sample3Module>();
}

void Sample3Plugin::Uninstall()
{
    DeregisterModule<Sample3Module, Sample3Module>();
}

} // namespace ark
