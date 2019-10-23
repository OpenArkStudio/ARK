/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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
#include "base/AFBase64.hpp"

using namespace ark;

static const unsigned char base64_test_dec[64] = {0x24, 0x48, 0x6E, 0x56, 0x87, 0x62, 0x5A, 0xBD, 0xBF, 0x17, 0xD9,
    0xA2, 0xC4, 0x17, 0x1A, 0x01, 0x94, 0xED, 0x8F, 0x1E, 0x11, 0xB3, 0xD7, 0x09, 0x0C, 0xB6, 0xE9, 0x10, 0x6F, 0x22,
    0xEE, 0x13, 0xCA, 0xB3, 0x07, 0x05, 0x76, 0xC9, 0xFA, 0x31, 0x6C, 0x08, 0x34, 0xFF, 0x8D, 0xC2, 0x6C, 0x38, 0x00,
    0x43, 0xE9, 0x54, 0x97, 0xAF, 0x50, 0x4B, 0xD1, 0x41, 0xBA, 0x95, 0x31, 0x5A, 0x0B, 0x97};

static const unsigned char base64_test_enc_standard[] = "JEhuVodiWr2/F9mixBcaAZTtjx4Rs9cJDLbpEG8i7hPK"
                                                        "swcFdsn6MWwINP+Nwmw4AEPpVJevUEvRQbqVMVoLlw==";

static const unsigned char base64_test_enc_utf7[] = "JEhuVodiWr2/F9mixBcaAZTtjx4Rs9cJDLbpEG8i7hPK"
                                                    "swcFdsn6MWwINP+Nwmw4AEPpVJevUEvRQbqVMVoLlw";

static const unsigned char base64_test_enc_imap[] = "JEhuVodiWr2,F9mixBcaAZTtjx4Rs9cJDLbpEG8i7hPK"
                                                    "swcFdsn6MWwINP+Nwmw4AEPpVJevUEvRQbqVMVoLlw";

static const unsigned char base64_test_enc_url[] = "JEhuVodiWr2_F9mixBcaAZTtjx4Rs9cJDLbpEG8i7hPK"
                                                   "swcFdsn6MWwINP-Nwmw4AEPpVJevUEvRQbqVMVoLlw==";

TEST_CASE("encode_standard", "[encode_standard]")
{
    unsigned char buffer[128] = {0};
    size_t len = 0;
    REQUIRE(-1 == AFBase64::base64_encode(NULL, 0, &len, base64_test_dec, 64));
    REQUIRE(len == 89); // \0 for tail

    REQUIRE(0 == AFBase64::base64_encode(buffer, sizeof(buffer), &len, base64_test_dec, 64));
    REQUIRE(len == 88);
    REQUIRE(0 == memcmp(base64_test_enc_standard, buffer, 88));

    std::string std_str_in;
    std::string std_str_out;
    std_str_in.assign(
        reinterpret_cast<const char*>(&base64_test_dec[0]), reinterpret_cast<const char*>(&base64_test_dec[0]) + 64);

    REQUIRE(0 == AFBase64::base64_encode(std_str_out, base64_test_dec, 64));
    REQUIRE(std_str_out.size() == 88);
    REQUIRE(0 == memcmp(base64_test_enc_standard, std_str_out.c_str(), 88));

    std_str_out.clear();

    REQUIRE(0 == AFBase64::base64_encode(std_str_out, std_str_in));
    REQUIRE(std_str_out.size() == 88);
    REQUIRE(0 == memcmp(base64_test_enc_standard, std_str_out.c_str(), 88));
}

TEST_CASE("decode_standard", "[decode_standard]")
{
    unsigned char buffer[128] = {0};
    size_t len = 0;
    REQUIRE(-1 == AFBase64::base64_decode(NULL, 0, &len, base64_test_enc_standard, 88));
    REQUIRE(len == 64);

    REQUIRE(0 == AFBase64::base64_decode(buffer, sizeof(buffer), &len, base64_test_enc_standard, 88));
    REQUIRE(len == 64);
    REQUIRE(0 == memcmp(base64_test_dec, buffer, 64));

    std::string std_str_in;
    std::string std_str_out;
    std_str_in.assign(reinterpret_cast<const char*>(&base64_test_enc_standard[0]),
        reinterpret_cast<const char*>(&base64_test_enc_standard[0]) + 88);

    REQUIRE(0 == AFBase64::base64_decode(std_str_out, base64_test_enc_standard, 88));
    REQUIRE(std_str_out.size() == 64);
    REQUIRE(0 == memcmp(base64_test_dec, std_str_out.c_str(), 64));

    std_str_out.clear();

    REQUIRE(0 == AFBase64::base64_decode(std_str_out, std_str_in));
    REQUIRE(std_str_out.size() == 64);
    REQUIRE(0 == memcmp(base64_test_dec, std_str_out.c_str(), 64));
}

TEST_CASE("decode_no_padding", "[decode_no_padding]")
{
    std::string std_str_out;

    AFBase64::base64_decode(std_str_out, "YW55IGNhcm5hbCBwbGVhcw");
    REQUIRE(0 == memcmp("any carnal pleas", std_str_out.c_str(), 16));
    REQUIRE(16 == std_str_out.size());

    AFBase64::base64_decode(std_str_out, "YW55IGNhcm5hbCBwbGVhc3U");
    REQUIRE(0 == memcmp("any carnal pleasu", std_str_out.c_str(), 17));
    REQUIRE(17 == std_str_out.size());

    AFBase64::base64_decode(std_str_out, "YW55IGNhcm5hbCBwbGVhc3Vy");
    REQUIRE(0 == memcmp("any carnal pleasur", std_str_out.c_str(), 18));
    REQUIRE(18 == std_str_out.size());
}

