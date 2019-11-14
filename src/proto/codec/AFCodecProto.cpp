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

#include "AFCodecProto.hpp"

namespace ark {

ark::ARK_CODEC_STATUS AFCodecProto::Encode()
{
    // TODO:
    return ARK_CODEC_STATUS::CODEC_STATUS_OK;
}

ark::ARK_CODEC_STATUS AFCodecProto::Decode()
{
    // TODO:
    return ARK_CODEC_STATUS::CODEC_STATUS_OK;
}

ARK_CODEC_TYPE AFCodecProto::GetCodecType() const
{
    return ARK_CODEC_TYPE::CODEC_PROTO;
}

void AFCodecProto::SetCodecType(ARK_CODEC_TYPE type)
{
    type_ = ARK_CODEC_TYPE::CODEC_PROTO;
}

} // namespace ark
