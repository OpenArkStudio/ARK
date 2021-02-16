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
#include "base/crypto/AFXXTea.hpp"

using namespace ark;

TEST_CASE("xxtea", "[xxtea]")
{
    const std::string text = "Hello World! 你好，中国！";
    const char* key = "1234567890";
    size_t len;
    std::string encrypted_value;
    auto ret = AFXXTea::xxtea_encrypt(text, key, encrypted_value);
    REQUIRE(ret == true);

    std::string decrypted_value;
    ret = AFXXTea::xxtea_decrypt(encrypted_value, key, decrypted_value);
    REQUIRE(ret == true);
    std::cout << decrypted_value << std::endl;
    REQUIRE(text == decrypted_value);
}