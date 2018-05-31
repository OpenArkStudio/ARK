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

template <typename T, typename TD>
class AFMapEx
{
public:
    typedef std::map<T, ARK_SHARE_PTR<TD> > MAP_DATA;

    AFMapEx() = default;
    virtual ~AFMapEx() = default;

    virtual bool AddElement(const T& name, const ARK_SHARE_PTR<TD> data)
    {
        typename MAP_DATA::iterator iter = mxObjectList.find(name);
        if (iter == mxObjectList.end())
        {
            mxObjectList.insert(typename MAP_DATA::value_type(name, data));
            return true;
        }

        return false;
    }

    virtual bool SetElement(const T& name, const ARK_SHARE_PTR<TD> data)
    {
        mxObjectList[name] = data;
        return true;
    }

    virtual bool RemoveElement(const T& name)
    {
        ARK_SHARE_PTR<TD> pData;
        typename MAP_DATA::iterator iter = mxObjectList.find(name);
        if (iter != mxObjectList.end())
        {
            pData = iter->second;
            mxObjectList.erase(iter);

            return true;
        }

        return false;
    }

    virtual TD* GetElementNude(const T& name)
    {
        typename MAP_DATA::iterator iter = mxObjectList.find(name);
        if (iter != mxObjectList.end())
        {
            return iter->second.get();
        }
        
        return NULL;
    }

    virtual ARK_SHARE_PTR<TD> GetElement(const T& name)
    {
        typename MAP_DATA::iterator iter = mxObjectList.find(name);
        if (iter != mxObjectList.end())
        {
            return iter->second;
        }
        
        return nullptr;
    }
    virtual TD* FirstNude(T& name)
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

    virtual TD* NextNude(T& name)
    {
        if (mxObjectCurIter == mxObjectList.end())
        {
            return NULL;
        }

        mxObjectCurIter++;
        if (mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second.get();
        }
        
        return NULL;
    }

    virtual TD* FirstNude()
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

    virtual TD* NextNude()
    {
        if (mxObjectCurIter == mxObjectList.end())
        {
            return NULL;
        }

        mxObjectCurIter++;
        if (mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second.get();
        }
        
        return NULL;
    }

    virtual ARK_SHARE_PTR<TD> First()
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

    virtual ARK_SHARE_PTR<TD> Next()
    {
        if (mxObjectCurIter == mxObjectList.end())
        {
            return ARK_SHARE_PTR<TD>();
        }

        ++mxObjectCurIter;
        if (mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second;
        }

        return nullptr;
    }

    virtual ARK_SHARE_PTR<TD> First(T& name)
    {
        if (mxObjectList.size() <= 0)
        {
            return ARK_SHARE_PTR<TD>();
        }

        mxObjectCurIter = mxObjectList.begin();
        if (mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second;
        }

        return nullptr;
    }

    virtual ARK_SHARE_PTR<TD> Next(T& name)
    {
        if (mxObjectCurIter == mxObjectList.end())
        {
            return ARK_SHARE_PTR<TD>();
        }

        mxObjectCurIter++;
        if (mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second;
        }

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

private:
    MAP_DATA mxObjectList;
    typename MAP_DATA::iterator mxObjectCurIter;
};