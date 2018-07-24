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

#include "SDK/Core/AFGUID.h"
#include "SDK/Core/AFIDataList.h"
#include "SDK/Core/AFIData.h"

using namespace ArkFrame;

#define DEFAULT_RADIUS 20

class AFCAOIModule
{
public:
    AFCAOIModule();
    ~AFCAOIModule();

    void Add(const AFGUID& self, const Point3D& xPos, int nRadius = DEFAULT_RADIUS);
    void Move(const AFGUID& self, const Point3D& xPos);
    void Leave(const AFGUID& self);

    struct AOIObject
    {
        AOIObject(const AFGUID& self, const Point3D& pos, const int nRadius)
        {
            id = self;
            x = (int)pos.x;
            y = (int)pos.z;
            radius = nRadius;
        }

        int x = 0;
        int y = 0;
        AFGUID id = NULL_GUID;
        int radius = 0;
        std::list<AOIObject*>::iterator x_pos;
        std::list<AOIObject*>::iterator y_pos;
    };

private:
    using ObjectMap = std::map<AFGUID, AOIObject*>;
    using ObjectList = std::list<AOIObject*>;

    void Update(AOIObject* pObject);
    void GetRange(AOIObject* pObject, ObjectMap* pMap);
    void UpdateObejctPos(AOIObject* pObject, const Point3D& xNewPos);

private:
    ObjectList xObjectList;
    ObjectList yObjectList;
    ObjectMap  objectMap;

    ObjectMap moveMap;
    ObjectMap enterMap;
    ObjectMap leaveMap;
};