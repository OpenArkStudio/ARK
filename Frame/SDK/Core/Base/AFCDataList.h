/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "AFIData.h"
#include "AFIDataList.h"
#include "AFMisc.hpp"

class AFDataListAlloc
{
public:
    AFDataListAlloc() {}
    ~AFDataListAlloc() {}

    void* Alloc(size_t size)
    {
        return new char[size];
    }

    void Free(void* ptr, size_t size)
    {
        delete[](char*)ptr;
    }

    void Swap(AFDataListAlloc& src)
    {
        //Do nothing
    }
};

template<size_t DATA_SIZE, size_t BUFFER_SIZE, typename ALLOC = AFDataListAlloc>
class AFBaseDataList : public AFIDataList
{
private:
    using self_t = AFBaseDataList<DATA_SIZE, BUFFER_SIZE, ALLOC>;

    struct dynamic_data_t
    {
        int nType;
        union
        {
            bool mbValue;
            int mnValue;
            int64_t mn64Value;
            float mfValue;
            double mdValue;
            size_t mnstrValue;
            void* mpVaule;
            size_t mnUserData;

            struct
            {
                uint64_t mnHigh;
                uint64_t mnLow;
            };
        };
    };

public:
    AFBaseDataList()
    {
        assert(DATA_SIZE > 0);
        assert(BUFFER_SIZE > 0);

        mpData = mDataStack;
        mnDataSize = DATA_SIZE;
        mnDataUsed = 0;

        mpBuffer = mBufferStack;
        mnBufferSize = BUFFER_SIZE;
        mnBufferUsed = 0;
    }

    AFBaseDataList(const char* strSour, int nLengh, char strSplit)
    {
        assert(DATA_SIZE > 0);
        assert(BUFFER_SIZE > 0);

        mpData = mDataStack;
        mnDataSize = DATA_SIZE;
        mnDataUsed = 0;

        mpBuffer = mBufferStack;
        mnBufferSize = BUFFER_SIZE;
        mnBufferUsed = 0;

        int nBegin = 0;
        int nEnd = 0;
        for(int i = 0; i < nLengh; i++)
        {
            if(strSour[i] == strSplit)
            {
                nEnd = i;
                if(!AddString(&strSour[nBegin], nEnd - nBegin))
                {
                    //return false;
                }

                if(i + 1 < nLengh)
                {
                    nBegin = i + 1;
                }
            }
        }

        if(nEnd < nLengh)
        {
            nEnd = nLengh;
            if(!AddString(&strSour[nBegin], nEnd - nBegin))
            {
                //return false;
            }
        }
    }

    AFBaseDataList(const self_t& src)
    {
        assert(DATA_SIZE > 0);
        assert(BUFFER_SIZE > 0);

        mpData = mDataStack;
        mnDataSize = DATA_SIZE;
        mnDataUsed = 0;

        mpBuffer = mBufferStack;
        mnBufferSize = BUFFER_SIZE;
        mnBufferUsed = 0;
        InnerAppend(src, 0, src.GetCount());
    }

    AFBaseDataList(const AFIDataList& src)
    {
        assert(DATA_SIZE > 0);
        assert(BUFFER_SIZE > 0);

        mpData = mDataStack;
        mnDataSize = DATA_SIZE;
        mnDataUsed = 0;

        mpBuffer = mBufferStack;
        mnBufferSize = BUFFER_SIZE;
        mnBufferUsed = 0;
        InnerAppend(src, 0, src.GetCount());
    }

    virtual ~AFBaseDataList()
    {
        Release();
    }

    self_t operator==(const self_t src)
    {
        Release();

        mpData = mDataStack;
        mnDataSize = DATA_SIZE;
        mnDataUsed = 0;

        mpBuffer = mBufferStack;
        mnBufferSize = BUFFER_SIZE;
        mnBufferUsed = 0;
        InnerAppend(src, 0, src.GetCount());

        return *this;
    }

