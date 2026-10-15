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

#include "codec_proto.hpp"

namespace ark {

ark::codec_status codec_proto::Encode() {
    // TODO:
    return codec_status::CODEC_STATUS_OK;
}

ark::codec_status codec_proto::Decode() {
    // TODO:
    return codec_status::CODEC_STATUS_OK;
}

codec_type codec_proto::GetCodecType() const {
    return codec_type::CODEC_PROTO;
}

void codec_proto::SetCodecType(codec_type type) {
    type_ = codec_type::CODEC_PROTO;
}

} // namespace ark
