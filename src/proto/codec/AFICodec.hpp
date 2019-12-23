/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFPlatform.hpp"

namespace ark {

/**
 * message codec types
 */
enum class ARK_CODEC_TYPE : uint8_t
{
    CODEC_UNKNOWN = 0,         /// unknown codec
    CODEC_PROTO = 1,           /// protobuffer codes
    CODEC_JSON = 2,            /// json codec
    CODEC_CUSTOM = 3,          /// custom codec
    CODEC_WS_EXTEND_PROTO = 4, /// websocket extension protobuffer data codec
    CODES_WS_EXTEND_JSON = 5,  /// websocket extension json data codec
};

/**
 * codec status
 */

enum class ARK_CODEC_STATUS : uint8_t
{
    CODEC_STATUS_OK = 1,      /// OK
    CODEC_STATUS_INVALID = 2, /// different codec type
    CODEC_STATUS_ERROR = 3,   /// codec error
};

class AFICodec
{
public:
    // AFICodec(ARK_CODEC_TYPE type)
    // {
    //     SetCodecType(type);
    // }

    virtual ~AFICodec() = default;

    virtual ARK_CODEC_STATUS Encode() = 0;
    virtual ARK_CODEC_STATUS Decode() = 0;

    virtual ARK_CODEC_TYPE GetCodecType() const = 0;
    virtual void SetCodecType(ARK_CODEC_TYPE type) = 0;
};

} // namespace ark
