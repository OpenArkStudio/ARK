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

#pragma once

#include "base/AFPlatform.hpp"

#define XXTEA_DELTA 0x9e3779b9
#define XXTEA_MX (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + (key->data[(p & 3) ^ e] ^ z)))
/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef XXTEA_GET_UINT32_BE
#define XXTEA_GET_UINT32_BE(n, b, i)                                                                                   \
                                                                                                                       \
    {                                                                                                                  \
        (n) = ((uint32_t)(b)[(i)] << 24) | ((uint32_t)(b)[(i) + 1] << 16) | ((uint32_t)(b)[(i) + 2] << 8) |            \
              ((uint32_t)(b)[(i) + 3]);                                                                                \
    }
#endif

namespace ark {

namespace detail {

template<bool CHECK_ENABLE>
class xxtea_check_length;

template<>
class xxtea_check_length<true>
{
public:
    static bool check_protect(size_t len)
    {
        return len > (static_cast<size_t>(std::numeric_limits<uint32_t>::max()) << 2);
    }
};

template<bool CHECK_ENABLE>
class xxtea_check_length
{
public:
    static bool check_protect(size_t)
    {
        return false;
    }
};

template<typename Ty>
class xxtea_check_length_delegate
{
public:
    static const bool value = sizeof(Ty) > sizeof(uint32_t);
};

} // namespace detail

class AFXXTea
{
public:
    class xxtea_key
    {
    public:
        uint32_t data[4]{0};
    };

    static void xxtea_setup(xxtea_key* k, const unsigned char filled[4 * sizeof(uint32_t)])
    {
        memset(k->data, 0, sizeof(k->data));

        for (int i = 0; i < 4; i++)
        {
            XXTEA_GET_UINT32_BE(k->data[i], filled, i << 2);
        }
    }

    /**
     * @brief encrypt data use xxtea
     * @param key           xxtea key, should be initialized by xxtea_setup
     * @param buffer        buffer address, must padding to uint32_t
     * @param len           buffer size, must padding to uint32_t, can not be greater than 2^34
     */
    static void xxtea_encrypt(const xxtea_key* key, void* buffer, size_t len)
    {
        if (len & 0x03)
        {
            abort();
        }

        if (detail::xxtea_check_length<detail::xxtea_check_length_delegate<size_t>::value>::check_protect(len))
        {
            abort();
        }

        if (NULL == key || NULL == buffer || 0 == len)
        {
            return;
        }

        uint32_t* v = reinterpret_cast<uint32_t*>(buffer);
        uint32_t n = static_cast<uint32_t>(len >> 2);

        uint32_t y, z, sum;
        uint32_t p, rounds, e;

        rounds = 6 + 52 / n;
        sum = 0;
        z = v[n - 1];
        do
        {
            sum += XXTEA_DELTA;
            e = (sum >> 2) & 3;
            for (p = 0; p < n - 1; p++)
            {
                y = v[p + 1];
                z = v[p] += XXTEA_MX;
            }
            y = v[0];
            z = v[n - 1] += XXTEA_MX;
        } while (--rounds);
    }

    /**
     * @brief encrypt data use xxtea
     * @param key           xxtea key, should be initialized by xxtea_setup
     * @param input         buffer holding the input data
     * @param ilen          length of the input data
     * @param output        buffer for the output data. Should be able to hold at
     *                      least ((ilen - 1) | 0x03) + 1. Cannot be the same buffer as
     *                      input!
     * @param olen          length of the output data, will be filled with the
     *                      actual number of bytes written.
     * @note if passed invalid parameter, olen will be set to 0
     */
    static void xxtea_encrypt(const xxtea_key* key, const void* input, size_t ilen, void* output, size_t* olen)
    {
        bool is_success = false;
        do
        {
            if (NULL == key || input == NULL || ilen <= 0 || output == NULL || NULL == olen)
            {
                break;
            }

            size_t real_olen = ((ilen - 1) | 0x03) + 1;
            if (*olen < real_olen)
            {
                break;
            }

            if (input != output)
            {
                memcpy(output, input, ilen);
            }

            if (real_olen > ilen)
            {
                memset(reinterpret_cast<char*>(output) + ilen, 0, real_olen - ilen);
            }

            *olen = real_olen;
            xxtea_encrypt(key, output, *olen);

            is_success = true;
        } while (false);

        if (!is_success && NULL != olen)
        {
            *olen = 0;
        }
    }

    /**
     * @brief decrypt data use xxtea
     * @param key           xxtea key, should be initialized by xxtea_setup
     * @param buffer        buffer address, must padding to uint32_t
     * @param len           buffer size, must padding to uint32_t, can not be greater than 2^34
     */
    static void xxtea_decrypt(const xxtea_key* key, void* buffer, size_t len)
    {
        if (len & 0x03)
        {
            abort();
        }

        if (detail::xxtea_check_length<detail::xxtea_check_length_delegate<size_t>::value>::check_protect(len))
        {
            abort();
        }

        if (NULL == key || NULL == buffer || 0 == len)
        {
            return;
        }

        uint32_t* v = reinterpret_cast<uint32_t*>(buffer);
        uint32_t n = static_cast<uint32_t>(len >> 2);

        uint32_t y, z, sum;
        uint32_t p, rounds, e;

        rounds = 6 + 52 / n;
        sum = rounds * XXTEA_DELTA;
        y = v[0];
        do
        {
            e = (sum >> 2) & 3;
            for (p = n - 1; p > 0; p--)
            {
                z = v[p - 1];
                y = v[p] -= XXTEA_MX;
            }
            z = v[n - 1];
            y = v[0] -= XXTEA_MX;
            sum -= XXTEA_DELTA;
        } while (--rounds);
    }

    /**
     * @brief decrypt data use xxtea
     * @param key           xxtea key, should be initialized by xxtea_setup
     * @param input         buffer holding the input data
     * @param ilen          length of the input data
     * @param output        buffer for the output data. Should be able to hold at
     *                      least ((ilen - 1) | 0x03) + 1. Cannot be the same buffer as
     *                      input!
     * @param olen          length of the output data, will be filled with the
     *                      actual number of bytes written.
     * @note if passed invalid parameter, olen will be set to 0
     */
    static void xxtea_decrypt(const xxtea_key* key, const void* input, size_t ilen, void* output, size_t* olen)
    {
        bool is_success = false;
        do
        {
            if (NULL == key || input == NULL || ilen <= 0 || output == NULL || NULL == olen)
            {
                break;
            }

            size_t real_olen = ((ilen - 1) | 0x03) + 1;
            if (*olen < real_olen)
            {
                break;
            }

            if (input != output)
            {
                memcpy(output, input, ilen);
            }

            if (real_olen > ilen)
            {
                memset(reinterpret_cast<char*>(output) + ilen, 0, real_olen - ilen);
            }

            *olen = real_olen;
            xxtea_decrypt(key, output, *olen);

            is_success = true;
        } while (false);

        if (!is_success && NULL != olen)
        {
            *olen = 0;
        }
    }
};

} // namespace ark
