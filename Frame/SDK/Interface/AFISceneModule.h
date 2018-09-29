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

#include "SDK/Core/AFList.hpp"
#include "SDK/Core/AFMap.hpp"
#include "SDK/Core/AFGUID.hpp"
#include "SDK/Core/AFIEntity.h"

namespace ark
{

    class AFCSceneGroupInfo
    {
    public:
        explicit AFCSceneGroupInfo(int nGroupID) :
            mnGroupID(nGroupID)
        {

        }

        virtual ~AFCSceneGroupInfo()
        {
        }

        bool Update()
        {
            return true;
        }

        AFMapEx<AFGUID, int> mxPlayerList;
        AFMapEx<AFGUID, int> mxOtherList;
        int mnGroupID;
    };

    // all group in this scene
    class AFCSceneInfo : public AFMapEx<int, AFCSceneGroupInfo>
    {
    public:
        explicit AFCSceneInfo() :
            mnGroupIndex(0),
            mnWidth(512)
        {
        }

        explicit AFCSceneInfo(int nWidth) :
            mnGroupIndex(0),
            mnWidth(nWidth)
        {
        }

        virtual ~AFCSceneInfo()
        {
            ClearAll();
        }

        int NewGroupID()
        {
            return ++mnGroupIndex;
        }

        int GetWidth()
        {
            return mnWidth;
        }

        bool AddObjectToGroup(const int nGroupID, const AFGUID& ident, bool bPlayer)
        {
            ARK_SHARE_PTR<AFCSceneGroupInfo> pInfo = GetElement(nGroupID);

            if (pInfo == nullptr)
            {
                return false;
            }
            else
            {
                if (bPlayer)
                {
                    return pInfo->mxPlayerList.AddElement(ident, ARK_SHARE_PTR<int>()); //TODO:Map.second mean nothing
                }
                else
                {
                    return pInfo->mxOtherList.AddElement(ident, ARK_SHARE_PTR<int>()); //TODO:Map.second mean nothing
                }
            }
        }

        bool RemoveObjectFromGroup(const int nGroupID, const AFGUID& ident, bool bPlayer)
        {
            ARK_SHARE_PTR<AFCSceneGroupInfo> pInfo = GetElement(nGroupID);

            if (nullptr == pInfo)
            {
                return false;
            }
            else
            {
                if (bPlayer)
                {
                    return pInfo->mxPlayerList.RemoveElement(ident);
                }
                else
                {
                    return pInfo->mxOtherList.RemoveElement(ident);
                }
            }
        }

        bool Update()
        {
            return true;
        }

    private:
        int mnGroupIndex;
        int mnSceneID;
        int mnWidth;
    };

    class AFISceneModule : public AFIModule, public AFMapEx<int, AFCSceneInfo>
    {
    public:
        virtual ~AFISceneModule()
        {
            ClearAll();
        }
    };

}