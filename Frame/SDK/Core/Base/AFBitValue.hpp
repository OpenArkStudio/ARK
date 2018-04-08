/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

//瀵瑰唴缃被鍨嬬殑浣嶆搷浣?绫诲瀷鍊煎彧鑳芥槸鍐呯疆鐨勭被鍨?int, short, char..绛?
template<class T>
class BitValue
{
public:
    enum BitValueDefine
    {
        ErrorValue = -1,        // 閿欒鍊?
        NoneValue = 0,          // 娌℃湁浣嶅€?
        HaveValue = 1,          // 瀛樺湪浣嶅€?
    };

    BitValue() : m_nValue(0) { }
    BitValue(T nValue) : m_nValue(nValue) { }

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

    // 鑾峰緱绗竴涓綅鍊兼槸0鐨勪綅绱㈠紩
    int GetFirstNoValueIndex()
    {
        for(int i = 0; i < GetBitLength(); ++i)
        {
            if((m_nValue & (T(1) << i)) == NoneValue)
            {
                return i;
            }
        }

        return -1;  // 32浣嶉兘鏈夊€?
    }

    T GetBitValue(const int nIndex)         // 寰楀埌鏌愪綅涓婄殑鍊?
    {
        if(nIndex < 0 || nIndex >= GetBitLength())
        {
            return ErrorValue;
        }

        return (m_nValue & (T(1) << nIndex));
    }

    void SetBitValue(const int nIndex)        // 璁剧疆鏌愪綅涓婄殑鍊?
    {
        if(nIndex < 0 || nIndex >= GetBitLength())
        {
            return;
        }

        m_nValue |= (T(1) << nIndex);
    }

    void ClearBitValue(const int nIndex)      // 娓呴櫎鏌愪綅涓婄殑鍊?
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

        return -1;  // 32浣嶉兘鏈夊€?
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