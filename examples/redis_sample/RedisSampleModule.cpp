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

#include "RedisSampleModule.h"

namespace ark {

bool RedisSampleModule::Init()
{
    m_pRedisModule = FindModule<AFIRedisModule>();

    return true;
}

bool RedisSampleModule::PostInit()
{
    auto result = m_pRedisModule->Connect();
    if (!result)
    {
        ARK_LOG_ERROR("connect redis failed");
        return false;
    }

    result = m_pRedisModule->SET("test", "123");
    if (!result)
    {
        ARK_LOG_ERROR("redis SET command failed");
        return false;
    }

    std::string value;
    result = m_pRedisModule->GET("test", value);
    if (!result)
    {
        ARK_LOG_ERROR("redis GET command failed");
        return false;
    }

    ARK_LOG_INFO("redis GET command successfully, value={}", value);

    return true;
}

} // namespace ark
