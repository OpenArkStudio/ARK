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

#include "ark/kernel/data.hpp"
#include "ark/kernel/data_list.hpp"
#include "ark/misc.hpp"

namespace ark {

class data_list_alloc
{
public:
    void* alloc(size_t size) {
        ARK_NEW_ARRAY_RET(char, size);
    }

    void free(void* ptr, size_t size) {
        ARK_DELETE_ARRAY(char, ptr);
    }

    void swap(data_list_alloc& src) {
        // Do nothing
    }
};

template<size_t DATA_SIZE, size_t BUFFER_SIZE, typename ALLOC = data_list_alloc>
class base_data_list final : public data_list
{
private:
    using self_t = base_data_list<DATA_SIZE, BUFFER_SIZE, ALLOC>;

    struct dynamic_data_t
    {
        data_type nType;

        union
        {
            bool mbValue;
            int mnValue;
            uint32_t mnUValue;
            int64_t mn64Value;
            uint64_t mnU64Value;
            float mfValue;
            double mdValue;
            size_t mnstrValue;
            void* mpVaule;
            size_t mnUserData;
            guid_t mxGUID;
        };
    };

public:
    base_data_list() {
        assert(DATA_SIZE > 0);
        assert(BUFFER_SIZE > 0);

        mpData = mDataStack;
        mnDataSize = DATA_SIZE;
        mnDataUsed = 0;

        mpBuffer = mBufferStack;
        mnBufferSize = BUFFER_SIZE;
        mnBufferUsed = 0;
    }

    base_data_list(const char* strSour, int nLengh, char strSplit) {
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

        for (int i = 0; i < nLengh; i++)
        {
            if (strSour[i] == strSplit)
            {
                nEnd = i;

                if (!add_string(&strSour[nBegin], nEnd - nBegin))
                {
                    ARK_ASSERT_NO_EFFECT(0);
                }

                if (i + 1 < nLengh)
                {
                    nBegin = i + 1;
                }
            }
        }

        if (nEnd < nLengh)
        {
            nEnd = nLengh;

            if (!add_string(&strSour[nBegin], nEnd - nBegin))
            {
                ARK_ASSERT_NO_EFFECT(0);
            }
        }
    }

    base_data_list(const self_t& src) {
        assert(DATA_SIZE > 0);
        assert(BUFFER_SIZE > 0);

        mpData = mDataStack;
        mnDataSize = DATA_SIZE;
        mnDataUsed = 0;

        mpBuffer = mBufferStack;
        mnBufferSize = BUFFER_SIZE;
        mnBufferUsed = 0;
        inner_append(src, 0, src.get_count());
    }

    base_data_list(const data_list& src) {
        assert(DATA_SIZE > 0);
        assert(BUFFER_SIZE > 0);

        mpData = mDataStack;
        mnDataSize = DATA_SIZE;
        mnDataUsed = 0;

        mpBuffer = mBufferStack;
        mnBufferSize = BUFFER_SIZE;
        mnBufferUsed = 0;
        inner_append(src, 0, src.get_count());
    }

    virtual ~base_data_list() {
        release();
    }

    self_t operator=(const self_t& src) {
        release();

        mpData = mDataStack;
        mnDataSize = DATA_SIZE;
        mnDataUsed = 0;

        mpBuffer = mBufferStack;
        mnBufferSize = BUFFER_SIZE;
        mnBufferUsed = 0;
        inner_append(src, 0, src.get_count());

        return *this;
    }

    void release() {
        if (mnDataSize > DATA_SIZE)
        {
            mxAlloc.free(mpData, mnDataSize * sizeof(dynamic_data_t));
        }

        if (mnBufferSize > BUFFER_SIZE)
        {
            mxAlloc.free(mpBuffer, mnBufferSize * sizeof(char));
        }
    }

    bool concat(const data_list& src) override {
        inner_append(src, 0, src.get_count());
        return true;
    }