TEST_CASE("encode_no_padding", "[encode_no_padding]")
{
    std::string std_str_out;

    AFBase64::base64_encode(std_str_out, "any carnal pleas", AFBase64::base64_mode_t::EN_BMT_UTF7);
    REQUIRE(0 == memcmp("YW55IGNhcm5hbCBwbGVhcw", std_str_out.c_str(), 22));
    REQUIRE(22 == std_str_out.size());

    AFBase64::base64_encode(std_str_out, "any carnal pleasu", AFBase64::base64_mode_t::EN_BMT_UTF7);
    REQUIRE(0 == memcmp("YW55IGNhcm5hbCBwbGVhc3U", std_str_out.c_str(), 23));
    REQUIRE(23 == std_str_out.size());

    AFBase64::base64_encode(std_str_out, "any carnal pleasur", AFBase64::base64_mode_t::EN_BMT_UTF7);
    REQUIRE(0 == memcmp("YW55IGNhcm5hbCBwbGVhc3Vy", std_str_out.c_str(), 24));
    REQUIRE(24 == std_str_out.size());
}

TEST_CASE("encode_utf7", "[encode_utf7]")
{
    unsigned char buffer[128] = {0};
    size_t len = 0;
    REQUIRE(-1 == AFBase64::base64_encode(NULL, 0, &len, base64_test_dec, 64, AFBase64::base64_mode_t::EN_BMT_UTF7));
    REQUIRE(len == 87); // \0 for tail

    REQUIRE(0 == AFBase64::base64_encode(
                     buffer, sizeof(buffer), &len, base64_test_dec, 64, AFBase64::base64_mode_t::EN_BMT_UTF7));
    REQUIRE(len == 86);
    REQUIRE(0 == memcmp(base64_test_enc_utf7, buffer, 86));
}

TEST_CASE("decode_utf7", "[decode_utf7]")
{
    unsigned char buffer[128] = {0};
    size_t len = 0;
    REQUIRE(
        -1 == AFBase64::base64_decode(NULL, 0, &len, base64_test_enc_utf7, 86, AFBase64::base64_mode_t::EN_BMT_UTF7));
    REQUIRE(len == 64);

    REQUIRE(0 == AFBase64::base64_decode(
                     buffer, sizeof(buffer), &len, base64_test_enc_utf7, 86, AFBase64::base64_mode_t::EN_BMT_UTF7));
    REQUIRE(len == 64);
    REQUIRE(0 == memcmp(base64_test_dec, buffer, 64));
}

TEST_CASE("encode_imap", "[encode_imap]")
{
    unsigned char buffer[128] = {0};
    size_t len = 0;
    REQUIRE(-1 == AFBase64::base64_encode(
                      NULL, 0, &len, base64_test_dec, 64, AFBase64::base64_mode_t::EN_BMT_IMAP_MAILBOX_NAME));
    REQUIRE(len == 87); // \0 for tail

    REQUIRE(0 == AFBase64::base64_encode(buffer, sizeof(buffer), &len, base64_test_dec, 64,
                     AFBase64::base64_mode_t::EN_BMT_IMAP_MAILBOX_NAME));
    REQUIRE(len == 86);
    REQUIRE(0 == memcmp(base64_test_enc_imap, buffer, 86));
}

TEST_CASE("decode_imap", "[decode_imap]")
{
    unsigned char buffer[128] = {0};
    size_t len = 0;
    REQUIRE(-1 == AFBase64::base64_decode(
                      NULL, 0, &len, base64_test_enc_imap, 86, AFBase64::base64_mode_t::EN_BMT_IMAP_MAILBOX_NAME));
    REQUIRE(len == 64);

    REQUIRE(0 == AFBase64::base64_decode(buffer, sizeof(buffer), &len, base64_test_enc_imap, 86,
                     AFBase64::base64_mode_t::EN_BMT_IMAP_MAILBOX_NAME));
    REQUIRE(len == 64);
    REQUIRE(0 == memcmp(base64_test_dec, buffer, 64));
}

TEST_CASE("encode_url", "[encode_url]")
{
    unsigned char buffer[128] = {0};
    size_t len = 0;
    REQUIRE(-1 == AFBase64::base64_encode(
                      NULL, 0, &len, base64_test_dec, 64, AFBase64::base64_mode_t::EN_BMT_URL_FILENAME_SAFE));
    REQUIRE(len == 89); // \0 for tail

    REQUIRE(0 == AFBase64 ::base64_encode(buffer, sizeof(buffer), &len, base64_test_dec, 64,
                     AFBase64::base64_mode_t::EN_BMT_URL_FILENAME_SAFE));
    REQUIRE(len == 88);
    REQUIRE(0 == memcmp(base64_test_enc_url, buffer, 88));
}

TEST_CASE("decode_url", "[decode_url]")
{
    unsigned char buffer[128] = {0};
    size_t len = 0;
    REQUIRE(-1 == AFBase64::base64_decode(
                      NULL, 0, &len, base64_test_enc_url, 88, AFBase64::base64_mode_t::EN_BMT_URL_FILENAME_SAFE));
    REQUIRE(len == 64);

    REQUIRE(0 == AFBase64::base64_decode(buffer, sizeof(buffer), &len, base64_test_enc_url, 88,
                     AFBase64::base64_mode_t::EN_BMT_URL_FILENAME_SAFE));
    REQUIRE(len == 64);
    REQUIRE(0 == memcmp(base64_test_dec, buffer, 64));
}
