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
#include "AFArrayPod.hpp"
#include "AFStringPod.hpp"

template<typename T>
class IsBuildinType
{
public:
    enum {YES = 1, NO = 0};
};



//////////////////////////////////////////////////////////////////////////

template<typename KEY, typename NODE>
class AFArrayMap {};

//partial template specialization for KEY=std::string
template<typename NODE>
class AFArrayMap<std::string, NODE>
{
public:
    AFArrayMap()
    {

    }

    ~AFArrayMap()
    {
        ReleaseAll();
        mxIndices.Clear();
    }

    void Clear()
    {
        ReleaseAll();
        mxIndices.Clear();
    }

    bool AddElement(const std::string& name, NODE* data)
    {
        mxIndices.Add(name.c_str(), mxNodes.size());
        mxNodes.push_back(data);
        return true;
    }

    NODE* GetElement(const std::string& name)
    {
        size_t index;
        if (!mxIndices.GetData(name.c_str(), index))
        {
            return false;
        }

        return mxNodes[index];
    }

    bool RemoveElement(const std::string& name)
    {
        size_t index;
        if (!mxIndices.GetData(name.c_str(), index))
        {
            return false;
        }
        
        //ÊÍ·ÅÄÚ´æ
        if (NULL != mxNodes[index])
        {
            delete mxNodes[index];
            mxNodes[index] = NULL;
        }

        mxNodes.remove(index);
        mxIndices.Remove(name.c_str());
        return true;
    }

    size_t GetCount() const
    {
        return mxNodes.size();
    }

protected:
    void ReleaseAll()
    {
        for (size_t i = 0; i < GetCount(); ++i)
        {
            delete mxNodes[i];
            mxNodes[i] = NULL;
        }

        mxNodes.clear();
    }

private:
    ArraryPod<NODE*, 1, CoreAlloc> mxNodes;
    StringPod<char, size_t, StringTraits<char>> mxIndices;
};