    bool split(const std::string& src, const std::string& split) override {
        clear();

        std::string tmp(src);
        if (tmp.empty())
        {
            return true;
        }

        std::string tmp_split(split);
        std::string::size_type pos;
        tmp += tmp_split;
        std::string::size_type size = tmp.length();

        for (std::string::size_type i = 0; i < size; ++i)
        {
            pos = int(tmp.find(tmp_split, i));
            if (pos < size)
            {
                std::string sub = tmp.substr(i, pos - i);
                add_string(sub.c_str());

                i = pos + tmp_split.size() - 1;
            }
        }

        return true;
    }

    bool append(const data& data) override {
        inner_append(data);
        return true;
    }

    bool append(const data_list& src, size_t start, size_t count) override {
        if (start >= src.get_count())
        {
            return false;
        }

        size_t end = start + count;

        if (end > src.get_count())
        {
            return false;
        }

        inner_append(src, start, end);
        return true;
    }

    void clear() override {
        mnDataUsed = 0;
        mnBufferUsed = 0;
    }

    bool empty() const override {
        return (0 == mnDataUsed);
    }

    size_t get_count() const override {
        return mnDataUsed;
    }

    data_type get_type(size_t index) const override {
        if (index >= mnDataUsed)
        {
            return data_type::DT_EMPTY;
        }

        return mpData[index].nType;
    }

    // add data
    bool add_bool(bool value) override {
        dynamic_data_t* p = add_dynamic_data();
        p->nType = data_type::DT_BOOLEAN;
        p->mbValue = value;
        return true;
    }

    bool add_int(int value) override {
        dynamic_data_t* p = add_dynamic_data();
        p->nType = data_type::DT_INT32;
        p->mnValue = value;
        return true;
    }

    bool add_int64(int64_t value) override {
        dynamic_data_t* p = add_dynamic_data();
        p->nType = data_type::DT_INT64;
        p->mn64Value = value;
        return true;
    }

    bool add_u_int(uint32_t value) override {
        dynamic_data_t* p = add_dynamic_data();
        p->nType = data_type::DT_UINT32;
        p->mnUValue = value;
        return true;
    }

    bool add_u_int64(uint64_t value) override {
        dynamic_data_t* p = add_dynamic_data();
        p->nType = data_type::DT_UINT64;
        p->mnU64Value = value;
        return true;
    }

    bool add_float(float value) override {
        dynamic_data_t* p = add_dynamic_data();
        p->nType = data_type::DT_FLOAT;
        p->mfValue = value;
        return true;
    }

    bool add_double(double value) override {
        dynamic_data_t* p = add_dynamic_data();
        p->nType = data_type::DT_DOUBLE;
        p->mdValue = value;
        return true;
    }

    bool add_string(const char* value) override {
        assert(value != nullptr);
        dynamic_data_t* p = add_dynamic_data();
        p->nType = data_type::DT_STRING;
        p->mnstrValue = mnBufferUsed;

        const size_t value_size = strlen(value) + 1;
        char* data = add_buffer(value_size);
        memcpy(data, value, value_size);

        return true;
    }

    virtual bool add_string(const char* value, const int nLength) {
        if (nLength <= 0)
        {
            return false;
        }

        assert(value != nullptr);
        dynamic_data_t* p = add_dynamic_data();

        if (nullptr == p)
        {
            return false;
        }

        p->nType = data_type::DT_STRING;
        p->mnstrValue = mnBufferUsed;

        size_t value_size = strlen(value);

        if (value_size > (size_t)nLength)
        {
            value_size = (size_t)nLength;
        }

        value_size += 1;
        char* data = add_buffer(value_size);
        memcpy(data, value, value_size);

        return true;
    }

    bool add_pointer(void* value) override {
        dynamic_data_t* p = add_dynamic_data();
        p->nType = data_type::DT_POINTER;
        p->mpVaule = value;
        return true;
    }

    bool add_user_data(const void* pData, size_t size) override {
        assert(pData != nullptr);

        dynamic_data_t* p = add_dynamic_data();
        p->nType = data_type::DT_USERDATA;
        p->mnUserData = mnBufferUsed;

        const size_t value_size = data::get_raw_user_data_size(size);
        char* value = add_buffer(value_size);
        data::init_raw_user_data(value, pData, size);

        return true;
    }

