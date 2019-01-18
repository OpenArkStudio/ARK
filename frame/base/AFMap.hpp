/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "AFMacros.hpp"

namespace ark
{

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
        typedef std::function<bool (PTRTYPE&) > MAP_CALLBACK;
        AFMapBase()
        {
            mNullPtr = nullptr;
        }

        virtual ~AFMapBase() = default;

        virtual bool AddElement(const T& name, const PTRTYPE data)
        {
            typename MAP_DATA::iterator iter = mxObjectList.find(name);

            if (iter == mxObjectList.end())
            {
                mxObjectList.insert(typename MAP_DATA::value_type(name, data));
                return true;
            }
            else
            {
                return false;
            }
        }

        virtual bool SetElement(const T& name, const PTRTYPE data)
        {
            mxObjectList[name] = data;
            return true;
        }

        virtual bool RemoveElement(const T& name)
        {
            typename MAP_DATA::iterator iter = mxObjectList.find(name);

            if (iter != mxObjectList.end())
            {
                mxObjectList.erase(iter);

                return true;
            }
            else
            {
                return false;
            }
        }

        virtual PTRTYPE& GetElement(const T& name)
        {
            typename MAP_DATA::iterator iter = mxObjectList.find(name);

            if (iter != mxObjectList.end())
            {
                return iter->second;
            }
            else
            {
                return mNullPtr;
            }
        }

        virtual PTRTYPE& First()
        {
            if (mxObjectList.size() <= 0)
            {
                return mNullPtr;
            }

            mxObjectCurIter = mxObjectList.begin();

            if (mxObjectCurIter != mxObjectList.end())
            {
                return mxObjectCurIter->second;
            }
            else
            {
                return mNullPtr;
            }
        }

        virtual PTRTYPE& Next()
        {
            if (mxObjectCurIter == mxObjectList.end())
            {
                return mNullPtr;
            }

            ++mxObjectCurIter;

            if (mxObjectCurIter != mxObjectList.end())
            {
                return mxObjectCurIter->second;
            }
            else
            {
                return mNullPtr;
            }
        }

        virtual PTRTYPE& First(T& name)
        {
            if (mxObjectList.size() <= 0)
            {
                return mNullPtr;
            }

            mxObjectCurIter = mxObjectList.begin();

            if (mxObjectCurIter != mxObjectList.end())
            {
                name = mxObjectCurIter->first;
                return mxObjectCurIter->second;
            }
            else
            {
                return mNullPtr;
            }
        }
        virtual PTRTYPE& Next(T& name)
        {
            if (mxObjectCurIter == mxObjectList.end())
            {
                return mNullPtr;
            }

            mxObjectCurIter++;

            if (mxObjectCurIter != mxObjectList.end())
            {
                name = mxObjectCurIter->first;
                return mxObjectCurIter->second;
            }
            else
            {
                return mNullPtr;
            }
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

        bool DoEveryElement(const MAP_CALLBACK& callback)
        {
            for (auto iter = mxObjectList.begin();  iter != mxObjectList.end(); ++iter)
            {
                if (!callback(iter->second))
                {
                    return false;
                }
            }

            return true;
        }

    private:
        MAP_DATA mxObjectList;
        typename MAP_DATA::iterator mxObjectCurIter;
        PTRTYPE mNullPtr;
    };

    template <typename T, typename TD>
    class AFMapEx : public AFMapBase<T, TD, true>
    {
    };

    template <typename T, typename TD>
    class AFMap : public AFMapBase<T, TD, false>
    {
    };

}