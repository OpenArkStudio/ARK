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
//#include "SDK/Core/AFGUID.hpp"
#include "SDK/Core/AFIEntity.h"

namespace ark
{

    //Map instance
    class AFMapInstance
    {
    public:
        explicit AFMapInstance(int nGroupID) :
            mnGroupID(nGroupID)
        {

        }

        virtual ~AFMapInstance()
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

    //All instance in this map
    class AFMapInfo : public AFMapEx<int, AFMapInstance>
    {
    public:
        explicit AFMapInfo() :
            instance_id_(0),
            width_(512)
        {
        }

        explicit AFMapInfo(int width) :
            instance_id_(0),
            width_(width)
        {
        }

        virtual ~AFMapInfo()
        {
            ClearAll();
        }

        int CreateInstanceID()
        {
            return ++instance_id_;
        }

        int GetWidth()
        {
            return width_;
        }

        bool AddEntityToInstance(const int nGroupID, const AFGUID& ident, bool bPlayer)
        {
            ARK_SHARE_PTR<AFMapInstance> pInfo = GetElement(nGroupID);

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

        bool RemoveEntityFromInstance(const int nGroupID, const AFGUID& ident, bool bPlayer)
        {
            ARK_SHARE_PTR<AFMapInstance> pInfo = GetElement(nGroupID);

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
        int instance_id_;
        int map_id_;
        int width_; //will separate the grid
    };

    class AFIMapModule : public AFIModule
    {
    public:
        virtual ~AFIMapModule() = default;

        virtual ARK_SHARE_PTR<AFMapInfo>& GetMapInfo(const int map_id) = 0;

        virtual bool IsInMapInstance(const AFGUID& self) = 0;
        virtual bool ExistMap(const int nContainerIndex) = 0;

        virtual bool SwitchMap(const AFGUID& self, const int nTargetSceneID, const int nTargetGroupID, const Point3D& pos, const float fOrient, const AFIDataList& arg) = 0;

        virtual bool CreateMap(const int nSceneID) = 0;
        virtual bool DestroyMap(const int nSceneID) = 0;

        virtual int GetOnlineCount() = 0;
        virtual int GetMaxOnlineCount() = 0;
        virtual int GetMapOnlineCount(const int nSceneID) = 0;
        virtual int GetMapOnlineCount(const int nSceneID, const int nGroupID) = 0;
        virtual int GetMapOnlineList(const int nSceneID, AFIDataList& var) = 0;

        virtual int CreateMapInstance(const int nSceneID) = 0;
        virtual bool ReleaseMapInstance(const int nSceneID, const int nGroupID) = 0;
        virtual bool ExitMapInstance(const int nSceneID, const int nGroupID) = 0;

        virtual bool GetInstEntityList(const int nSceneID, const int nGroupID, AFIDataList& list) = 0;
        virtual int GetEntityByDataNode(const int nSceneID, const std::string& strPropertyName, const AFIDataList& valueArg, AFIDataList& list) = 0;
    };

}