    bool add_raw_user_data(void* value) override {
        return add_user_data(data::get_user_data(value), data::get_user_data_size(value));
    }

    // get data
    bool get_bool(size_t index) const override {
        if (index > mnDataUsed)
        {
            return NULL_BOOLEAN;
        }

        if (mpData[index].nType == data_type::DT_BOOLEAN)
        {
            return mpData[index].mbValue;
        }
        else
        {
            return NULL_BOOLEAN;
        }
    }

    int get_int(size_t index) const override {
        if (index > mnDataUsed)
        {
            return NULL_INT;
        }

        if (mpData[index].nType == data_type::DT_INT32)
        {
            return mpData[index].mnValue;
        }
        else
        {
            return NULL_INT;
        }
    }

    int64_t int64(size_t index) const override {
        if (index > mnDataUsed)
        {
            return NULL_INT64;
        }

        if (mpData[index].nType == data_type::DT_INT64)
        {
            return mpData[index].mn64Value;
        }
        else
        {
            return NULL_INT64;
        }
    }

    uint32_t u_int(size_t index) const override {
        if (index > mnDataUsed)
        {
            return NULL_INT;
        }

        if (mpData[index].nType == data_type::DT_UINT32)
        {
            return mpData[index].mnUValue;
        }
        else
        {
            return NULL_INT;
        }
    }

    uint64_t u_int64(size_t index) const override {
        if (index > mnDataUsed)
        {
            return NULL_INT64;
        }

        if (mpData[index].nType == data_type::DT_UINT64)
        {
            return mpData[index].mnU64Value;
        }
        else
        {
            return NULL_INT64;
        }
    }

    float get_float(size_t index) const override {
        if (index > mnDataUsed)
        {
            return NULL_FLOAT;
        }

        if (mpData[index].nType == data_type::DT_FLOAT)
        {
            return mpData[index].mfValue;
        }
        else
        {
            return NULL_FLOAT;
        }
    }

    double get_double(size_t index) const override {
        if (index > mnDataUsed)
        {
            return NULL_DOUBLE;
        }

        if (mpData[index].nType == data_type::DT_DOUBLE)
        {
            return mpData[index].mdValue;
        }
        else
        {
            return NULL_DOUBLE;
        }
    }

    const char* string(size_t index) const override {
        if (index > mnDataUsed)
        {
            return NULL_STR.c_str();
        }

        if (mpData[index].nType == data_type::DT_STRING)
        {
            return mpBuffer + mpData[index].mnstrValue;
        }
        else
        {
            return NULL_STR.c_str();
        }
    }

    void* pointer(size_t index) const override {
        if (index > mnDataUsed)
        {
            return nullptr;
        }

        if (mpData[index].nType == data_type::DT_POINTER)
        {
            return mpData[index].mpVaule;
        }
        else
        {
            return nullptr;
        }
    }

    const void* user_data(size_t index, size_t& size) const override {
        if (index > mnDataUsed)
        {
            size = 0;
            return nullptr;
        }

        if (mpData[index].nType == data_type::DT_USERDATA)
        {
            char* p = mpBuffer + mpData[index].mnUserData;
            size = data::get_user_data_size(p);
            return data::get_user_data(p);
        }
        else
        {
            size = 0;
            return nullptr;
        }
    }

    void* raw_user_data(size_t index) const override {
        if (index > mnDataUsed)
        {
            return nullptr;
        }

        if (mpData[index].nType == data_type::DT_USERDATA)
        {
            return mpBuffer + mpData[index].mnUserData;
        }
        else
        {
            return nullptr;
        }
    }

    virtual bool set_bool(size_t index, bool value) {
        if (index >= mnDataUsed)
        {
            return false;
        }

        if (mpData[index].nType != data_type::DT_BOOLEAN)
        {
            return false;
        }
        else
        {
            mpData[index].mbValue = value;
            return true;
        }
    }

    virtual bool set_int(size_t index, int value) {
        if (index >= mnDataUsed)
        {
            return false;
        }

        if (mpData[index].nType != data_type::DT_INT32)
        {
            return false;
        }
        else
        {
            mpData[index].mnValue = value;
            return true;
        }
    }

