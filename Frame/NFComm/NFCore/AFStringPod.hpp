#pragma once

#include <string>
#include "AFCoreDef.hpp"

//取hash值时区分大小写
template<typename TYPE>
class StringTraits
{
public:
    static size_t hash(const TYPE* value);
    static size_t length(const TYPE* value);
    static bool equal(const TYPE* lvalue, const TYPE* rvalue);
    static void copy(TYPE* dst, const TYPE* src, size_t len);
};

template<>
class StringTraits<char>
{
public:
    static size_t hash(const char* value)
    {
        return GetHashValue(value);
    }

    static size_t length(const char* value)
    {
        return strlen(value);
    }

    static bool equal(const char* lvalue, const char* rvalue)
    {
        return (0 == strcmp(lvalue, rvalue));
    }

    static void copy(char* dst, const char* src, size_t len)
    {
        memcpy(dst, src, len);
    }
};

//取hash值时不区分大小写
template<typename TYPE>
class StringTraitsNoCase : public StringTraits<TYPE>
{

};

template<>
class StringTraitsNoCase<char> : public StringTraits<char>
{
public:
    static size_t hash(const char* value)
    {
        return GetHashValueNoCase(value);
    }

    static bool equal(const char* lvalue, const char* rvalue)
    {
        return (0 == stricmp(lvalue, rvalue));
    }
};
//////////////////////////////////////////////////////////////////////////
class StringPodAlloc
{
public:
    StringPodAlloc() {}
    ~StringPodAlloc() {}

    void* Alloc(size_t size)
    {
        return new char[size];
    }

    void Free(void* ptr, size_t size)
    {
        delete[](char*)ptr;
    }
};

template<typename TYPE, typename DATA>
class StringPodNode
{
public:
    StringPodNode* pNext;
    size_t hash;
    DATA data;
    TYPE name[1];
};

template<typename TYPE, typename DATA, typename TRAITS = StringTraits<TYPE>, typename ALLOC = StringPodAlloc>
class StringPod;

template<typename TYPE, typename DATA, typename TRAITS = StringTraits<TYPE>, typename ALLOC = StringPodAlloc>
class StringPodIter
{
private:
    using hash_t = StringPod<TYPE, DATA, TRAITS, ALLOC>;
    using node_t = StringPodNode<TYPE, DATA>;

public:
    StringPodIter() = delete;

    StringPod(const hash_t* self, node_t* node)
    {
        mpSelf = self;
        mpNode = node;
    }

    StringPodIter& operator++()
    {
        node_t* next = mpNode->pNext;
        if (NULL != next)
        {
            mpNode = next;
        }
        else
        {
            mpNode = mpSelf->ToNext(mpNode);
        }

        return *this;
    }

private:
    const hash_t* mpSelf;
    node_t* mpNode;
};