    void Release()
    {
        if(mnDataSize > DATA_SIZE)
        {
            mxAlloc.Free(mpData, mnDataSize * sizeof(dynamic_data_t));
        }

        if(mnBufferSize > BUFFER_SIZE)
        {
            mxAlloc.Free(mpBuffer, mnBufferSize * sizeof(char));
        }
    }

    virtual bool Concat(const AFIDataList& src)
    {
        InnerAppend(src, 0, src.GetCount());
        return true;
    }

    virtual bool Append(const AFIData& data)
    {
        InnerAppend(data);
        return true;
    }

    virtual bool Append(const AFIDataList& src, size_t start, size_t count)
    {
        if(start >= src.GetCount())
        {
            return false;
        }

        size_t end = start + count;
        if(end > src.GetCount())
        {
            return false;
        }

        InnerAppend(src, start, end);
        return true;
    }

    virtual void Clear()
    {
        mnDataUsed = 0;
        mnBufferUsed = 0;
    }

    virtual bool Empty() const
    {
        return (0 == mnDataUsed);
    }

    virtual size_t GetCount() const
    {
        return mnDataUsed;
    }

    virtual int GetType(size_t index) const
    {
        if(index >= mnDataUsed)
        {
            return DT_UNKNOWN;
        }

        return mpData[index].nType;
    }

    //add data
    virtual bool AddBool(bool value)
    {
        dynamic_data_t* p = AddDynamicData();
        p->nType = DT_BOOLEAN;
        p->mbValue = value;
        return true;
    }

    virtual bool AddInt(int value)
    {
        dynamic_data_t* p = AddDynamicData();
        p->nType = DT_INT;
        p->mnValue = value;
        return true;
    }

    virtual bool AddInt64(int64_t value)
    {
        dynamic_data_t* p = AddDynamicData();
        p->nType = DT_INT64;
        p->mn64Value = value;
        return true;
    }

    virtual bool AddFloat(float value)
    {
        dynamic_data_t* p = AddDynamicData();
        p->nType = DT_FLOAT;
        p->mfValue = value;
        return true;
    }

    virtual bool AddDouble(double value)
    {
        dynamic_data_t* p = AddDynamicData();
        p->nType = DT_DOUBLE;
        p->mdValue = value;
        return true;
    }

    virtual bool AddString(const char* value)
    {
        assert(value != nullptr);
        dynamic_data_t* p = AddDynamicData();
        p->nType = DT_STRING;
        p->mnstrValue = mnBufferUsed;

        const size_t value_size = strlen(value) + 1;
        char* data = AddBuffer(value_size);
        memcpy(data, value, value_size);

        return true;
    }

    virtual bool AddString(const char* value, const int nLength)
    {
        if(nLength <= 0)
        {
            return false;
        }

        assert(value != nullptr);
        dynamic_data_t* p = AddDynamicData();
        if(nullptr == p)
        {
            return false;
        }

        p->nType = DT_STRING;
        p->mnstrValue = mnBufferUsed;

        //need only need  sub string
        size_t value_size = strlen(value);
        if(value_size > (size_t)nLength)
        {
            value_size = (size_t)nLength;
        }

        value_size += 1;
        char* data = AddBuffer(value_size);
        memcpy(data, value, value_size);

        return true;
    }

    virtual bool AddObject(const AFGUID& value)
    {
        dynamic_data_t* p = AddDynamicData();
        p->nType = DT_OBJECT;
        p->mnHigh = value.nHigh;
        p->mnLow = value.nLow;
        return true;
    }

    virtual bool AddPointer(void* value)
    {
        dynamic_data_t* p = AddDynamicData();
        p->nType = DT_POINTER;
        p->mpVaule = value;
        return true;
    }

    virtual bool AddUserData(const void* pData, size_t size)
    {
        assert(pData != nullptr);

        dynamic_data_t* p = AddDynamicData();
        p->nType = DT_USERDATA;
        p->mnUserData = mnBufferUsed;

        const size_t value_size = AFIData::GetRawUserDataSize(size);
        char* value = AddBuffer(value_size);
        AFIData::InitRawUserData(value, pData, size);

        return true;
    }

