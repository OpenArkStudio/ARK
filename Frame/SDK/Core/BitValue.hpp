#pragma once

//对内置类型的位操作 类型值只能是内置的类型(int, short, char..等)
template<class T>
class BitValue
{
public:
    enum BitValueDefine
    {
        ErrorValue = -1,        // 错误值
        NoneValue = 0,          // 没有位值
        HaveValue = 1,          // 存在位值
    };

    BitValue() : m_nValue(0) { }
    BitValue(T nValue) : m_nValue(nValue) { }

    void SetValue(T nValue)
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

    // 获得第一个位值是0的位索引
    int GetFirstNoValueIndex()
    {
        for (int i = 0; i < GetBitLength(); ++i)
        {
            if ((m_nValue & (T(1) << i)) == NoneValue)
            {
                return i;
            }
        }

        return -1;  // 32位都有值
    }

    T GetBitValue(unsigned char nIndex)         // 得到某位上的值
    {
        if (nIndex < 0 || nIndex >= GetBitLength())
        {
            return ErrorValue;
        }

        return (m_nValue & (T(1) << nIndex));
    }

    void SetBitValue(unsigned char nIndex)        // 设置某位上的值
    {
        if (nIndex < 0 || nIndex >= GetBitLength())
        {
            return;
        }

        m_nValue |= (T(1) << nIndex);
    }

    void ClearBitValue(unsigned char nIndex)      // 清除某位上的值
    {
        if (nIndex < 0 || nIndex >= GetBitLength())
        {
            return;
        }

        m_nValue &= ~(T(1) << nIndex);
    }

    bool HaveBitValue(unsigned char nIndex)
    {
        return GetBitValue(nIndex) != NoneValue;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    static int GetFirstNoValueIndex(T nValue)
    {
        for (int i = 0; i < GetBitLength(); ++i)
        {
            if ((nValue & (T(1) << i)) == NoneValue)
            {
                return i;
            }
        }
        return -1;  // 32位都有值
    }

    static T GetBitValue(T nValue, unsigned char nIndex)
    {
        if (nIndex < 0 || nIndex >= GetBitLength())
        {
            return ErrorValue;
        }

        return (nValue & (T(1) << nIndex));
    }

    static void SetBitValue(T& nValue, unsigned char nIndex)
    {
        if (nIndex < 0 || nIndex >= GetBitLength())
        {
            return;
        }

        nValue |= (T(1) << nIndex);
    }

    static void ClearBitValue(T& nValue, unsigned char nIndex)
    {
        if (nIndex < 0 || nIndex >= GetBitLength())
        {
            return;
        }

        nValue &= ~(T(1) << nIndex);
    }

    static bool HaveBitValue(T nValue, unsigned char nIndex)
    {
        return GetBitValue(nValue, nIndex) != NoneValue;
    }

private:
    T m_nValue;
};