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

#pragma once

#include "base/AFPlatform.hpp"
#include "base/AFMacros.hpp"
#include "net/include/AFNetDefine.hpp"

#pragma pack(push, 1)

namespace ark {

class AFNetEvent final
{
    AFNetEventType type_{AFNetEventType::NONE};
    conv_id_t id_{0};
    std::string ip_{};
    bus_id_t bus_id_{0};

public:
    static AFNetEvent* AllocEvent()
    {
        return ARK_NEW AFNetEvent();
    }

    static void Release(AFNetEvent*& event)
    {
        ARK_DELETE(event);
    }

    AFNetEventType GetType() const
    {
        return type_;
    }

    conv_id_t GetId() const
    {
        return id_;
    }

    const std::string& GetIP() const
    {
        return ip_;
    }

    bus_id_t GetBusId() const
    {
        return bus_id_;
    }

    void SetType(AFNetEventType value)
    {
        type_ = value;
    }

    void SetId(conv_id_t value)
    {
        id_ = value;
    }

    void SetIP(const std::string& value)
    {
        ip_ = value;
    }

    void SetBusId(bus_id_t value)
    {
        bus_id_ = value;
    }
};

} // namespace ark

#pragma pack(pop)
