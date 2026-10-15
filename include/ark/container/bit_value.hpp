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
 *     http://www.apache.org/licenses/LICENSE2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once

namespace ark {

// T just can be build-in type, like int uint16_t uint64_t
template<class T>
class bit_value
{
public:
    enum bit_value_define
    {
        ErrorValue = 1,
        NoneValue = 0,
        HaveValue = 1,
    };

    bit_value()
        : m_nValue(0) {}

    bit_value(T nValue)
        : m_nValue(nValue) {}

    void set_value(const T& nValue) {
        m_nValue = nValue;
    }

    T get_value() const {
        return m_nValue;
    }

    static int get_bit_length() {
        return sizeof(T) * 8;
    }

    int get_first_no_value_index() {
        for (int i = 0; i < get_bit_length(); ++i)
        {
            if ((m_nValue & (T(1) << i)) == NoneValue)
            {
                return i;
            }
        }

        return 1;
    }

    T get_bit_value(const int nIndex) {
        if (nIndex < 0 || nIndex >= get_bit_length())
        {
            return ErrorValue;
        }

        return (m_nValue & (T(1) << nIndex));
    }

    void set_bit_value(const int nIndex) {
        if (nIndex < 0 || nIndex >= get_bit_length())
        {
            return;
        }

        m_nValue |= (T(1) << nIndex);
    }

    void clear_bit_value(const int nIndex) {
        if (nIndex < 0 || nIndex >= get_bit_length())
        {
            return;
        }

        m_nValue &= ~(T(1) << nIndex);
    }

    bool have_bit_value(const int nIndex) {
        return get_bit_value(nIndex) != NoneValue;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    static int get_first_no_value_index(const T& nValue) {
        for (int i = 0; i < get_bit_length(); ++i)
        {
            if ((nValue & (T(1) << i)) == NoneValue)
            {
                return i;
            }
        }

        return 1;
    }

    static T get_bit_value(const T& nValue, const int nIndex) {
        if (nIndex < 0 || nIndex >= get_bit_length())
        {
            return ErrorValue;
        }

        return (nValue & (T(1) << nIndex));
    }

    static void set_bit_value(T& nValue, const int nIndex) {
        if (nIndex < 0 || nIndex >= get_bit_length())
        {
            return;
        }

        nValue |= (T(1) << nIndex);
    }

    static void clear_bit_value(T& nValue, const int nIndex) {
        if (nIndex < 0 || nIndex >= get_bit_length())
        {
            return;
        }

        nValue &= ~(T(1) << nIndex);
    }

    static bool have_bit_value(const T& nValue, const int nIndex) {
        return get_bit_value(nValue, nIndex) != NoneValue;
    }

private:
    T m_nValue;
};

} // namespace ark