    virtual bool set_int64(size_t index, int64_t value) {
        if (index >= mnDataUsed)
        {
            return false;
        }

        if (mpData[index].nType != data_type::DT_INT64)
        {
            return false;
        }
        else
        {
            mpData[index].mn64Value = value;
            return true;
        }
    }

    virtual bool set_u_int(size_t index, uint32_t value) {
        if (index >= mnDataUsed)
        {
            return false;
        }

        if (mpData[index].nType != data_type::DT_UINT32)
        {
            return false;
        }
        else
        {
            mpData[index].mnUValue = value;
            return true;
        }
    }

    virtual bool set_u_int64(size_t index, uint64_t value) {
        if (index >= mnDataUsed)
        {
            return false;
        }

        if (mpData[index].nType != data_type::DT_UINT64)
        {
            return false;
        }
        else
        {
            mpData[index].mnU64Value = value;
            return true;
        }
    }

    virtual bool set_float(size_t index, float value) {
        if (index >= mnDataUsed)
        {
            return false;
        }

        if (mpData[index].nType != data_type::DT_FLOAT)
        {
            return false;
        }
        else
        {
            mpData[index].mfValue = value;
            return true;
        }
    }

    virtual bool set_double(size_t index, double value) {
        if (index >= mnDataUsed)
        {
            return false;
        }

        if (mpData[index].nType != data_type::DT_DOUBLE)
        {
            return false;
        }
        else
        {
            mpData[index].mdValue = value;
            return true;
        }
    }

    virtual bool set_string(size_t index, const char* value) {
        assert(value != nullptr);

        if (index >= mnDataUsed)
        {
            return false;
        }

        if (mpData[index].nType != data_type::DT_STRING)
        {
            return false;
        }

        char* p = mpBuffer + mpData[index].mnstrValue;
        const size_t size1 = strlen(value) + 1;

        if (size1 <= (strlen(p) + 1))
        {
            ARK_STRNCPY(p, value, size1);
            return true;
        }

        mpData[index].mnstrValue = mnDataUsed;
        const size_t value_size = strlen(value) + 1;
        char* v = add_buffer(value_size);
        memcpy(v, value, value_size);

        return true;
    }

    virtual bool set_pointer(size_t index, void* value) {
        if (index >= mnDataUsed)
        {
            return false;
        }

        if (mpData[index].nType != data_type::DT_POINTER)
        {
            return false;
        }
        else
        {
            mpData[index].mpVaule = value;
            return true;
        }
    }

    const std::string to_string(size_t index) override {
        if (index > mnDataUsed)
        {
            return NULL_STR;
        }

        std::string data;
        switch (mpData[index].nType)
        {
        case data_type::DT_BOOLEAN:
            data = ARK_TO_STRING(mpData[index].mbValue);
            break;
        case data_type::DT_INT32:
            data = ARK_TO_STRING(mpData[index].mnValue);
            break;
        case data_type::DT_INT64:
            data = ARK_TO_STRING(mpData[index].mn64Value);
            break;
        case data_type::DT_UINT32:
            data = ARK_TO_STRING(mpData[index].mnUValue);
            break;
        case data_type::DT_UINT64:
            data = ARK_TO_STRING(mpData[index].mnU64Value);
            break;
        case data_type::DT_FLOAT:
            data = ARK_TO_STRING(mpData[index].mfValue);
            break;
        case data_type::DT_DOUBLE:
            data = ARK_TO_STRING(mpData[index].mdValue);
            break;
        case data_type::DT_STRING:
            data = string(index);
            break;
        case data_type::DT_POINTER:
            break;
        case data_type::DT_USERDATA:
            break;
        default:
            assert(0);
            break;
        }

        return data;
    }

