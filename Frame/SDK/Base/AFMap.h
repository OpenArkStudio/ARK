/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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
#include "AFMacros.hpp"

template <typename T, typename TD>
class AFMap
{
public:
    typedef std::map<T, TD*> map_data;

    AFMap() {}
    virtual ~AFMap() {}

    virtual bool AddElement(const T& name, TD* data)
    {
        typename map_data::iterator itr = mObjectList.find(name);
        if(itr == mObjectList.end())
        {
            mObjectList.insert(typename map_data::value_type(name, data));
            return true;
        }

        return false;
    }

    virtual TD* RemoveElement(const T& name)
    {
        TD* pData = NULL;
        typename map_data::iterator itr = mObjectList.find(name);
        if(itr != mObjectList.end())
        {
            pData = itr->second;
            mObjectList.erase(itr);
        }

        return pData;
    }

    virtual TD* GetElement(const T& name)
    {
        typename map_data::iterator itr = mObjectList.find(name);
        if(itr != mObjectList.end())
        {
            return itr->second;
        }
        else
        {
            return NULL;
        }
    }

    virtual TD* First()
    {
        if(mObjectList.size() <= 0)
        {
            return NULL;
        }

        mObjectCurIter = mObjectList.begin();
        if(mObjectCurIter != mObjectList.end())
        {
            return mObjectCurIter->second;
        }
        else
        {
            return NULL;
        }
    }

    virtual TD* Next()
    {
        if(mObjectCurIter == mObjectList.end())
        {
            return NULL;
        }

        ++mObjectCurIter;
        if(mObjectCurIter != mObjectList.end())
        {
            return mObjectCurIter->second;
        }
        else
        {
            return NULL;
        }
    }

    virtual TD* First(T& name)
    {
        if(mObjectList.size() <= 0)
        {
            return NULL;
        }

        mObjectCurIter = mObjectList.begin();
        if(mObjectCurIter != mObjectList.end())
        {
            name = mObjectCurIter->first;
            return mObjectCurIter->second;
        }
        else
        {
            return NULL;
        }
    }

    virtual TD* Next(T& name)
    {
        if(mObjectCurIter == mObjectList.end())
        {
            return NULL;
        }

        mObjectCurIter++;
        if(mObjectCurIter != mObjectList.end())
        {
            name = mObjectCurIter->first;
            return mObjectCurIter->second;
        }

        return NULL;
    }

    int Count()
    {
        return mObjectList.size();
    }

    bool ClearAll()
    {
        mObjectList.clear();
        return true;
    }
private:
    map_data mObjectList;
    typename map_data::iterator mObjectCurIter;
};