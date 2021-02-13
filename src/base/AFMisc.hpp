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

#include "base/AFMacros.hpp"
#include "base/AFEnum.hpp"
#include "base/AFBus.hpp"
#include "base/time/AFDateTime.hpp"
#include "base/AFStringUtils.hpp"

namespace ark {

class AFMisc
{
public:
    static uint32_t GetNearest2N(uint32_t size)
    {
        if (size == 0)
        {
            return 0;
        }

        if (size <= 8)
        {
            return 8;
        }

        if (size > (1 << 16))
        {
            return 0;
        }

        if ((size & (size - 1)) == 0)
        {
            return size;
        }

        int count = 0;

        while (size)
        {
            size = size >> 1;
            ++count;
        }

        return 1 << count;
    }

    template<typename T>
    static std::string ToString(const T& value)
    {
        return ARK_LEXICAL_CAST<std::string>(value);
    }

    template<typename T>
    static bool FromString(const std::string& str_value, T& result)
    {
        try
        {
            result = ARK_LEXICAL_CAST<T>(str_value);
            return true;
        }
        catch (std::system_error& ex)
        {
            ARK_ASSERT_NO_EFFECT(0);
            return false;
        }

        return false;
    }

    template<typename T>
    static T FromString(const std::string& str_value)
    {
        try
        {
            T result = ARK_LEXICAL_CAST<T>(str_value);
            return result;
        }
        catch ([[maybe_unused]] std::system_error& ex)
        {
            ARK_ASSERT_NO_EFFECT(0);
            return T();
        }
    }

    static std::string Bus2Str(bus_id_t bus_id)
    {
        AFBusAddr bus_addr(bus_id);
        return bus_addr.ToString();
    }

    static ArkDataType CovertDataType(const std::string& type_name)
    {
        static char const* data_type[] = {
            "bool", "int", "uint", "int64", "uint64", "float", "double", "string", "vector3d", "struct", "table",
            "array",
            // If there are more types, please add type name in here.
        };

        static int data_type_len = ARRAY_LENTGH(data_type);
        for (int i = 0; i < data_type_len; ++i)
        {
            if (type_name == data_type[i])
            {
                return ArkDataType(i);
            }
        }

        return ArkDataType::DT_EMPTY;
    }

    template<typename T>
    static T Operate(ArkDataOpType op, const T& base, const T& delta)
    {
        switch (op)
        {
        case ArkDataOpType::DOT_INC:
            base += delta;
            break;
        case ArkDataOpType::DOT_DEC:
            base -= std::min(base, delta);
            break;
        case ArkDataOpType::DOT_MUL:
            base *= delta;
            break;
        case ArkDataOpType::DOT_DIV:
            ARK_ASSERT_RET_VAL(delta != 0, base);
            base /= delta;
            break;
        case ArkDataOpType::DOT_SET:
            base = delta;
            break;
        case ArkDataOpType::DOT_AND:
            base &= delta;
            break;
        case ArkDataOpType::DOT_OR:
            base |= delta;
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }

        return base;
    }

    /**
     * StrToData
     */
    static long pow_i(int d, int n)
    {
        int i;
        long m = 1;
        for (i = 0; i < n; i++)
        {
            m *= d;
        }

        return m;
    }

    static bool isRightData(char c, int jz)
    {
        if ((jz < 2) || (jz > 16))
        {
            return false;
        }
        char ch[] = "00112233445566778899aAbBcCdDeEfF";
        int i = 0;
        for (i = 0; i < jz; i++)
        {
            if (c == ch[i * 2])
            {
                return true;
            }
            if (c == ch[i * 2 + 1])
            {
                return true;
            }
        }
        return false;
    }

    static long StrToData(const std::string& strData, int jz)
    {
        int l = (int)strData.length();
        long IntValue = 0;
        int bitValue = 0;
        int i;
        for (i = 0; i < l; i++)
        {
            if (isRightData(strData[i], jz))
            {
                bitValue = ToHexDigit(strData[i]);
                IntValue = bitValue * pow_i(jz, l - i - 1) + IntValue;
            }
            else
            {
                return -1;
            }
        }
        return IntValue;
    }

