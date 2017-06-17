#pragma once

#include "AFIData.h"

namespace ArkFrame
{

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
        delete[](char*)ptr;
    }

    void Swap(AFDataAlloc& src)
    {
        //do nothing
    }
};

template<size_t BUFFER_SIZE, typename ALLOC = AFDataAlloc>
class AFCData : public AFIData
{
public:
    using self_t = AFCData<BUFFER_SIZE, ALLOC>;

public:
    AFCData()
    {
        mnType = DT_UNKNOWN;
        mn64Value = 0;
    }

    virtual ~AFCData()
    {
        Release();
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
            mbValue = src.GetBool();
            break;
        case DT_INT:
            mnValue = src.GetInt();
            break;
        case DT_INT64:
            mn64Value = src.GetInt64();
            break;
        case DT_FLOAT:
            mfValue = src.GetFloat();
            break;
        case DT_DOUBLE:
            mdValue = src.GetDouble();
            break;
        case DT_STRING:
            InnerSetString(src.GetString());
            break;
        case DT_OBJECT:
        {
            mnIdent = src.GetObject().nIdent;
            mnSerial = src.GetObject().nSerial;
        }
        break;
        case DT_POINTER:
            mpVaule = src.Pointer();
            break;
        case DT_USERDATA:
        {
            size_t size;
            const void* pData = src.GetUserData(size);
            InnerSetUserData(pData, size);
        }
        break;
        default:
            break;
        }
    }

    AFCData(int type, bool value)
    {
        assert(type == DT_BOOLEAN);

        mnType = DT_BOOLEAN;
        mbValue = value;
    }

    AFCData(int type, int value)
    {
        assert(type == DT_INT);

        mnType = DT_INT;
        mnValue = value;
    }

    AFCData(int type, int64_t value)
    {
        assert(type == DT_INT64);

        mnType = DT_INT64;
        mnValue = value;
    }

    AFCData(int type, float value)
    {
        assert(type == DT_FLOAT);

        mnType = DT_FLOAT;
        mfValue = value;
    }

    AFCData(int type, double value)
    {
        assert(type == DT_DOUBLE);

        mnType = DT_DOUBLE;
        mdValue = value;
    }

    AFCData(int type, const char* value)
    {
        assert(type == DT_STRING);

        mnType = DT_STRING;
        InnerSetString(value);
    }

    AFCData(int type, const AFGUID& value)
    {
        assert(type == DT_OBJECT);

        mnType = DT_OBJECT;
        mnIdent = value.nIdent;
        mnSerial = value.nSerial;
    }

    AFCData(int type, const void* value, size_t size)
    {
        assert(type == DT_USERDATA);

        mnType = DT_USERDATA;
        InnerSetUserData(value, size);
    }

    self_t& operator=(const self_t& src)
    {
        self_t tmp(src);
        Swap(tmp);
        return *this;
    }

    self_t& operator=(const AFIData& src)
    {
        self_t tmp(src);
        Swap(tmp);
        return *this;
    }

    void Swap(self_t& src)
    {
        int tmp_type = this->mnType;
        int64_t tmp_value = this->mn64Value;
        uint32_t tmp_alloc_len = this->mnAllocLen;
        char tmp_buffer[BUFFER_SIZE];
        bool tmp_use_buffer = (tmp_type == DT_STRING) || (this->mstrValue == this->mBuffer);
        if ((src.mnType == DT_STRING) || (src.mstrValue == src.mBuffer))
        {
            if (tmp_use_buffer)
            {
                memcpy(tmp_buffer, this->mBuffer, BUFFER_SIZE);
            }

            memcpy(this->mBuffer, src.mBuffer, BUFFER_SIZE);
            mstrValue = mBuffer;
        }
        else
        {
            this->mn64Value = src.mn64Value;
            mnAllocLen = src.mnAllocLen;
        }

        this->mnType = src.mnType;

        if (tmp_use_buffer)
        {
            memcpy(src.mBuffer, tmp_buffer, BUFFER_SIZE);
            src.mstrValue = src.mBuffer;
        }
        else
        {
            src.mn64Value = tmp_value;
            src.mnAllocLen = tmp_alloc_len;
        }

        src.mnType = tmp_type;
        mxAlloc.Swap(src.mxAlloc);
    }
    //////////////////////////////////////////////////////////////////////////

    virtual int GetType() const
    {
        return mnType;
    }

    //Get data
    virtual bool GetBool() const
    {
        assert(mnType == DT_BOOLEAN);

        if (mnType != DT_BOOLEAN)
        {
            return NULL_BOOLEAN;
        }

        return mbValue;
    }

    virtual int GetInt() const
    {
        assert(mnType == DT_INT);

        if (mnType != DT_INT)
        {
            return NULL_INT;
        }

        return mnValue;
    }

    virtual int64_t GetInt64() const
    {
        assert(mnType == DT_INT64);

        if (mnType != DT_INT64)
        {
            return NULL_INT64;
        }

        return mn64Value;
    }

    virtual float GetFloat() const
    {
        assert(mnType == DT_FLOAT);

        if (mnType != DT_FLOAT)
        {
            return NULL_FLOAT;
        }

        return mfValue;
    }

    virtual double GetDouble() const
    {
        assert(mnType == DT_DOUBLE);

        if (mnType != DT_DOUBLE)
        {
            return NULL_DOUBLE;
        }

        return mdValue;
    }

    virtual const char* GetString() const
    {
        assert(mnType == DT_STRING);

        if (mnType != DT_STRING)
        {
            return NULL_STR.c_str();
        }

        return mstrValue;
    }

    virtual AFGUID GetObject() const
    {
        assert(mnType == DT_OBJECT);

        if (mnType != DT_OBJECT)
        {
            return NULL_GUID;
        }

        return AFGUID(mnIdent, mnSerial);
    }

    virtual void* GetPointer() const
    {
        assert(mnType == DT_POINTER);

        if (mnType != DT_POINTER)
        {
            return NULL;
        }

        return mpVaule;
    }

    virtual const void* GetUserData(size_t& size) const
    {
        assert(mnType == DT_POINTER);

        if (mnType != DT_POINTER)
        {
            size = 0;
            return NULL;
        }

        size = AFIData::GetUserDataSize(mpUserData);
        return AFIData::GetUserData(mpUserData);
    }

    virtual void* GetRawUserData() const
    {
        assert(mnType == DT_POINTER);

        if (mnType != DT_POINTER)
        {
            return NULL;
        }

        return mpUserData;
    }

    //Set data
    virtual void SetUnknown()
    {
        Release();
        mnType = DT_UNKNOWN;
        mn64Value = 0;
    }

    virtual void SetBool(bool value)
    {
        Release();
        mnType = DT_BOOLEAN;
        mbValue = value;
    }

    virtual void SetInt(int value)
    {
        Release();
        mnType = DT_INT;
        mnValue = value;
    }

    virtual void SetInt64(int64_t value)
    {
        Release();
        mnType = DT_INT64;
        mn64Value = value;
    }

    virtual void SetFloat(float value)
    {
        Release();
        mnType = DT_FLOAT;
        mfValue = value;
    }

    virtual void SetDouble(double value)
    {
        Release();
        mnType = DT_DOUBLE;
        mdValue = value;
    }

    virtual void SetString(const char* value)
    {
        Release();
        mnType = DT_STRING;
        InnerSetString(value);
    }

    virtual void SetObject(const AFGUID& value)
    {
        Release();
        mnType = DT_OBJECT;
        mnIdent = value.nIdent;
        mnSerial = value.nSerial;
    }

    virtual void SetPointer(void* value)
    {
        Release();
        mnType = DT_POINTER;
        mpVaule = value;
    }

    virtual void SetUserData(const void* value, size_t size)
    {
        Release();
        mnType = DT_USERDATA;
        InnerSetUserData(value, size);
    }

    virtual void SetRawUserData(void* value)
    {
        SetUserData(AFIData::GetUserData(value), AFIData::GetUserDataSize(value));
    }

    virtual void Assign(const AFIData& src)
    {
        self_t tmp(src);
        Swap(tmp);
    }

    virtual size_t GetMemUsage() const
    {
        size_t size = sizeof(self_t);
        switch (mnType)
        {
        case DT_STRING:
        {
            if (NULL != mstrValue)
            {
                size += strlen(mstrValue) + 1;
            }
        }
        break;
        case DT_USERDATA:
        {
            if (NULL != mpUserData)
            {
                size += AFIData::GetUserDataSize(mpUserData);
            }
        }
        break;
        default:
            break;
        }

        return size;
    }

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

    //可变数据联合体 size = 8
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

//special
using AFXData = AFCData<4>;

}