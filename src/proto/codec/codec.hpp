/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/platform.hpp"

namespace ark {

/**
 * message codec types
 */
enum class codec_type : uint8_t
{
    unknown = 0,            /// unknown codec
    protobuf = 1,           /// protobuffer codes
    json = 2,               /// json codec
    custom = 3,             /// custom codec
    websocket_protobuf = 4, /// websocket extension protobuffer data codec
    websocket_json = 5,     /// websocket extension json data codec
};

/**
 * codec status
 */

enum class codec_status : uint8_t
{
    ok = 1,      /// OK
    invalid = 2, /// different codec type
    error = 3,   /// codec error
};

class codec
{
public:
    // AFICodec(ARK_CODEC_TYPE type)
    // {
    //     set_codec_type(type);
    // }

    virtual ~codec() = default;

    virtual codec_status encode() = 0;
    virtual codec_status decode() = 0;

    virtual codec_type get_codec_type() const = 0;
    virtual void set_codec_type(codec_type type) = 0;
};

} // namespace ark
