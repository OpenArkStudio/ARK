// C++ 11实现的lexical_cast，为了剥离boost
//代码来自: https://github.com/qicosmos/cosmos

#ifndef CPP_11_LEXICAL_CAST_HPP
#define CPP_11_LEXICAL_CAST_HPP

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

using namespace std;

namespace converter {
static const char* strue = "true";
static const char* sfalse = "false";

template<typename To, typename From>
struct value_converter
{
};

// to numeric
template<typename From>
struct value_converter<uint8_t, From>
{
    static uint8_t convert(const string& from) {
        return std::atoi(from.c_str());
    }

    static uint8_t convert(const char* from) {
        return std::atoi(from);
    }
};

template<typename From>
struct value_converter<int16_t, From>
{
    static int16_t convert(const string& from) {
        return std::atoi(from.c_str());
    }

    static int16_t convert(const char* from) {
        return std::atoi(from);
    }
};

template<typename From>
struct value_converter<uint16_t, From>
{
    static uint16_t convert(const string& from) {
        return std::atoi(from.c_str());
    }

    static uint16_t convert(const char* from) {
        return std::atoi(from);
    }
};

template<typename From>
struct value_converter<int32_t, From>
{
    static int32_t convert(const string& from) {
        return std::atoi(from.c_str());
    }

    static int32_t convert(const char* from) {
        return std::atoi(from);
    }
};

template<typename From>
struct value_converter<uint32_t, From>
{
    static uint32_t convert(const string& from) {
        return std::atoi(from.c_str());
    }

    static uint32_t convert(const char* from) {
        return std::atoi(from);
    }
};

template<typename From>
struct value_converter<long, From>
{
    static long convert(const string& from) {
        return std::atol(from.c_str());
    }

    static long convert(const char* from) {
        return std::atol(from);
    }
};

template<typename From>
struct value_converter<long long, From>
{
    static long long convert(const string& from) {
        return std::atoll(from.c_str());
    }

    static long long convert(const char* from) {
        return std::atoll(from);
    }
};

template<typename From>
struct value_converter<uint64_t, From>
{
    static uint64_t convert(const string& from) {
        return std::atoll(from.c_str());
    }

    static uint64_t convert(const char* from) {
        return std::atoll(from);
    }
};

template<typename From>
struct value_converter<double, From>
{
    static double convert(const string& from) {
        return std::atof(from.c_str());
    }

    static double convert(const char* from) {
        return std::atof(from);
    }
};

template<typename From>
struct value_converter<float, From>
{
    static float convert(const string& from) {
        return (float)std::atof(from.c_str());
    }

    static float convert(const char* from) {
        return (float)std::atof(from);
    }
};

// to bool
template<typename From>
struct value_converter<bool, From>
{
    static typename std::enable_if<std::is_integral<From>::value, bool>::type convert(From from) {
        return !!from;
    }
};

static bool checkbool(const char* from, const size_t len, const char* s) {
    for (size_t i = 0; i < len; i++)
    {
        if (from[i] != s[i])
        {
            return false;
        }
    }

    return true;
}

static bool convert(const char* from) {
    size_t len = strlen(from);

    bool r = true;
    if (len == 4)
    {
        //"true"
        r = checkbool(from, len, strue);

        if (r)
        {
            return true;
        }
    }
    else if (len == 5)
    {
        //"false"
        r = checkbool(from, len, sfalse);

        if (r)
        {
            return false;
        }
    }
    else
    {
        // 数字转为bool
        int value = value_converter<int, const char*>::convert(from);
        return (value > 0);
    }

    throw std::invalid_argument("argument is invalid");
}

template<>
struct value_converter<bool, string>
{
    static bool convert(const string& from) {
        return converter::convert(from.c_str());
    }
};

template<>
struct value_converter<bool, const char*>
{
    static bool convert(const char* from) {
        return converter::convert(from);
    }
};

template<>
struct value_converter<bool, char*>
{
    static bool convert(char* from) {
        return converter::convert(from);
    }
};

template<unsigned N>
struct value_converter<bool, const char[N]>
{
    static bool convert(const char (&from)[N]) {
        return converter::convert(from);
    }
};

template<unsigned N>
struct value_converter<bool, char[N]>
{
    static bool convert(const char (&from)[N]) {
        return converter::convert(from);
    }
};

// to string
template<typename From>
struct value_converter<string, From>
{
    static string convert(const From& from) {
        return std::to_string(from);
    }
};
} // namespace converter

template<typename To, typename From>
typename std::enable_if<!std::is_same<To, From>::value, To>::type lexical_cast(const From& from) {
    return converter::value_converter<To, From>::convert(from);
}

template<typename To, typename From>
typename std::enable_if<std::is_same<To, From>::value, To>::type lexical_cast(const From& from) {
    return from;
}

template<typename ValueTYPE>
bool value_from_string(const std::string& strValue, ValueTYPE& nValue) {
    try
    {
        nValue = lexical_cast<ValueTYPE>(strValue);
        return true;
    }
    catch (...)
    { return false; }

    return false;
}

template<typename ValueTYPE>
bool value_to_string(const ValueTYPE& nValue, std::string& strData) {
    try
    {
        strData = lexical_cast<std::string>(nValue);
        return true;
    }
    catch (...)
    { return false; }

    return false;
}
#endif //! CPP_11_LEXICAL_CAST_HPP
