#pragma once

#include "AFIData.h"

class AFDataAlloc
{
public:
    AFDataAlloc() {}
    ~AFDataAlloc() {}

    void* Alloc(size_t size)
    {
        return new char[size];
    }

    void Free(void* ptr, size_t size)
    {
        delete [](char*)ptr;
    }

    void Swap(AFDataAlloc& src)
    {
        //do nothing
    }
};

template<size_t BUFFER_SIZE, typename ALLOC = AFDataAlloc>
class AFCData
{
public:
    using self_t = AFCData<BUFFER_SIZE, ALLOC>;

public:
    AFCData()
    {
        mnType = DT_UNKNOWN;
        mn64Value = 0;
    }

    AFCData(const self_t& src)
    {
        this->mnType = src.mnType;
        switch (this->mnType)
        {
        case DT_BOOLEAN:
            mbValue = src.mbValue;
            break;
        case DT_INT:
            mnValue = src.mnValue;
            break;
        case DT_INT64:
            mn64Value = src.mn64Value;
            break;
        case DT_FLOAT:
            mfValue = src.mfValue;
            break;
        case DT_DOUBLE:
            mdValue = src.mdValue;
            break;
        case DT_STRING:
            InnerSetString(src.String());
            break;
        case DT_OBJECT:
            {
                mnIdent = src.mnIdent;
                mnSerial = src.mnSerial;
            }
            break;
        case DT_POINTER:
            mpVaule = src.mpVaule;
            break;
        case DT_USERDATA:
        {
            size_t size;
            const void* pData = src.mpUserData(size);
            InnerSetUserData(pData, size);
        }
            break;
        default:
            break;
        }
    }

    explicit AFCData(const AFIData& src)
    {
        this->mnType = src.GetType();
        switch (this->mnType)
        {
        case DT_BOOLEAN:
            mbValue = src.Bool();
            break;
        case DT_INT:
            mnValue = src.Int();
            break;
        case DT_INT64:
            mn64Value = src.Int64();
            break;
        case DT_FLOAT:
            mfValue = src.Float();
            break;
        case DT_DOUBLE:
            mdValue = src.Double();
            break;
        case DT_STRING:
            InnerSetString(src.String());
            break;
        case DT_OBJECT:
        {
            mnIdent = src.Object().nIdent;
            mnSerial = src.Object().nSerial;
        }
        break;
        case DT_POINTER:
            mpVaule = src.Pointer();
            break;
        case DT_USERDATA:
        {
            size_t size;
            const void* pData = src.UserData(size);
            InnerSetUserData(pData, size);
        }
        break;
        default:
            break;
        }
    }

    //TODO

protected:
    void Release()
    {
        switch (mnType)
        {
        case DT_STRING:
            {
                if (mstrValue != mBuffer)
                {
                    mxAlloc.Free(mstrValue, mnAllocLen);
                    mstrValue = NULL;
                }
            }
            break;
        case DT_USERDATA:
            {
                if (NULL != mpUserData)
                {
                    mxAlloc.Free(mpUserData, mnAllocLen);
                    mpUserData = NULL;
                }
            }
            break;
        default:
            break;
        }
    }

    void InnerSetString(const char* value)
    {
        const size_t value_size = strlen(value) + 1;
        char* p = NULL;

        if (value_size > BUFFER_SIZE)
        {
            p = (char*)mxAlloc.Alloc(value_size);
            mnAllocLen = value_size;
        }
        else
        {
            p = mBuffer;
        }

        memcpy(p, value, value_size);
        mstrValue = p;
    }

    void InnerSetUserData(const void* data, size_t size)
    {
        size_t value_size = GetRawUserDataSize(size);
        char* p = (char*)mxAlloc.Alloc(value_size);
        InitRawUserData(p, data, size);
        mpUserData = p;
        mnAllocLen = value_size;
    }

private:
    ALLOC mxAlloc;
    int mnType;

    //可变数据联合体
    union
    {
        bool mbValue;
        int mnValue;
        int64_t mn64Value;
        float mfValue;
        double mdValue;
        char* mstrValue;
        void* mpVaule;
        char* mpUserData;

        struct 
        {
            uint32_t mnIdent;
            uint32_t mnSerial;
        };
    };

    //额外buffer
    union
    {
        uint32_t mnAllocLen;
        char mBuffer[BUFFER_SIZE];
    };
};