    static int ToHexDigit(char c)
    {
        assert(isRightData(c, 16));

        int bitValue = 0;
        if (isdigit(c))
        {
            bitValue = c - 0x30;
        }
        else
        {
            switch (c)
            {
            case 'a':
            case 'A':
                bitValue = 0x0a;
                break;

            case 'b':
            case 'B':
                bitValue = 0x0b;
                break;

            case 'c':
            case 'C':
                bitValue = 0x0c;
                break;

            case 'd':
            case 'D':
                bitValue = 0x0d;
                break;

            case 'e':
            case 'E':
                bitValue = 0x0e;
                break;

            case 'f':
            case 'F':
                bitValue = 0x0f;
                break;
            default:
                break;
            }
        }
        return bitValue;
    }

    /**
     * ConvertToCStyleStr
     * 
     * 00 01 02 03 04 05 06 07  ........
     * 08 09 0a 0b 0c 0d 0e 0f  ........
     * 10 11 12 13 14 15 16 17  ........
     * 18 19 1a 1b 1c 1d 1e 1f  ........
     * 20 21 22 23 24 25 26 27   !"#$%&'
     * 28 29 2a 2b 2c 2d 2e 2f  ()*+,-./
     * 30 31 32 33 34 35 36 37  01234567
     * 38 39 3a 3b 3c 3d 3e 3f  89:;<=>?
     * 40 41 42 43 44 45 46 47  @ABCDEFG
     * 48 49 4a 4b 4c 4d 4e 4f  HIJKLMNO
     * 50 51 52 53 54 55 56 57  PQRSTUVW
     * 58 59 5a 5b 5c 5d 5e 5f  XYZ[\]^_
     * 60 61 62 63 64 65 66 67  `abcdefg
     * 68 69 6a 6b 6c 6d 6e 6f  hijklmno
     * 70 71 72 73 74 75 76 77  pqrstuvw
     * 78 79 7a 7b 7c 7d 7e 7f  xyz{|}~.
     */

