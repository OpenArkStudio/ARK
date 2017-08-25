/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @FileName  AFMapEx.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFMapEx                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFMapEx                                                  *
*****************************************************************************/
#pragma once

#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Base/AFMacros.hpp"

template <typename T , typename TD>
class AFMapEx
{
public:
    typedef std::map<T, ARK_SHARE_PTR<TD> > NFMapOBJECT;

    AFMapEx() {};
    virtual ~AFMapEx()
    {
    };

    virtual bool AddElement(const T& name, const ARK_SHARE_PTR<TD> data)
    {
        typename NFMapOBJECT::iterator itr = mObjectList.find(name);
        if (itr == mObjectList.end())
        {
            mObjectList.insert(typename NFMapOBJECT::value_type(name, data));
            return true;
        }

        return false;
    }

    virtual bool SetElement(const T& name, const ARK_SHARE_PTR<TD> data)
    {
        mObjectList[name] = data;

        return true;
    }

    virtual bool RemoveElement(const T& name)
    {
        ARK_SHARE_PTR<TD> pData;
        typename NFMapOBJECT::iterator itr = mObjectList.find(name);
        if (itr != mObjectList.end())
        {
            pData = itr->second;
            mObjectList.erase(itr);

            return true;
        }

        return false;
    }

    virtual TD* GetElementNude(const T& name)
    {
        typename NFMapOBJECT::iterator itr = mObjectList.find(name);
        if (itr != mObjectList.end())
        {
            return itr->second.get();
        }
        
        return NULL;
    }

    virtual ARK_SHARE_PTR<TD> GetElement(const T& name)
    {
        typename NFMapOBJECT::iterator itr = mObjectList.find(name);
        if (itr != mObjectList.end())
        {
            return itr->second;
        }
        
        return nullptr;
    }
    virtual TD* FirstNude(T& name)
    {
        if (mObjectList.size() <= 0)
        {
            return NULL;
        }

        mObjectCurIter = mObjectList.begin();
        if (mObjectCurIter != mObjectList.end())
        {
            name = mObjectCurIter->first;
            return mObjectCurIter->second.get();
        }
        
        return NULL;
    }

    virtual TD* NextNude(T& name)
    {
        if (mObjectCurIter == mObjectList.end())
        {
            return NULL;
        }

        mObjectCurIter++;
        if (mObjectCurIter != mObjectList.end())
        {
            name = mObjectCurIter->first;
            return mObjectCurIter->second.get();
        }
        
        return NULL;
    }

    virtual TD* FirstNude()
    {
        if (mObjectList.size() <= 0)
        {
            return NULL;
        }

        mObjectCurIter = mObjectList.begin();
        if (mObjectCurIter != mObjectList.end())
        {
            return mObjectCurIter->second.get();
        }
        
        return NULL;
    }

    virtual TD* NextNude()
    {
        if (mObjectCurIter == mObjectList.end())
        {
            return NULL;
        }

        mObjectCurIter++;
        if (mObjectCurIter != mObjectList.end())
        {
            return mObjectCurIter->second.get();
        }
        
        return NULL;
    }

    virtual ARK_SHARE_PTR<TD> First()
    {
        if (mObjectList.size() <= 0)
        {
            return nullptr;
        }

        mObjectCurIter = mObjectList.begin();
        if (mObjectCurIter != mObjectList.end())
        {
            return mObjectCurIter->second;
        }

        return nullptr;
    }

    virtual ARK_SHARE_PTR<TD> Next()
    {
        if (mObjectCurIter == mObjectList.end())
        {
            return ARK_SHARE_PTR<TD>();
        }

        ++mObjectCurIter;
        if (mObjectCurIter != mObjectList.end())
        {
            return mObjectCurIter->second;
        }

        return nullptr;
    }

    virtual ARK_SHARE_PTR<TD> First(T& name)
    {
        if (mObjectList.size() <= 0)
        {
            return ARK_SHARE_PTR<TD>();
        }

        mObjectCurIter = mObjectList.begin();
        if (mObjectCurIter != mObjectList.end())
        {
            name = mObjectCurIter->first;
            return mObjectCurIter->second;
        }

        return nullptr;
    }

    virtual ARK_SHARE_PTR<TD> Next(T& name)
    {
        if (mObjectCurIter == mObjectList.end())
        {
            return ARK_SHARE_PTR<TD>();
        }

        mObjectCurIter++;
        if (mObjectCurIter != mObjectList.end())
        {
            name = mObjectCurIter->first;
            return mObjectCurIter->second;
        }

        return nullptr;
    }

    int Count()
    {
        return (int)mObjectList.size();
    }

    bool ClearAll()
    {
        mObjectList.clear();
        return true;
    }

private:
    NFMapOBJECT     mObjectList;
    typename NFMapOBJECT::iterator mObjectCurIter;
};
