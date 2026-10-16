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

#include "ark/compress/zstd.hpp"
//#include "ark/compress/lz4.hpp"
#include "ark/compress/zlib.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/utility/compress_module.hpp"

namespace ark {

class compress_module_impl final : public compress_module
{
    ARK_DECLARE_MODULE_FUNCTIONS

private:
    std::unique_ptr<zstd_compress> zstd_{nullptr};

public:
    bool init() override;

    bool compress(IN const compress_type type, IN const char* in_data, IN std::size_t in_size,
        OUT std::string& compressed_buff, IN uint8_t compress_level = 1) override;

    bool decompress(IN const compress_type type, IN const char* in_data, IN std::size_t in_size,
        OUT std::string& decompressed_buff) override;

protected:
    bool load_config();

    bool z_std_normal_compress(IN const char* in_data, IN std::size_t in_size, OUT std::string& compressed_buff,
        IN uint8_t compress_level = 1);

    bool z_std_normal_decompress(IN const char* in_data, IN std::size_t in_size, OUT std::string& decompressed_buff);

    // bool LZ4Compress(IN const char* in_data, IN std::size_t in_size, OUT std::string& compressed_buff,
    //     IN uint8_t compress_level = 1) override;
    // bool LZ4NormalDecompress(
    //     IN const char* in_data, IN std::size_t in_size, OUT std::string& decompressed_buff) override;

    bool zlib_compress(IN const char* in_data, IN std::size_t in_size, OUT std::string& compressed_buff,
        IN uint8_t compress_level = 1);
    bool zlib_normal_decompress(IN const char* in_data, IN std::size_t in_size, OUT std::string& decompressed_buff);
};

} // namespace ark
