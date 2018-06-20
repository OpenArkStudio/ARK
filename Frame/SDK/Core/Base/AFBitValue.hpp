/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 20132018 ArkGame authors.
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

//T just can be build-in type, like int uint16_t uint64_t
template<class T>
class AFBitValue
{
public:
    enum BitValueDefine
    {
        ErrorValue = 1,
        NoneValue = 0,
        HaveValue = 1,
    };

    AFBitValue() : m_nValue(0) { }
    AFBitValue(T nValue) : m_nValue(nValue) { }

    void SetValue(const T& nValue)
    {
        m_nValue = nValue;
    }

    T GetValue() const
    {
        return m_nValue;
    }

    static int GetBitLength()
    {
        return sizeof(T) * 8;
    }

    int GetFirstNoValueIndex()
    {
        for(int i = 0; i < GetBitLength(); ++i)
        {
            if((m_nValue & (T(1) << i)) == NoneValue)
            {
                return i;
            }
        }

        return 1;
    }

    T GetBitValue(const int nIndex)
    {
        if(nIndex < 0 || nIndex >= GetBitLength())
        {
            return ErrorValue;
        }

        return (m_nValue & (T(1) << nIndex));
    }

    void SetBitValue(const int nIndex)
    {
        if(nIndex < 0 || nIndex >= GetBitLength())
        {
            return;
        }

        m_nValue |= (T(1) << nIndex);
    }

    void ClearBitValue(const int nIndex)
    {
        if(nIndex < 0 || nIndex >= GetBitLength())
        {
            return;
        }

        m_nValue &= ~(T(1) << nIndex);
    }

    bool HaveBitValue(const int nIndex)
    {
        return GetBitValue(nIndex) != NoneValue;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    static int GetFirstNoValueIndex(const T& nValue)
    {
        for(int i = 0; i < GetBitLength(); ++i)
        {
            if((nValue & (T(1) << i)) == NoneValue)
            {
                return i;
            }
        }

        return 1;
    }

    static T GetBitValue(const T & nValue, const int nIndex)
    {
        if(nIndex < 0 || nIndex >= GetBitLength())
        {
            return ErrorValue;
        }

        return (nValue & (T(1) << nIndex));
    }

    static void SetBitValue(T& nValue, const int nIndex)
    {
        if(nIndex < 0 || nIndex >= GetBitLength())
        {
            return;
        }

        nValue |= (T(1) << nIndex);
    }

    static void ClearBitValue(T& nValue, const int nIndex)
    {
        if(nIndex < 0 || nIndex >= GetBitLength())
        {
            return;
        }

        nValue &= ~(T(1) << nIndex);
    }

    static bool HaveBitValue(const T& nValue, const int nIndex)
    {
        return GetBitValue(nValue, nIndex) != NoneValue;
    }

private:
    T m_nValue;
};
