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

#include "ark/core_def.hpp"
#include "ark/kernel/data.hpp"
#include "ark/misc.hpp"
#include "ark/platform.hpp"

namespace ark {

class data_alloc_new_type
{
public:
    void* alloc(size_t size) {
        ARK_NEW_ARRAY_RET(char, size);
    }

    void free(void* ptr, size_t size) {
        ARK_DELETE_ARRAY(char, ptr);
    }

    void swap(data_alloc_new_type& src) {
        // do nothing
    }
};

template<size_t BUFFER_SIZE, typename ALLOC = data_alloc_new_type>
class data_base final : public data
{
public:
    using self_t = data_base<BUFFER_SIZE, ALLOC>;

    data_base() {
        mnType = data_type::DT_EMPTY;
        mn64Value = 0;
    }

    ~data_base() override {
        release();
    }

    data_base(const self_t& src) {
        this->mnType = src.mnType;

        switch (this->mnType)
        {
        case data_type::DT_BOOLEAN:
            mbValue = src.mbValue;
            break;
        case data_type::DT_INT32:
            mnValue = src.mnValue;
            break;
        case data_type::DT_INT64:
            mn64Value = src.mn64Value;
            break;
        case data_type::DT_UINT32:
            mnU32Value = src.mnU32Value;
            break;
        case data_type::DT_UINT64:
            mnU64Value = src.mnU64Value;
            break;
        case data_type::DT_FLOAT:
            mfValue = src.mfValue;
            break;
        case data_type::DT_DOUBLE:
            mdValue = src.mdValue;
            break;
        case data_type::DT_STRING:
            inner_set_string(src.get_string());
            break;
        case data_type::DT_POINTER:
            mpVaule = src.mpVaule;
            break;
        case data_type::DT_USERDATA:
            set_user_data(src);
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }
    }

    explicit data_base(const data& src) {
        this->mnType = src.get_type();

        switch (this->mnType)
        {
        case data_type::DT_BOOLEAN:
            mbValue = src.get_bool();
            break;
        case data_type::DT_INT32:
            mnValue = src.get_int();
            break;
        case data_type::DT_INT64:
            mn64Value = src.get_int64();
            break;
        case data_type::DT_UINT32:
            mnU32Value = src.get_u_int();
            break;
        case data_type::DT_UINT64:
            mnU64Value = src.get_u_int64();
            break;
        case data_type::DT_FLOAT:
            mfValue = src.get_float();
            break;
        case data_type::DT_DOUBLE:
            mdValue = src.get_double();
            break;
        case data_type::DT_STRING:
            inner_set_string(src.get_string());
            break;
        case data_type::DT_POINTER:
            mpVaule = src.get_pointer();
            break;
        case data_type::DT_USERDATA:
            set_user_data(src);
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }
    }

    bool from(const node* pData) override {
        if (pData == nullptr)
        {
            return false;
        }

        this->mnType = pData->get_type();

        switch (this->mnType)
        {
        case data_type::DT_BOOLEAN:
            mbValue = pData->get_bool();
            break;
        case data_type::DT_INT32:
            mnValue = pData->get_int32();
            break;
        case data_type::DT_INT64:
            mn64Value = pData->get_int64();
            break;
        case data_type::DT_UINT32:
            mnU32Value = pData->get_u_int32();
            break;
        case data_type::DT_UINT64:
            mnU64Value = pData->get_u_int64();
            break;
        case data_type::DT_FLOAT:
            mfValue = pData->get_float();
            break;
        case data_type::DT_DOUBLE:
            mdValue = pData->get_double();
            break;
        case data_type::DT_STRING:
            inner_set_string(pData->get_string().c_str());
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }

        return true;
    }

    data_base(data_type type, bool value) {
        assert(type == data_type::DT_BOOLEAN);

        mnType = data_type::DT_BOOLEAN;
        mbValue = value;
    }

    data_base(data_type type, int value) {
        assert(type == data_type::DT_INT32);

        mnType = data_type::DT_INT32;
        mnValue = value;
    }

    data_base(data_type type, uint32_t value) {
        assert(type == data_type::DT_UINT32);

        mnType = data_type::DT_UINT32;
        mnValue = value;
    }

    data_base(data_type type, int64_t value) {
        assert(type == data_type::DT_INT64);

        mnType = data_type::DT_INT64;
        mn64Value = value;
    }

    data_base(data_type type, uint64_t value) {
        assert(type == data_type::DT_UINT64);

        mnType = data_type::DT_UINT64;
        mn64Value = value;
    }

    data_base(data_type type, float value) {
        assert(type == data_type::DT_FLOAT);

        mnType = data_type::DT_FLOAT;
        mfValue = value;
    }

    data_base(data_type type, double value) {
        assert(type == data_type::DT_DOUBLE);

        mnType = data_type::DT_DOUBLE;
        mdValue = value;
    }

    data_base(data_type type, const char* value) {
        assert(type == data_type::DT_STRING);

        mnType = data_type::DT_STRING;
        inner_set_string(value);
    }

