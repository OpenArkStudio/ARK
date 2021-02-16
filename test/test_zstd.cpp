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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "base/compress/AFZStd.hpp"
#include "base/AFMisc.hpp"

using namespace ark;

TEST_CASE("zstd_compress", "[zstd_compress]")
{
    AFZstdCompress zstd;

    // normal compress,
    std::string origin_string = AFMisc::random_string(51200);
    // std::string origin_string =
    //     "0123456789abcdefghijklmnopqrstuvwxyzABCDEFHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFHIJKLMN"
    //     "OPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzA"
    //     "BCDEFHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFHIJKLMNOPQRSTUVWXYZ";

    std::string compressed_string;
    std::string decompressed_string;

    auto begin = std::chrono::steady_clock::now();
    REQUIRE(true == zstd.NormalCompress(origin_string.c_str(), origin_string.length(), 1, compressed_string));
    auto end = std::chrono::steady_clock::now();
    auto c_interval = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    begin = std::chrono::steady_clock::now();
    REQUIRE(true == zstd.NormalDecompress(compressed_string.c_str(), compressed_string.length(), decompressed_string));
    end = std::chrono::steady_clock::now();
    auto d_interval = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    REQUIRE(origin_string == decompressed_string);

    std::cout << "zstd testing..." << std::endl;
    std::cout << "origin:\n"
              /*<< AFMisc::ToHexDumpText(origin_string, 32) */
              << "\norigin length:" << origin_string.length()
              << "\ncompressed:\n"
              /*<< AFMisc::ToHexDumpText(compressed_string, 32)*/
              << "\ncompressed length:" << compressed_string.length() << "\ncompressed time:" << c_interval << "us"
              << std::endl;
    std::cout << "compressed:\n"
              /*<< AFMisc::ToHexDumpText(compressed_string, 32)*/
              << "\ncompressed length:"
              << compressed_string.length()
              /*<< "\ndecompressed:" << AFMisc::ToHexDumpText(decompressed_string, 32)*/
              << "\ndecompressed length:" << decompressed_string.length() << "\ndecompressed time:" << d_interval
              << "us" << std::endl;
}