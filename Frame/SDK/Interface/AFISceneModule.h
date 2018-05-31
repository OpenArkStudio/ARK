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

#include "SDK/Core/Base/AFList.h"
#include "SDK/Core/Base/AFMap.h"
#include "SDK/Core/Base/AFGUID.h"
#include "SDK/Core/AFIEntity.h"

class AFCSceneGroupInfo
{
public:
    AFCSceneGroupInfo(int nSceneID, int nGroupID)
    {
        mnGroupID = nGroupID;
    }

    AFCSceneGroupInfo(int nSceneID, int nGroupID, int nWidth)
    {
        mnGroupID = nGroupID;
    }

    virtual ~AFCSceneGroupInfo()
    {
    }

    bool Execute()
    {
        return true;
    }

    AFMapEx<AFGUID, int> mxPlayerList;
    AFMapEx<AFGUID, int> mxOtherList;
    int mnGroupID;
};

// all group in this scene
class AFCSceneInfo
    : public AFMapEx<int, AFCSceneGroupInfo>
{
public:

    explicit AFCSceneInfo(int nSceneID)
    {
        mnGroupIndex = 0;
        mnSceneID = nSceneID;
        mnWidth = 512;
    }

    explicit AFCSceneInfo(int nSceneID, int nWidth)
    {
        mnGroupIndex = 0;
        mnSceneID = nSceneID;
        mnWidth = nWidth;
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
        if (pInfo.get())
        {
            if (bPlayer)
            {
                return pInfo->mxPlayerList.AddElement(ident, ARK_SHARE_PTR<int>()); // TODO:Map.second涓虹┖锛屼娇鐢ㄧ殑鏃跺€欏崈涓囨敞鎰?
            }
            else
            {
                return pInfo->mxOtherList.AddElement(ident, ARK_SHARE_PTR<int>()); // TODO:Map.second涓虹┖锛屼娇鐢ㄧ殑鏃跺€欏崈涓囨敞鎰?
            }
        }

        return false;
    }

    bool RemoveObjectFromGroup(const int nGroupID, const AFGUID& ident, bool bPlayer)
    {
        ARK_SHARE_PTR<AFCSceneGroupInfo> pInfo = GetElement(nGroupID);
        if (nullptr != pInfo)
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

        return false;
    }

    bool Execute()
    {
        ARK_SHARE_PTR<AFCSceneGroupInfo> pGroupInfo = First();
        while (pGroupInfo.get())
        {
            pGroupInfo->Execute();

            pGroupInfo = Next();
        }
        return true;
    }
private:
    int mnGroupIndex;
    int mnSceneID;
    int mnWidth;
};

class AFISceneModule
    : public AFIModule,
  public AFMapEx<int, AFCSceneInfo>
{
public:
    virtual ~AFISceneModule()
    {
        ClearAll();
    }
};