    virtual bool AddRawUserData(void* value)
    {
        return AddUserData(AFIData::GetUserData(value), AFIData::GetUserDataSize(value));
    }

    //get data
    virtual bool Bool(size_t index) const
    {
        if(index > mnDataUsed)
        {
            return NULL_BOOLEAN;
        }

        switch(mpData[index].nType)
        {
        case DT_BOOLEAN:
            return mpData[index].mbValue;
            break;
        default:
            break;
        }

        return NULL_BOOLEAN;
    }

    virtual int Int(size_t index) const
    {
        if(index > mnDataUsed)
        {
            return NULL_INT;
        }

        switch(mpData[index].nType)
        {
        case DT_INT:
            return mpData[index].mnValue;
            break;
        default:
            break;
        }

        return NULL_INT;
    }

    virtual int64_t Int64(size_t index) const
    {
        if(index > mnDataUsed)
        {
            return NULL_INT64;
        }

        switch(mpData[index].nType)
        {
        case DT_INT64:
            return mpData[index].mn64Value;
            break;
        default:
            break;
        }

        return NULL_INT64;
    }

    virtual float Float(size_t index) const
    {
        if(index > mnDataUsed)
        {
            return NULL_FLOAT;
        }

        switch(mpData[index].nType)
        {
        case DT_FLOAT:
            return mpData[index].mfValue;
            break;
        default:
            break;
        }

        return NULL_FLOAT;
    }

    virtual double Double(size_t index) const
    {
        if(index > mnDataUsed)
        {
            return NULL_DOUBLE;
        }

        switch(mpData[index].nType)
        {
        case DT_DOUBLE:
            return mpData[index].mdValue;
            break;
        default:
            break;
        }

        return NULL_DOUBLE;
    }

    virtual const char* String(size_t index) const
    {
        if(index > mnDataUsed)
        {
            return NULL_STR.c_str();
        }

        switch(mpData[index].nType)
        {
        case DT_STRING:
            return mpBuffer + mpData[index].mnstrValue;
            break;
        default:
            break;
        }

        return NULL_STR.c_str();
    }

    virtual AFGUID Object(size_t index) const
    {
        if(index > mnDataUsed)
        {
            return NULL_GUID;
        }

        switch(mpData[index].nType)
        {
        case DT_OBJECT:
            return AFGUID(mpData[index].mnHigh, mpData[index].mnLow);
            break;
        default:
            break;
        }

        return NULL_GUID;
    }

    virtual void* Pointer(size_t index) const
    {
        if(index > mnDataUsed)
        {
            return NULL;
        }

        switch(mpData[index].nType)
        {
        case DT_STRING:
            return mpData[index].mpVaule;
            break;
        default:
            break;
        }

        return NULL;
    }

    virtual const void* UserData(size_t index, size_t& size) const
    {
        if(index > mnDataUsed)
        {
            size = 0;
            return NULL;
        }

        switch(mpData[index].nType)
        {
        case DT_USERDATA:
            {
                char* p = mpBuffer + mpData[index].mnUserData;
                size = AFIData::GetUserDataSize(p);
                return AFIData::GetUserData(p);
            }
            break;
        default:
            break;
        }

        size = 0;
        return NULL;
    }
    virtual void* RawUserData(size_t index) const
    {
        if(index > mnDataUsed)
        {
            return NULL;
        }

        switch(mpData[index].nType)
        {
        case DT_USERDATA:
            {
                return mpBuffer + mpData[index].mnUserData;
            }
            break;
        default:
            break;
        }

        return NULL;
    }

    virtual bool SetBool(size_t index, bool value)
    {
        if(index >= mnDataUsed)
        {
            return false;
        }

        if(mpData[index].nType != DT_BOOLEAN)
        {
            return false;
        }

        mpData[index].mbValue = value;
        return true;
    }

    virtual bool SetInt(size_t index, int value)
    {
        if(index >= mnDataUsed)
        {
            return false;
        }

        if(mpData[index].nType != DT_INT)
        {
            return false;
        }

        mpData[index].mnValue = value;
        return true;
    }

