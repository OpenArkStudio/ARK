// -------------------------------------------------------------------------
//    @FileName         :    AFEventDefine.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFEventDefine
//
// -------------------------------------------------------------------------

#ifndef AF_EVENTDEFINE_H
#define AF_EVENTDEFINE_H

#include <iostream>
///

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

#endif