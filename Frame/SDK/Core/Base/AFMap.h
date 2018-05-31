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
#include "AFMacros.hpp"

template <typename T, typename TD>
class AFMap
{
public:
    typedef std::map<T, TD*> MAP_DATA;

    AFMap() = default;
    virtual ~AFMap() = default;

    virtual bool AddElement(const T& name, TD* data)
    {
        typename MAP_DATA::iterator iter = mxObjectList.find(name);
        if(iter == mxObjectList.end())
        {
            mxObjectList.insert(typename MAP_DATA::value_type(name, data));
            return true;
        }

        return false;
    }

    virtual TD* RemoveElement(const T& name)
    {
        TD* pData = NULL;
        typename MAP_DATA::iterator iter = mxObjectList.find(name);
        if(iter != mxObjectList.end())
        {
            pData = iter->second;
            mxObjectList.erase(iter);
        }

        return pData;
    }

    virtual TD* GetElement(const T& name)
    {
        typename MAP_DATA::iterator iter = mxObjectList.find(name);
        if(iter != mxObjectList.end())
        {
            return iter->second;
        }
        else
        {
            return NULL;
        }
    }

    virtual TD* First()
    {
        if(mxObjectList.size() <= 0)
        {
            return NULL;
        }

        mxObjectCurIter = mxObjectList.begin();
        if(mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second;
        }
        else
        {
            return NULL;
        }
    }

    virtual TD* Next()
    {
        if(mxObjectCurIter == mxObjectList.end())
        {
            return NULL;
        }

        ++mxObjectCurIter;
        if(mxObjectCurIter != mxObjectList.end())
        {
            return mxObjectCurIter->second;
        }
        else
        {
            return NULL;
        }
    }

    virtual TD* First(T& name)
    {
        if(mxObjectList.size() <= 0)
        {
            return NULL;
        }

        mxObjectCurIter = mxObjectList.begin();
        if(mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second;
        }
        else
        {
            return NULL;
        }
    }

    virtual TD* Next(T& name)
    {
        if(mxObjectCurIter == mxObjectList.end())
        {
            return NULL;
        }

        mxObjectCurIter++;
        if(mxObjectCurIter != mxObjectList.end())
        {
            name = mxObjectCurIter->first;
            return mxObjectCurIter->second;
        }

        return NULL;
    }

    int GetCount()
    {
        return mxObjectList.size();
    }

    bool Clear()
    {
        mxObjectList.clear();
        return true;
    }
private:
    MAP_DATA mxObjectList;
    typename MAP_DATA::iterator mxObjectCurIter;
};