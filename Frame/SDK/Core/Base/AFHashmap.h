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

#include "AFPlatform.hpp"

template <typename T, typename TD>
class AFHashmap
{
public:
    typedef std::unordered_map<T, ARK_SHARE_PTR<TD> > HashmapObject;
    AFHashmap()
    {
        mnPos = 0;
        ++mnHashmapCount;
    };

    virtual ~AFHashmap()
    {
        --mnHashmapCount;
    };

    bool AddElement(const T& name, const ARK_SHARE_PTR<TD> data)
    {
        typename HashmapObject::iterator itr = mxObjectList.find(name);

        if (itr == mxObjectList.end())
        {
            mxObjectList.insert(typename HashmapObject::value_type(name, data));
            return true;
        }

        return false;
    }

    bool SetElement(const T& name, const ARK_SHARE_PTR<TD> data)
    {
        mxObjectList[name] = data;
        return true;
    }

    bool RemoveElement(const T& name)
    {
        ARK_SHARE_PTR<TD> pData;
        typename HashmapObject::iterator itr = mxObjectList.find(name);

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
        typename HashmapObject::iterator itr = mxObjectList.find(name);

        if (itr != mxObjectList.end())
        {
            return itr->second.get();
        }

        return nullptr;
    }

    ARK_SHARE_PTR<TD> GetElement(const T& name)
    {
        typename HashmapObject::iterator itr = mxObjectList.find(name);

        if (itr != mxObjectList.end())
        {
            return itr->second;
        }

        return nullptr;
    }

    bool ExitElement(const T& name)
    {
        typename HashmapObject::iterator itr = mxObjectList.find(name);
        return (itr != mxObjectList.end());
    }

    ARK_SHARE_PTR<TD> First()
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

    ARK_SHARE_PTR<TD> Next()
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

    ARK_SHARE_PTR<TD> First(T& name)
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

    ARK_SHARE_PTR<TD> Next(T& name)
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

    ARK_SHARE_PTR<TD> Suit()
    {
        T t;
        return Suit(t);
    }

    ARK_SHARE_PTR<TD> Suit(T& name)
    {
        if (Count() <= 0)
        {
            return nullptr;
        }

        mnPos = (mnPos + 1) % Count();
        typename HashmapObject::iterator it = mxObjectList.begin();
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
    static int mnHashmapCount;

private:
    int mnPos;
    HashmapObject mxObjectList;
    typename HashmapObject::iterator mxObjectCurIter;
};

template <typename T, typename TD>
int AFHashmap<T, TD>::mnHashmapCount = 0;