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

namespace ark
{

    enum AFEventDefine
    {
        ////////////////Has Self---logic//////////////////////////////////////////////////////////
        //arg[0]:resultID[EGameErrorCode]
        AFED_ON_GENERAL_MESSAGE,

        //arg[0]:x,y,z
        AFED_ON_CLIENT_REQUIRE_MOVE,
        AFED_ON_CLIENT_MOVE_RESULT,

        //arg[0]:player_id key_value skillID target(1-N)
        AFED_ON_CLIENT_REQUIRE_USE_SKILL,
        AFED_ON_CLIENT_USE_SKILL_RESULT,

        //arg[0]:skillID, fx, fy, fz, TargetObjectList
        AFED_ON_CLIENT_REQUIRE_USE_SKILL_POS,
        AFED_ON_CLIENT_USE_SKILL_POS_RESULT,

        //arg[0]:itemID,TargetObject
        AFED_ON_CLIENT_REQUIRE_USE_ITEM,

        //arg[0]:itemID, fx, fy, fz, TargetObjectList
        AFED_ON_CLIENT_REQUIRE_USE_ITEM_POS,

        //arg[0]:object,type,sceneID,line
        AFED_ON_CLIENT_ENTER_SCENE,
        //arg[0]:object
        AFED_ON_CLIENT_LEAVE_SCENE,

        //arg[0]:object,type,sceneID,line
        AFED_ON_OBJECT_ENTER_SCENE_BEFORE,
        AFED_ON_OBJECT_ENTER_SCENE_RESULT,

        AFED_ON_OBJECT_BE_KILLED,

        // 通知副本奖励
        AFED_ON_NOTICE_ECTYPE_AWARD,

    };

}