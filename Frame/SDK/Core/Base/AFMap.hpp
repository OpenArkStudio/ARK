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

#include "SDK/Core/Base/AFPlatform.hpp"
#include "SDK/Core/Base/AFMacros.hpp"


template <typename TD, bool NEEDSMART>
struct MapSmartPtrType
{
    typedef TD* VALUE;
};

template <typename TD>
struct MapSmartPtrType<TD, true>
{
    typedef ARK_SHARE_PTR<TD> VALUE;
};

template <typename TD>
struct MapSmartPtrType<TD, false>
{
    typedef TD* VALUE;
};

template <typename T, typename TD, bool NEEDSMART>
class AFMapBase
{
public:
    typedef typename MapSmartPtrType<TD, NEEDSMART>::VALUE PTRTYPE;
    typedef std::map<T, PTRTYPE > MAP_DATA;
    AFMapBase() = default;
    virtual ~AFMapBase() = default;

    virtual bool AddElement(const T& name, const PTRTYPE data)
    {
        typename MAP_DATA::iterator iter = mxObjectList.find(name);
        if(iter == mxObjectList.end())
        {
            mxObjectList.insert(typename MAP_DATA::value_type(name, data));
            return true;
        }

        return false;
    }

    virtual bool SetElement(const T& name, const PTRTYPE data)
    {
        mxObjectList[name] = data;
        return true;
    }

    virtual bool RemoveElement(const T& name)
    {
        PTRTYPE pData;
        typename MAP_DATA::iterator iter = mxObjectList.find(name);
        if(iter != mxObjectList.end())
        {
            pData = iter->second;
            mxObjectList.erase(iter);

            return true;
        }

        return false;
    }

    virtual const PTRTYPE& GetElement(const T& name)
    {
        typename MAP_DATA::iterator iter = mxObjectList.find(name);
        if(iter != mxObjectList.end())
        {
            return iter->second;
        }

        return mNullPtr;
    }

    virtual const PTRTYPE& First()
    {
        if(mxObjectList.size() <= 0)
        {
            return mNullPtr;
        }

        mxObjectCurIter = mxObjectList.begin();
        if(mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second;
        }

        return mNullPtr;
    }

    virtual const PTRTYPE&  Next()
    {
        if(mxObjectCurIter == mxObjectList.end())
        {
            return mNullPtr;
        }

        ++mxObjectCurIter;
        if(mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second;
        }

        return mNullPtr;
    }

    virtual const PTRTYPE& First(T& name)
    {
        if(mxObjectList.size() <= 0)
        {
            return mNullPtr;
        }

        mxObjectCurIter = mxObjectList.begin();
        if(mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second;
        }

        return mNullPtr;
    }
    virtual const PTRTYPE&  Next(T& name)
    {
        if(mxObjectCurIter == mxObjectList.end())
        {
            return mNullPtr;
        }

        mxObjectCurIter++;
        if(mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second;
        }

        return mNullPtr;
    }

    virtual  TD* FirstNude()
    {
        return nullptr;
    }

    virtual  TD*  NextNude()
    {
        return nullptr;
    }

    int GetCount()
    {
        return (int)mxObjectList.size();
    }

    bool ClearAll()
    {
        mxObjectList.clear();
        return true;
    }

protected:
    MAP_DATA mxObjectList;
    typename MAP_DATA::iterator mxObjectCurIter;
    const PTRTYPE mNullPtr = nullptr;
};

template <typename T, typename TD>
class AFMapEx: public AFMapBase<T, TD, true >
{
public:
    virtual  TD* FirstNude()
    {
        mxObjectCurIter = mxObjectList.begin();
        if(mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second.get();
        }

        return nullptr;
    }

    virtual  TD*  NextNude()
    {
        if(mxObjectCurIter == mxObjectList.end())
        {
            return nullptr;
        }

        mxObjectCurIter++;
        if(mxObjectCurIter == mxObjectList.end())
        {
            return nullptr;
        }

        return mxObjectCurIter->second.get();
    }
};

template <typename T, typename TD>
class AFMap: public AFMapBase<T, TD, false >
{
public:
    virtual  TD* FirstNude()
    {
        return First();
    }

    virtual  TD*  NextNude()
    {
        return Next();
    }
};