    static std::string ConvertToCStyleStr(const std::string& _Str)
    {
        static const char* shiftMap[] = {"\\(00)", "\\(01)", "\\(02)", "\\(03)", "\\(04)", "\\(05)", "\\(06)", "\\(07)",
            "\\(08)", "\\t", "\\n", "\\(0B)", "\\(0C)", "\\r", "\\(0E)", "\\(0F)", "\\(10)", "\\(11)", "\\(12)",
            "\\(13)", "\\(14)", "\\(15)", "\\(16)", "\\(17)", "\\(18)", "\\(19)", "\\(1A)", "\\(1B)", "\\(1C)",
            "\\(1D)", "\\(1E)", "\\(1F)", " ", "!", "\"", "#", "$", "%", "&", "\'", "(", ")", "*", "+", ",", "-", ".",
            "/", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?", "@", "A", "B", "C",
            "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
            "Y", "Z", "[", "\\\\", "]", "^", "_", "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
            "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "\\(7F)"};

        std::string rstString;
        // Pre allocated memory
        rstString.reserve(_Str.size() * 2);

        for (size_t i = 0; i < _Str.size(); i++)
        {
            //bool isAscIIChar = int8_t(_Str[i]) >= 0x0 && int8_t(_Str[i]) <= 0x7f;
            bool isAscIIChar = (static_cast<unsigned char>(_Str[i]) <= 127);
            if (isAscIIChar)
            {
                // english characters
                rstString.append(shiftMap[size_t(_Str[i])]);
            }
            else
            {
                // other characters
                rstString.append(1, _Str[i]);
            }
        }
        return rstString;
    }

    static std::string hexdump_oneline(const std::string& prefix, const std::string& line, size_t width)
    {
        std::ostringstream ostrm;
        ostrm << prefix;

        // hex
        for (size_t i = 0; i < line.size(); i++)
        {
            char hex_text[16] = "";
            snprintf(hex_text, sizeof(hex_text), "%02X ", (unsigned char)(line[i]));
            ostrm << hex_text;
            if (i == 7 || i == 15 || i == 23)
            {
                ostrm << ' ';
            }
        }

        // the space between hex and text
        for (unsigned long spacer = line.size(); spacer < width; spacer++)
        {
            ostrm << "   ";
        }
        if (line.size() <= 8)
        {
            ostrm << "   ";
        }
        else if (line.size() <= 16)
        {
            ostrm << "  ";
        }
        else if (line.size() <= 24)
        {
            ostrm << " ";
        }
        ostrm << ": ";

        // text
        for (size_t i = 0; i < line.size(); i++)
        {
            if (line[i] < 32)
                ostrm << '.';
            else
                ostrm << char(line[i]);
        }

        ostrm << std::endl;

        return ostrm.str();
    }

    static std::string ToHexDumpText(const std::string& _Str, size_t width, const std::string& prefix = "| ")
    {
        std::ostringstream ostrm;

        std::string str = _Str;
        while (str.size() > width)
        {
            std::string line(str.c_str(), width);
            ostrm << hexdump_oneline(prefix, line, width);
            str = str.substr(width);
        }

        if (str.size() > 0)
        {
            ostrm << hexdump_oneline(prefix, str, width);
        }

        return ostrm.str();
    }

    /**
     * ConvertFromCStyleStr
     */
    static bool IsHexNumber(const char& c)
    {
        const static char hexNumber[] = "0123456789abcdefABCDEF";
        return (strchr(hexNumber, c) != NULL);
    }

    // pSrc is the string that format is "\(3F)".
    static int ProcShiftCharByNum(char& dst, const char* const pSrc, int srcStrLen)
    {
        assert(pSrc != NULL);

        // check if it's the standard format : "\(3F)"
        bool isAllowdFormat = false;
        if (srcStrLen < 5)
        {
            isAllowdFormat = false;
        }
        else if (pSrc[0] != '\\' || pSrc[1] != '(' || pSrc[4] != ')')
        {
            isAllowdFormat = false;
        }
        else if ((!IsHexNumber(pSrc[2])) || (!IsHexNumber(pSrc[3])))
        {
            isAllowdFormat = false;
        }
        else
        {
            isAllowdFormat = true;
        }

        if (isAllowdFormat == false)
        {
            // illegal format.
            //assert(false); // tag: do not assert

            //For unrecognized escapes, the '\' symbol is parsed as a non escape indicator and
            // the cursor points to the next character of the '\' symbol
            dst = pSrc[0];
            return 1;
        }

        // Right situation
        std::string hexNumString(&pSrc[2], 2);
        int hexNum = StrToData(hexNumString, 16);
        dst = (static_cast<unsigned char>(hexNum));
        return 5; // standard format:"\(3F)" is 5 bytes.
    }

    // pSrc is "\r" "\n" "\t" "\\"
    static int ProcShiftCharByChar(char& dst, const char* const pSrc, int srcStrLen)
    {
        // Check format.
        if (srcStrLen < 2)
        {
            // illegal format.
            //assert(false); // tag: do not assert

            // For unrecognized escapes, the '\' symbol is parsed as a non escape indicator and
            // the cursor points to the next character of the '\' symbol
            dst = pSrc[0];
            return 1;
        }

        switch (pSrc[1])
        {
        case 'r':
            dst = '\r';
            return 2;
        case 'n':
            dst = '\n';
            return 2;
        case 't':
            dst = '	';
            return 2;
        case '\\':
            dst = '\\';
            return 2;
        default:
            // illegal format.
            //assert(false); // tag: do not assert

            //For unrecognized escapes, the '\' symbol is parsed as a non escape indicator and
            // the cursor points to the next character of the '\' symbol
            dst = pSrc[0];
            return 1;
        }
    }

    // return the pos that the iter moves.
    static int ProcShiftChar(char& dst, const char* pSrc, int srcStrLen)
    {
        switch (pSrc[1])
        {
        case 'r':
        case 'n':
        case 't':
        case '\\':
            return ProcShiftCharByChar(dst, pSrc, srcStrLen);
        case '(':
            return ProcShiftCharByNum(dst, pSrc, srcStrLen);
        default:
            // illegal format.
            //assert(false); // tag: do not assert

            //For unrecognized escapes, the '\' symbol is parsed as a non escape indicator and
            // the cursor points to the next character of the '\' symbol
            dst = pSrc[0];
            return 1;
        }
    }

    static std::string ConvertFromCStyleStr(const std::string& _CStyleStr)
    {
        std::string rstString;
        // Pre allocated memory
        rstString.reserve(_CStyleStr.size());

        size_t cStyleStrIndex = 0;
        while (cStyleStrIndex < _CStyleStr.size())
        {
            if (_CStyleStr[cStyleStrIndex] != '\\')
            {
                rstString.append(1, _CStyleStr[cStyleStrIndex]);
                cStyleStrIndex++;
            }
            else
            {
                char shiftChar = 0;
                int cStyleStrLeftLen = _CStyleStr.size() - cStyleStrIndex;
                cStyleStrIndex += ProcShiftChar(shiftChar, &_CStyleStr[cStyleStrIndex], cStyleStrLeftLen);
                rstString.append(1, shiftChar);
            }
        }

        return rstString;
    }

    // when len <= prefixLen
    //   test_str(adfadf, 3) == adf
    // when len < 20
    //   test_str(adf, 13) == adfa:67890123
    // when len >= 20
    //   test_str(asdf, 45) = asdf:=====(20_56789)(30_56789)12345
    static std::string gen_test_str(const std::string& str_prefix, const size_t str_len)
    {
        std::ostringstream ostr;
        ostr << str_prefix << ':';
        std::string str = ostr.str();

        // when len <= prefixLen
        //   test_str(adfadf, 3) == adf
        if (str.size() >= str_len)
        {
            str.resize(str_len);
            return str;
        }

        // when len < 20
        //   test_str(adf, 13) == adfa:67890123
        if (str_len < 20)
        {
            for (size_t i = str.size() + 1; i <= str_len; ++i)
            {
                str.push_back(char(((i % 10) + 0x30)));
            }

            return str;
        }

        // when len >= 20
        //   test_str(asdf, 45) = asdf:=====(20_56789)(30_56789)12345
        {
            // asdf:=====
            str.resize(10, '=');

            // (20_56789)(30_56789)
            while (str.size() + 10 <= str_len)
            {
                const size_t new_len = str.size() + 10;
                std::ostringstream tmp_ostrm;
                tmp_ostrm << str << "(" << new_len << '_';
                str = tmp_ostrm.str();
                for (size_t i = str.size() + 1; i < new_len; ++i)
                {
                    str.push_back(char(((i % 10) + 0x30)));
                }
                str += ')';
            }

            // 12345
            if (str.size() < str_len)
            {
                for (size_t i = str.size() + 1; i <= str_len; ++i)
                {
                    char c = ((i % 10) + 0x30);
                    str.push_back(c);
                }
            }

            return str;
        }

        return str;
    }

    static std::string make_test_str(size_t test_str_size)
    {
        static const std::string START_STR = "!T_";
        static const std::string INTERVAL_STR = "_T";

        std::ostringstream ostr;
        ostr << START_STR << AFDateTime::GetNowTime();
        std::string msg_str = ostr.str();
        msg_str += gen_test_str(INTERVAL_STR, test_str_size - msg_str.size());
        return msg_str;
    }

    static std::string random_string(size_t length)
    {
        auto randchar = []() -> char {
            const char charset[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[rand() % max_index];
        };
        std::string str(length, 0);
        std::generate_n(str.begin(), length, randchar);
        return str;
    }
};

} // namespace ark
