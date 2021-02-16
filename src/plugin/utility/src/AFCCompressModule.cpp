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

// #include "base/AFLZ4.hpp"
#include "base/AFDefine.hpp"
#include "base/AFLogger.hpp"
#include "utility/include/AFCCompressModule.hpp"

namespace ark {

bool AFCCompressModule::Init()
{
    zstd_ = std::make_unique<AFZstdCompress>();

    if (!LoadConfig())
    {
        return false;
    }

    // Init zstd dict

    // other

    return true;
}

bool AFCCompressModule::Compress(IN const ArkCompressType type, IN const char* in_data, IN std::size_t in_size, OUT std::string& compressed_buff, IN uint8_t compress_level)
{
    bool result = false;
    switch (type)
    {
    case ArkCompressType::COMPRESS_TYPE_ZLIB:
        result = ZlibCompress(in_data, in_size, compressed_buff, compress_level);
        break;
    case ArkCompressType::COMPRESS_TYPE_ZSTD:
        result = ZStdNormalCompress(in_data, in_size, compressed_buff, compress_level);
        break;
    default:
        ARK_LOG_ERROR("no this compress type = {}", static_cast<int>(type));
        break;
    }

    return result;
}

bool AFCCompressModule::Decompress(IN const ArkCompressType type, IN const char* in_data, IN std::size_t in_size, OUT std::string& decompressed_buff)
{
    bool result = false;
    switch (type)
    {
    case ArkCompressType::COMPRESS_TYPE_ZLIB:
        result = ZlibNormalDecompress(in_data, in_size, decompressed_buff);
        break;
    case ArkCompressType::COMPRESS_TYPE_ZSTD:
        result = ZStdNormalDecompress(in_data, in_size, decompressed_buff);
        break;
    default:
        ARK_LOG_ERROR("no this compress type = {}", static_cast<int>(type));
        break;
    }

    return result;
}

bool AFCCompressModule::ZStdNormalCompress(IN const char* in_data, IN std::size_t in_size,
    OUT std::string& compressed_buff, IN uint8_t compress_level /* = 1*/)
{
    return zstd_->NormalCompress(in_data, in_size, compress_level, compressed_buff);
}

bool AFCCompressModule::ZStdNormalDecompress(
    IN const char* in_data, IN std::size_t in_size, OUT std::string& decompressed_buff)
{
    return zstd_->NormalDecompress(in_data, in_size, decompressed_buff);
}

// bool AFCCompressModule::LZ4Compress(IN const char* in_data, IN std::size_t in_size, OUT std::string& compressed_buff,
//     IN uint8_t compress_level /* = 1*/)
// {
//     return AFLZ4Compress::Compress(in_data, in_size, compress_level, compressed_buff);
// }

// bool AFCCompressModule::LZ4NormalDecompress(
//     IN const char* in_data, IN std::size_t in_size, OUT std::string& decompressed_buff)
// {
//     return AFLZ4Compress::Decompress(in_data, in_size, decompressed_buff);
// }

bool AFCCompressModule::ZlibCompress(IN const char* in_data, IN std::size_t in_size, OUT std::string& compressed_buff,
    IN uint8_t compress_level /* = 1*/)
{
    return AFZlibCompress::Compress(in_data, in_size, compress_level, compressed_buff);
}

bool AFCCompressModule::ZlibNormalDecompress(
    IN const char* in_data, IN std::size_t in_size, OUT std::string& decompressed_buff)
{
    return AFZlibCompress::Decompress(in_data, in_size, decompressed_buff);
}

bool AFCCompressModule::LoadConfig()
{
    // TODO: load zstd dicts and other args
    return true;
}

} // namespace ark
