// -------------------------------------------------------------------------
//    @FileName         :    NFHashmap.h
//    @Author           :    Nick Yang
//    @Date             :    2016/09/18
//    @Module           :    这个是hashmap的智能指针专用,切记，不能在循环中自己删除自己
//
// -------------------------------------------------------------------------

#ifndef NF_HASHMAP_H
#define NF_HASHMAP_H

#include <unordered_map>
#include <string>
#include <memory>
#include "NFPlatform.h"

template <typename T , typename TD>
class NFHashmap
{
public:
    typedef std::unordered_map<T, NF_SHARED_PTR<TD> > NFHashmapObject;
    NFHashmap()
    {
        mnPos = 0;
        ++mnNFHashmapCount;
    };

    virtual ~NFHashmap()
    {
        --mnNFHashmapCount;
    };

    bool AddElement(const T& name, const NF_SHARED_PTR<TD> data)
    {
        typename NFHashmapObject::iterator itr = mxObjectList.find(name);
        if (itr == mxObjectList.end())
        {
            mxObjectList.insert(typename NFHashmapObject::value_type(name, data));
            return true;
        }

        return false;
    }

    bool SetElement(const T& name, const NF_SHARED_PTR<TD> data)
    {
        mxObjectList[name] = data;
        return true;
    }

    bool RemoveElement(const T& name)
    {
        NF_SHARED_PTR<TD> pData;
        typename NFHashmapObject::iterator itr = mxObjectList.find(name);
        if (itr != mxObjectList.end())
        {
            pData = itr->second;
            mxObjectList.erase(itr);
            return true;
        }

        return false;
    }

    TD* GetElementNude(const T& name)
    {
        typename NFHashmapObject::iterator itr = mxObjectList.find(name);
        if (itr != mxObjectList.end())
        {
            return itr->second.get();
        }

        return nullptr;
    }

    NF_SHARED_PTR<TD> GetElement(const T& name)
    {
        typename NFHashmapObject::iterator itr = mxObjectList.find(name);
        if (itr != mxObjectList.end())
        {
            return itr->second;
        }

        return nullptr;
    }

    bool ExitElement(const T& name)
    {
        typename NFHashmapObject::iterator itr = mxObjectList.find(name);
        return (itr != mxObjectList.end());
    }

    NF_SHARED_PTR<TD> First()
    {
        if (mxObjectList.size() <= 0)
        {
            return nullptr;
        }

        mxObjectCurIter = mxObjectList.begin();
        if (mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second;
        }

        return nullptr;
    }

    NF_SHARED_PTR<TD> Next()
    {
        if (mxObjectCurIter == mxObjectList.end())
        {
            return nullptr;
        }

        ++mxObjectCurIter;
        if (mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second;
        }

        return nullptr;
    }

    TD* FirstNude(T& name)
    {
        if (mxObjectList.size() <= 0)
        {
            return NULL;
        }

        mxObjectCurIter = mxObjectList.begin();
        if (mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second.get();
        }

        return NULL;
    }

    TD* NextNude(T& name)
    {
        if (mxObjectCurIter == mxObjectList.end())
        {
            return NULL;
        }

        ++mxObjectCurIter;
        if (mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second.get();
        }

        return NULL;
    }

    TD* FirstNude()
    {
        if (mxObjectList.size() <= 0)
        {
            return NULL;
        }

        mxObjectCurIter = mxObjectList.begin();
        if (mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second.get();
        }

        return NULL;
    }

    TD* NextNude()
    {
        if (mxObjectCurIter == mxObjectList.end())
        {
            return NULL;
        }

        ++mxObjectCurIter;
        if (mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second.get();
        }

        return NULL;
    }

    NF_SHARED_PTR<TD> First(T& name)
    {
        if (mxObjectList.size() <= 0)
        {
            return nullptr;
        }

        mxObjectCurIter = mxObjectList.begin();
        if (mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second;
        }

        return nullptr;
    }

    NF_SHARED_PTR<TD> Next(T& name)
    {
        if (mxObjectCurIter == mxObjectList.end())
        {
            return nullptr;
        }

        mxObjectCurIter++;
        if (mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second;
        }

        return nullptr;
    }

    NF_SHARED_PTR<TD> Suit()
    {
        T t;
        return Suit(t);
    }

    NF_SHARED_PTR<TD> Suit(T& name)
    {
        //不能使用迭代器那套，因为会打乱迭代器顺序
        if (Count() <= 0)
        {
            return nullptr;
        }

        mnPos = (mnPos + 1) % Count();
        typename NFHashmapObject::iterator it = mxObjectList.begin();
        std::advance(it, mnPos);

        name =  it->first;
        return it->second;
    }

    int Count()
    {
        return mxObjectList.size();
    }

    bool ClearAll()
    {
        mxObjectList.clear();
        return true;
    }

public:
    //just for hash-map statistic
    static int mnNFHashmapCount;

private:
    int mnPos;
    NFHashmapObject mxObjectList;
    typename NFHashmapObject::iterator mxObjectCurIter;
};

template <typename T , typename TD>
int NFHashmap<T, TD>::mnNFHashmapCount = 0;

#endif //!NF_HASHMAP_H
