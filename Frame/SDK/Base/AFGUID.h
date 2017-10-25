/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Base/AFMacros.hpp"

struct AFGUID
{
	uint64_t n64Value;

    AFGUID()
    {
        n64Value = 0;
    }


    AFGUID(uint64_t value)
    {
        n64Value = value;
    }

    bool IsNull()
    {
        return (0 == n64Value);
    }

    bool IsNull() const
    {
        return (0 == n64Value);
    }

    AFGUID& operator=(const uint64_t value)
    {
        n64Value = value;
        return *this;
    }

    bool operator==(const AFGUID& rhs) const
    {
        return (this->n64Value == rhs.n64Value);
    }

    bool operator!=(const AFGUID& rhs) const
    {
        return (this->n64Value != rhs.n64Value);
    }

    bool operator<(const AFGUID& rhs) const
    {
        return this->n64Value < rhs.n64Value;
    }

    std::string ToString() const
    {
        return ARK_LEXICAL_CAST<std::string>(n64Value);
    }

    bool FromString(const std::string& strID)
    {
		return Ark_from_str(strID, n64Value);
    }
};

struct AFGUID128
{
	uint64_t high;	//area_id * 10000000 + app_id
	uint64_t low;	//10 bits(reserve) + 41 bits(timestamp) + 12 bits(sequence_id)

	//TODO:
};