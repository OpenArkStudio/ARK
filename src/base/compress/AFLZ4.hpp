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

#include <lz4.h>
#include "base/AFMacros.hpp"

namespace ark {

#define PRE_ALLOCATED_LZ4_BUFF_SIZE (1024 * 1024)

class AFLZ4Compress
{
public:
    static bool Compress(IN const char* in_data, IN std::size_t in_size, IN int c_level, OUT std::string& compress_buff)
    {
        static char pre_buffer[PRE_ALLOCATED_LZ4_BUFF_SIZE] = {0};
        memset(pre_buffer, 0, sizeof(pre_buffer));

        const int pre_buff_size = LZ4_compressBound(in_size);
        if (pre_buff_size >= PRE_ALLOCATED_LZ4_BUFF_SIZE)
        {
            return false;
        }

        const int compressed_size = LZ4_compress_fast(in_data, pre_buffer, in_size, pre_buff_size, c_level);
        ARK_ASSERT_RET_VAL(compressed_size > 0, false);

        compress_buff.append(pre_buffer, compressed_size);
        return true;
    }

    static bool Decompress(IN const char* in_data, IN std::size_t in_size, OUT std::string& decompress_buff)
    {
        static char pre_buffer[PRE_ALLOCATED_LZ4_BUFF_SIZE] = {0};
        memset(pre_buffer, 0, sizeof(pre_buffer));

        int const decompressed_size = LZ4_decompress_safe(in_data, pre_buffer, in_size, PRE_ALLOCATED_LZ4_BUFF_SIZE);
        ARK_ASSERT_RET_VAL(decompressed_size > 0, false);

        decompress_buff.append(pre_buffer, decompressed_size);
        return true;
    }
};

} // namespace ark