    data_base(data_type type, const void* value, size_t size) {
        assert(type == data_type::DT_USERDATA);

        mnType = data_type::DT_USERDATA;
        inner_set_user_data(value, size);
    }

    self_t& operator=(const self_t& src) {
        self_t tmp(src);
        swap(tmp);
        return *this;
    }

    self_t& operator=(const data& src) {
        self_t tmp(src);
        swap(tmp);
        return *this;
    }

    void swap(self_t& src) {
        data_type tmp_type = this->mnType;
        int64_t tmp_value = this->mn64Value;
        // guid_t tmp_guid = this->mxGUID;
        uint32_t tmp_alloc_len = this->mnAllocLen;
        char tmp_buffer[BUFFER_SIZE] = {0};
        bool tmp_use_buffer = (tmp_type == data_type::DT_STRING) && (this->mstrValue == this->mBuffer);

        if ((src.mnType == data_type::DT_STRING) && (src.mstrValue == src.mBuffer))
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
        mxAlloc.swap(src.mxAlloc);
    }

    //////////////////////////////////////////////////////////////////////////

    data_type get_type() const override {
        return mnType;
    }

    void set_default_value(data_type type) override {
        switch (type)
        {
        case data_type::DT_BOOLEAN:
            set_bool(NULL_BOOLEAN);
            break;
        case data_type::DT_INT32:
            set_int(NULL_INT);
            break;
        case data_type::DT_INT64:
            set_int64(NULL_INT64);
            break;
        case data_type::DT_UINT32:
            set_u_int(NULL_INT);
            break;
        case data_type::DT_UINT64:
            set_u_int64(NULL_INT64);
            break;
        case data_type::DT_FLOAT:
            set_float(NULL_FLOAT);
            break;
        case data_type::DT_DOUBLE:
            set_double(NULL_DOUBLE);
            break;
        case data_type::DT_STRING:
            set_string(NULL_STR.c_str());
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }
    }

    bool is_null_value() const override {
        switch (get_type())
        {
        case data_type::DT_BOOLEAN:
            return mbValue == NULL_BOOLEAN;
            break;
        case data_type::DT_INT32:
            return mnValue == NULL_INT;
            break;
        case data_type::DT_INT64:
            return mn64Value == NULL_INT64;
            break;
        case data_type::DT_UINT32:
            return mnU32Value == NULL_INT;
            break;
        case data_type::DT_UINT64:
            return mnU64Value == NULL_INT64;
            break;
        case data_type::DT_FLOAT:
            return IS_FLOAT_ZERO(float, mfValue);
            break;
        case data_type::DT_DOUBLE:
            return IS_FLOAT_ZERO(double, mfValue);
            break;
        case data_type::DT_STRING:
            return mstrValue == NULL_STR.c_str();
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }

        return false;
    }

    // Get data
    bool get_bool() const override {
        ARK_ASSERT_RET_VAL(mnType == data_type::DT_BOOLEAN, NULL_BOOLEAN);
        return mbValue;
    }

    int get_int() const override {
        ARK_ASSERT_RET_VAL(mnType == data_type::DT_INT32, NULL_INT);
        return mnValue;
    }

    int64_t get_int64() const override {
        ARK_ASSERT_RET_VAL(mnType == data_type::DT_INT64, NULL_INT64);
        return mn64Value;
    }

    float get_float() const override {
        ARK_ASSERT_RET_VAL(mnType == data_type::DT_FLOAT, NULL_FLOAT);
        return mfValue;
    }

    double get_double() const override {
        ARK_ASSERT_RET_VAL(mnType == data_type::DT_DOUBLE, NULL_DOUBLE);
        return mdValue;
    }

    const char* get_string() const override {
        ARK_ASSERT_RET_VAL(mnType == data_type::DT_STRING, NULL_STR.c_str());
        return mstrValue;
    }

    uint32_t get_u_int() const override {
        ARK_ASSERT_RET_VAL(mnType == data_type::DT_UINT32, NULL_INT);
        return mnU32Value;
    }

    uint64_t get_u_int64() const override {
        ARK_ASSERT_RET_VAL(mnType == data_type::DT_UINT64, NULL_INT64);
        return mnU64Value;
    }

    void* get_pointer() const override {
        ARK_ASSERT_RET_VAL(mnType == data_type::DT_POINTER, nullptr);
        return mpVaule;
    }

    const void* get_user_data(size_t& size) const override {
        assert(mnType == data_type::DT_USERDATA);

        if (mnType != data_type::DT_USERDATA)
        {
            size = 0;
            return nullptr;
        }

        size = data::get_user_data_size(mpUserData);
        return data::get_user_data(mpUserData);
    }

    void* get_raw_user_data() const override {
        ARK_ASSERT_RET_VAL(mnType == data_type::DT_USERDATA, nullptr);
        return mpUserData;
    }

    // Set data
    void set_unknown() override {
        release();
        mnType = data_type::DT_EMPTY;
        mn64Value = 0;
    }

