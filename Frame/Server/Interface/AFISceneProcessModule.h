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

#include "SDK/Interface/AFIModule.h"

enum E_SCENE_TYPE
{
    SCENE_TYPE_NORMAL,//城镇
    SCENE_TYPE_CLONE_SCENE,//副本

    SCENE_TYPE_ERROR,
};

class AFISceneProcessModule : public AFIModule
{
public:
    virtual bool IsCloneScene(const int nSceneID) = 0;
    virtual E_SCENE_TYPE GetCloneSceneType(const int nContainerID) = 0;
    virtual bool ApplyCloneGroup(const int nSceneID, int& nGroupID) = 0;
    virtual bool ExitCloneGroup(const int nSceneID, const int& nGroupID) = 0;
};