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

#include "protobuf_codec.hpp"

namespace ark {

ark::codec_status protobuf_codec::encode() {
    // TODO:
    return codec_status::ok;
}

ark::codec_status protobuf_codec::decode() {
    // TODO:
    return codec_status::ok;
}

codec_type protobuf_codec::get_codec_type() const {
    return codec_type::protobuf;
}

void protobuf_codec::set_codec_type(codec_type type) {
    type_ = codec_type::protobuf;
}

} // namespace ark
