/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @FileName  AFGUID.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFGUID                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFGUID                                                  *
*****************************************************************************/
#pragma once

#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Base/AFMacros.hpp"

struct AFGUID
{
    union
    {
        struct
        {
            uint64_t n64Value;
        };

        struct
        {
            uint32_t nIdent;
            uint32_t nSerial;
        };
    };

    AFGUID()
    {
        n64Value = 0;
    }

    AFGUID(uint32_t ident, uint32_t serial)
    {
        nIdent = ident;
        nSerial = serial;
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
        if(this->nIdent == rhs.nIdent)
        {
            return this->nSerial < rhs.nSerial;
        }

        return this->nIdent < rhs.nIdent;
    }

    std::string ToString() const
    {
        return ARK_LEXICAL_CAST<std::string>(n64Value);// +"-" + ARK_LEXICAL_CAST<std::string>(nSerial);
    }

    bool FromString(const std::string& strID)
    {
        size_t nStrLength = strID.length();
        size_t nPos = strID.find('-');
        if(nPos == std::string::npos)
        {
            return false;
        }

        std::string strIdent = strID.substr(0, nPos);
        std::string strSerial = "";
        if(nPos + 1 < nStrLength)
        {
            strSerial = strID.substr(nPos + 1, nStrLength - nPos);
        }

        try
        {
            nIdent = ARK_LEXICAL_CAST<int32_t>(strIdent);
            nSerial = ARK_LEXICAL_CAST<int32_t>(strSerial);

            return true;
        }
        catch(...)
        {
            return false;
        }

        return true;
    }
};

//inline bool operator==(const AFGUID& source, const AFGUID& other)
//{
//    return source.n64Value == other.n64Value;
//}
//
//inline bool operator!=(const AFGUID& source, const AFGUID& other)
//{
//    return source.n64Value != other.n64Value;
//}