    virtual bool SetInt64(size_t index, int64_t value)
    {
        if(index >= mnDataUsed)
        {
            return false;
        }

        if(mpData[index].nType != DT_INT64)
        {
            return false;
        }

        mpData[index].mn64Value = value;
        return true;
    }

    virtual bool SetFloat(size_t index, float value)
    {
        if(index >= mnDataUsed)
        {
            return false;
        }

        if(mpData[index].nType != DT_FLOAT)
        {
            return false;
        }

        mpData[index].mfValue = value;
        return true;
    }

    virtual bool SetDouble(size_t index, double value)
    {
        if(index >= mnDataUsed)
        {
            return false;
        }

        if(mpData[index].nType != DT_DOUBLE)
        {
            return false;
        }

        mpData[index].mdValue = value;
        return true;
    }

    virtual bool SetString(size_t index, const char* value)
    {
        assert(value != nullptr);

        if(index >= mnDataUsed)
        {
            return false;
        }

        if(mpData[index].nType != DT_STRING)
        {
            return false;
        }

        char* p = mpBuffer + mpData[index].mnstrValue;
        const size_t size1 = strlen(value) + 1;

        if(size1 <= (strlen(p) + 1))
        {
#if ARK_PLATFORM == PLATFORM_WIN
            strncpy_s(p, (strlen(p) + 1), value, size1);
#else
            strncpy(p, value, size1);
#endif
            return true;
        }

        mpData[index].mnstrValue = mnDataUsed;
        const size_t value_size = strlen(value) + 1;
        char* v = AddBuffer(value_size);
        memcpy(v, value, value_size);

        return true;
    }

    virtual bool SetObject(size_t index, const AFGUID& value)
    {
        if(index >= mnDataUsed)
        {
            return false;
        }

        if(mpData[index].nType != DT_OBJECT)
        {
            return false;
        }

        mpData[index].mnHigh = value.nHigh;
        mpData[index].mnLow = value.nLow;
        return true;
    }

    virtual bool SetPointer(size_t index, void* value)
    {
        if(index >= mnDataUsed)
        {
            return false;
        }

        if(mpData[index].nType != DT_POINTER)
        {
            return false;
        }

        mpData[index].mpVaule = value;
        return true;
    }

    virtual const std::string ToString(size_t index)
    {
        if(index > mnDataUsed)
        {
            return "";
        }

        std::string strdata;
        bool ret = true;
        switch(mpData[index].nType)
        {
        case DT_BOOLEAN:
            ret = AFMisc::ARK_TO_STR(strdata, mpData[index].mbValue);
            break;
        case DT_INT:
            ret = AFMisc::ARK_TO_STR(strdata, mpData[index].mnValue);
            break;
        case DT_INT64:
            ret = AFMisc::ARK_TO_STR(strdata, mpData[index].mn64Value);
            break;
        case DT_FLOAT:
            ret = AFMisc::ARK_TO_STR(strdata, mpData[index].mfValue);
            break;
        case DT_DOUBLE:
            ret = AFMisc::ARK_TO_STR(strdata, mpData[index].mdValue);
            break;
        case DT_STRING:
            strdata = String(index);
            break;
        case DT_OBJECT:
            strdata = Object(index).ToString();
            break;
        case DT_POINTER:
            break;
        case DT_USERDATA:
            break;
        default:
            assert(0);
            break;
        }

        ARK_ASSERT_RET_VAL(ret, NULL_STR);

        return strdata;
    }

