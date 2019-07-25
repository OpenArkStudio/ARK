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

#include "base/AFDefine.hpp"
#include "base/AFDateTime.hpp"
#include "base/AFBaseStruct.hpp"
#include "interface/AFIPluginManager.h"
#include "utility/include/AFCGUIDModule.h"

namespace ark {

bool AFCGUIDModule::Init()
{
#ifdef AF_THREAD_SAFE
    uid_generator_ = ARK_NEW AFUidGeneratorThreadSafe();
#else
    uid_generator_ = ARK_NEW AFUidGenerator();
#endif // AF_THREAD_SAFE

    return true;
}

bool AFCGUIDModule::PreShut()
{
    ARK_DELETE(uid_generator_);
    return true;
}

AFGUID AFCGUIDModule::CreateGUID()
{
    ARK_ASSERT_RET_VAL(uid_generator_ != nullptr, NULL_GUID);

    AFBusAddr bus_addr(pPluginManager->BusID());
    int64_t id = int64_t(bus_addr.zone_id) << (2 * 8) | int64_t(bus_addr.app_type) << (1 * 8) |
                 int64_t(bus_addr.inst_id) << (0 * 8);
    return uid_generator_->GetUID(id);
}

std::string AFCGUIDModule::ParseUID(const AFGUID& id)
{
    return uid_generator_->ParseUID(id);
}

} // namespace ark
