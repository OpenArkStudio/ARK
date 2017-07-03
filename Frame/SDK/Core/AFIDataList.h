#pragma once

#include <string>
#include "AFGUID.h"
#include "AFString.hpp"

namespace ArkFrame
{

class AFIDataList
{
public:
    virtual ~AFIDataList() {}

    virtual bool Concat(const AFIDataList& src) = 0;

    virtual bool Append(const AFIData& data) = 0;
    virtual bool Append(const AFIDataList& src, size_t start, size_t count) = 0;

    virtual void Clear() = 0;

    virtual bool Empty() const = 0;

    virtual size_t GetCount() const = 0;

    virtual int GetType(size_t index) const = 0;

    //add data
    virtual bool AddBool(bool value) = 0;
    virtual bool AddInt(int value) = 0;
    virtual bool AddInt64(int64_t value) = 0;
    virtual bool AddFloat(float value) = 0;
    virtual bool AddDouble(double value) = 0;
    virtual bool AddString(const char* value) = 0;
    virtual bool AddObject(const AFGUID& value) = 0;
    virtual bool AddPointer(void* value) = 0;
    virtual bool AddUserData(const void* pData, size_t size) = 0;
    virtual bool AddRawUserData(void* value) = 0;

    //get data
    virtual bool Bool(size_t index) const = 0;
    virtual int Int(size_t index) const = 0;
    virtual int64_t Int64(size_t index) const = 0;
    virtual float Float(size_t index) const = 0;
    virtual double Double(size_t index) const = 0;
    virtual const char* String(size_t index) const = 0;
    virtual AFGUID Object(size_t index) const = 0;
    virtual void* Pointer(size_t index) const = 0;
    virtual const void* UserData(size_t index, size_t& size) const = 0;
    virtual void* RawUserData(size_t index) const = 0;

    //get memory usage
    virtual size_t GetMemUsage() const = 0;

    //operator <<
    inline AFIDataList& operator<<(bool value)
    {
        AddBool(value);
        return *this;
    }

    inline AFIDataList& operator<<(int8_t value)
    {
        AddInt(value);
        return *this;
    }

    inline AFIDataList& operator<<(uint8_t value)
    {
        AddInt(value);
        return *this;
    }

    inline AFIDataList& operator<<(int16_t value)
    {
        AddInt(value);
        return *this;
    }

    inline AFIDataList& operator<<(uint16_t value)
    {
        AddInt(value);
        return *this;
    }

    inline AFIDataList& operator<<(int32_t value)
    {
        AddInt(value);
        return *this;
    }

    inline AFIDataList& operator<<(uint32_t value)
    {
        AddInt(value);
        return *this;
    }
    
    inline AFIDataList& operator<<(long value)
    {
#if NF_PLATFORM == NF_PLATFORM_WIN || defined(WIN32)
        AddInt(value);
#else
        AddInt64(value);
#endif
        return *this;
    }

    inline AFIDataList& operator<<(unsigned long value)
    {
#if NF_PLATFORM == NF_PLATFORM_WIN || defined(WIN32)
        AddInt(value);
#else
        AddInt64(value);
#endif
        return *this;
    }

    inline AFIDataList& operator<<(int64_t value)
    {
        AddInt64(value);
        return *this;
    }

    inline AFIDataList& operator<<(uint64_t value)
    {
        AddInt64(value);
        return *this;
    }

    inline AFIDataList& operator<<(float value)
    {
        AddFloat(value);
        return *this;
    }

    inline AFIDataList& operator<<(double value)
    {
        AddDouble(value);
        return *this;
    }
    inline AFIDataList& operator<<(const char* value)
    {
        AddString(value);
        return *this;
    }
    inline AFIDataList& operator<<(const std::string& value)
    {
        AddString(value.c_str());
        return *this;
    }

    inline AFIDataList& operator<<(const AFString<char, 128>& value)
    {
        AddString(value.c_str());
        return *this;
    }

    inline AFIDataList& operator<<(const AFGUID& value)
    {
        AddObject(value);
        return *this;
    }

    inline AFIDataList& operator<<(const AFIDataList& value)
    {
        Concat(value);
        return *this;
    }
};

}