    void set_bool(bool value) override {
        release();
        mnType = data_type::DT_BOOLEAN;
        mbValue = value;
    }

    void set_int(int value) override {
        release();
        mnType = data_type::DT_INT32;
        mnValue = value;
    }

    void set_int64(int64_t value) override {
        release();
        mnType = data_type::DT_INT64;
        mn64Value = value;
    }

    void set_float(float value) override {
        release();
        mnType = data_type::DT_FLOAT;
        mfValue = value;
    }

    void set_double(double value) override {
        release();
        mnType = data_type::DT_DOUBLE;
        mdValue = value;
    }

    void set_string(const char* value) override {
        release();
        mnType = data_type::DT_STRING;
        inner_set_string(value);
    }

    void set_u_int(uint32_t value) override {
        release();
        mnType = data_type::DT_UINT32;
        mnU32Value = value;
    }

    void set_u_int64(uint64_t value) override {
        release();
        mnType = data_type::DT_UINT64;
        mnU64Value = value;
    }

    void set_pointer(void* value) override {
        release();
        mnType = data_type::DT_POINTER;
        mpVaule = value;
    }

    void set_user_data(const void* value, size_t size) override {
        release();
        mnType = data_type::DT_USERDATA;
        inner_set_user_data(value, size);
    }

    void set_raw_user_data(void* value) override {
        set_user_data(data::get_user_data(value), data::get_user_data_size(value));
    }

    void assign(const data& src) override {
        self_t tmp(src);
        swap(tmp);
    }

    size_t get_mem_usage() const override {
        size_t size = sizeof(self_t);

        switch (mnType)
        {
        case data_type::DT_STRING:
        {
            if (mstrValue != nullptr)
            {
                size += strlen(mstrValue) + 1;
            }
        }
        break;
        case data_type::DT_USERDATA:
        {
            if (mpUserData != nullptr)
            {
                size += data::get_user_data_size(mpUserData);
            }
        }
        break;
        default:
            break;
        }

        return size;
    }

    std::string to_string() const override {
        switch (get_type())
        {
        case data_type::DT_BOOLEAN:
            return ARK_TO_STRING(this->mbValue);
            break;
        case data_type::DT_INT32:
            return ARK_TO_STRING(this->mnValue);
            break;
        case data_type::DT_INT64:
            return ARK_TO_STRING(this->mn64Value);
            break;
        case data_type::DT_UINT32:
            return ARK_TO_STRING(this->mnU32Value);
            break;
        case data_type::DT_UINT64:
            return ARK_TO_STRING(this->mnU64Value);
            break;
        case data_type::DT_FLOAT:
            return ARK_TO_STRING(this->mfValue);
            break;
        case data_type::DT_DOUBLE:
            return ARK_TO_STRING(this->mdValue);
            break;
        case data_type::DT_STRING:
            return this->mstrValue;
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }

        return NULL_STR;
    }

    void release() {
        switch (mnType)
        {
        case data_type::DT_STRING:
        {
            if (mstrValue != mBuffer)
            {
                mxAlloc.free(mstrValue, mnAllocLen);
                mstrValue = nullptr;
            }
        }
        break;
        case data_type::DT_USERDATA:
        {
            if (mpUserData != nullptr)
            {
                mxAlloc.free(mpUserData, mnAllocLen);
                mpUserData = nullptr;
            }
        }
        break;
        default:
            break;
        }
    }

protected:
    void inner_set_string(const char* value) {
        const size_t value_size = strlen(value) + 1;
        char* p = nullptr;

        if (value_size > BUFFER_SIZE)
        {
            p = (char*)mxAlloc.alloc(value_size);
            mnAllocLen = (uint32_t)value_size;
        }
        else
        {
            p = mBuffer;
        }

        memcpy(p, value, value_size);
        mstrValue = p;
    }

    void inner_set_user_data(const void* data, size_t size) {
        size_t value_size = get_raw_user_data_size(size);
        char* p = (char*)mxAlloc.alloc(value_size);
        init_raw_user_data(p, data, size);
        mpUserData = p;
        mnAllocLen = (uint32_t)value_size;
    }

    void set_user_data(const self_t& src) {
        size_t size;
        const void* pData = src.get_user_data(size);
        inner_set_user_data(pData, size);
    }

    void set_user_data(const data& src) {
        size_t size;
        const void* pData = src.get_user_data(size);
        inner_set_user_data(pData, size);
    }

private:
    ALLOC mxAlloc;
    data_type mnType;

    // This union size is 8 bytes
    union
    {
        bool mbValue;
        int mnValue;
        uint32_t mnU32Value;
        int64_t mn64Value;
        uint64_t mnU64Value;
        float mfValue;
        double mdValue;
        char* mstrValue;
        void* mpVaule;
        char* mpUserData;
    };

    // buffer
    union
    {
        uint32_t mnAllocLen{0};
        char mBuffer[BUFFER_SIZE];
    };
};

// special
using data_impl = data_base<4, core_allocator>;

} // namespace ark