    size_t get_mem_usage() const override {
        size_t size = sizeof(self_t);

        if (mnDataSize > DATA_SIZE)
        {
            size += sizeof(dynamic_data_t) * mnDataSize;
        }

        if (mnBufferSize > BUFFER_SIZE)
        {
            size += sizeof(char) * mnBufferSize;
        }

        return size;
    }

protected:
    dynamic_data_t* add_dynamic_data() {
        if (mnDataUsed >= mnDataSize)
        {
            size_t new_size = mnDataSize * 2;
            dynamic_data_t* p = (dynamic_data_t*)mxAlloc.alloc(new_size * sizeof(dynamic_data_t));
            memcpy(p, mpData, mnDataUsed * sizeof(dynamic_data_t));

            if (mnDataSize > DATA_SIZE)
            {
                mxAlloc.free(mpData, mnDataSize * sizeof(dynamic_data_t));
            }

            mpData = p;
            mnDataSize = new_size;
        }

        return mpData + mnDataUsed++;
    }

    char* add_buffer(size_t need_size) {
        size_t new_used = mnBufferUsed + need_size;

        if (new_used > mnBufferSize)
        {
            size_t new_size = mnBufferSize * 2;

            if (new_used > new_size)
            {
                new_size = new_used * 2;
            }

            char* p = (char*)mxAlloc.alloc(new_size);
            memcpy(p, mpBuffer, mnBufferUsed);

            if (mnBufferSize > BUFFER_SIZE)
            {
                mxAlloc.free(mpBuffer, mnBufferSize);
            }

            mpBuffer = p;
            mnBufferSize = new_size;
        }

        char* ret = mpBuffer + mnBufferUsed;
        mnBufferUsed = new_used;
        return ret;
    }

    void inner_append(const data& data) {
        bool bRet(false);

        switch (data.get_type())
        {
        case data_type::DT_BOOLEAN:
            bRet = add_bool(data.get_bool());
            break;
        case data_type::DT_INT32:
            bRet = add_int(data.get_int());
            break;
        case data_type::DT_INT64:
            bRet = add_int64(data.get_int64());
            break;
        case data_type::DT_UINT32:
            bRet = add_u_int(data.get_u_int());
            break;
        case data_type::DT_UINT64:
            bRet = add_u_int64(data.get_u_int64());
            break;
        case data_type::DT_FLOAT:
            bRet = add_float(data.get_float());
            break;
        case data_type::DT_DOUBLE:
            bRet = add_double(data.get_double());
            break;
        case data_type::DT_STRING:
            bRet = add_string(data.get_string());
            break;
        case data_type::DT_POINTER:
            bRet = add_pointer(data.get_pointer());
            break;
        case data_type::DT_USERDATA:
        {
            size_t size;
            const void* pData = data.get_user_data(size);
            bRet = add_user_data(pData, size);
        }
        break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }

        ARK_ASSERT_NO_EFFECT(bRet);
    }

    bool inner_append(const data_list& src, size_t start, size_t end) {
        bool bRet(false);

        for (size_t i = start; i < end; ++i)
        {
            switch (src.get_type(i))
            {
            case data_type::DT_BOOLEAN:
                bRet = add_bool(src.get_bool(i));
                break;
            case data_type::DT_INT32:
                bRet = add_int(src.get_int(i));
                break;
            case data_type::DT_INT64:
                bRet = add_int64(src.int64(i));
                break;
            case data_type::DT_UINT32:
                bRet = add_u_int(src.u_int(i));
                break;
            case data_type::DT_UINT64:
                bRet = add_u_int64(src.u_int64(i));
                break;
            case data_type::DT_FLOAT:
                bRet = add_float(src.get_float(i));
                break;
            case data_type::DT_DOUBLE:
                bRet = add_double(src.get_double(i));
                break;
            case data_type::DT_STRING:
                bRet = add_string(src.string(i));
                break;
            case data_type::DT_POINTER:
                bRet = add_pointer(src.pointer(i));
                break;
            case data_type::DT_USERDATA:
            {
                size_t size;
                const void* pData = src.user_data(i, size);
                bRet = add_user_data(pData, size);
            }
            break;
            default:
                ARK_ASSERT_NO_EFFECT(0);
                break;
            }
        }

        return bRet;
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

using data_list_impl = base_data_list<8, 128>;

} // namespace ark