    virtual size_t GetMemUsage() const
    {
        size_t size = sizeof(self_t);
        if(mnDataSize > DATA_SIZE)
        {
            size += sizeof(dynamic_data_t) * mnDataSize;
        }

        if(mnBufferSize > BUFFER_SIZE)
        {
            size += sizeof(char) * mnBufferSize;
        }

        return size;
    }

protected:
    dynamic_data_t* AddDynamicData()
    {
        if(mnDataUsed >= mnDataSize)
        {
            size_t new_size = mnDataSize * 2;
            dynamic_data_t* p = (dynamic_data_t*)mxAlloc.Alloc(new_size * sizeof(dynamic_data_t));
            memcpy(p, mpData, mnDataUsed * sizeof(dynamic_data_t));
            if(mnDataSize > DATA_SIZE)
            {
                mxAlloc.Free(mpData, mnDataSize * sizeof(dynamic_data_t));
            }

            mpData = p;
            mnDataSize = new_size;
        }

        return mpData + mnDataUsed++;
    }

    char* AddBuffer(size_t need_size)
    {
        size_t new_used = mnBufferUsed + need_size;
        if(new_used > mnBufferSize)
        {
            size_t new_size = mnBufferSize * 2;
            if(new_used > new_size)
            {
                new_size = new_used * 2;
            }

            char* p = (char*)mxAlloc.Alloc(new_size);
            memcpy(p, mpBuffer, mnBufferUsed);

            if(mnBufferSize > BUFFER_SIZE)
            {
                mxAlloc.Free(mpBuffer, mnBufferSize);
            }

            mpBuffer = p;
            mnBufferSize = new_size;
        }

        char* ret = mpBuffer + mnBufferUsed;
        mnBufferUsed = new_used;
        return ret;
    }

    void InnerAppend(const AFIData& data)
    {
        bool bRet(false);
        switch(data.GetType())
        {
        case DT_BOOLEAN:
            bRet = AddBool(data.GetBool());
            break;
        case DT_INT:
            bRet = AddInt(data.GetInt());
            break;
        case DT_INT64:
            bRet = AddInt64(data.GetInt64());
            break;
        case DT_FLOAT:
            bRet = AddFloat(data.GetFloat());
            break;
        case DT_DOUBLE:
            bRet = AddDouble(data.GetDouble());
            break;
        case DT_STRING:
            bRet = AddString(data.GetString());
            break;
        case DT_OBJECT:
            bRet = AddObject(data.GetObject());
            break;
        case DT_POINTER:
            bRet = AddPointer(data.GetPointer());
            break;
        case DT_USERDATA:
            {
                size_t size;
                const void* pData = data.GetUserData(size);
                bRet = AddUserData(pData, size);
            }
            break;
        default:
            assert(0);
            break;
        }

        if(!bRet)
        {
            //
        }

    }

    bool InnerAppend(const AFIDataList& src, size_t start, size_t end)
    {
        for(size_t i = start; i < end; ++i)
        {
            bool bRet(false);
            switch(src.GetType(i))
            {
            case DT_BOOLEAN:
                bRet = AddBool(src.Bool(i));
                break;
            case DT_INT:
                bRet = AddInt(src.Int(i));
                break;
            case DT_INT64:
                bRet = AddInt64(src.Int64(i));
                break;
            case DT_FLOAT:
                bRet = AddFloat(src.Float(i));
                break;
            case DT_DOUBLE:
                bRet = AddDouble(src.Double(i));
                break;
            case DT_STRING:
                bRet = AddString(src.String(i));
                break;
            case DT_OBJECT:
                bRet = AddObject(src.Object(i));
                break;
            case DT_POINTER:
                bRet = AddPointer(src.Pointer(i));
                break;
            case DT_USERDATA:
                {
                    size_t size;
                    const void* pData = src.UserData(i, size);
                    bRet = AddUserData(pData, size);
                }
                break;
            default:
                ARK_ASSERT_NO_EFFECT(0);
                break;
            }

            return bRet;
        }
    }

private:
    ALLOC mxAlloc;
    dynamic_data_t mDataStack[DATA_SIZE];
    dynamic_data_t* mpData;
    size_t mnDataSize;
    size_t mnDataUsed;
    char mBufferStack[BUFFER_SIZE];
    char* mpBuffer;
    size_t mnBufferSize;
    size_t mnBufferUsed;
};

using AFCDataList = AFBaseDataList<8, 128>;