/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the 'License');
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#pragma once

#include <string>

namespace ARK
{
class IObject
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xIObject = "IObject";
        return xIObject;
    }
    //IObject
    //DataNodes
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataTables

};

class Buff
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xBuff = "Buff";
        return xBuff;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& EffectType()
    {
        static std::string xEffectType = "EffectType";    //int
        return xEffectType;
    }
    static const std::string& EffectValueType()
    {
        static std::string xEffectValueType = "EffectValueType";    //int
        return xEffectValueType;
    }
    static const std::string& EffectValueReferType()
    {
        static std::string xEffectValueReferType = "EffectValueReferType";    //int
        return xEffectValueReferType;
    }
    static const std::string& EffectTimeValue()
    {
        static std::string xEffectTimeValue = "EffectTimeValue";    //int
        return xEffectTimeValue;
    }
    static const std::string& EffectTimeInterval()
    {
        static std::string xEffectTimeInterval = "EffectTimeInterval";    //float
        return xEffectTimeInterval;
    }
    static const std::string& WashGroupID()
    {
        static std::string xWashGroupID = "WashGroupID";    //int
        return xWashGroupID;
    }
    static const std::string& ReverseReferType()
    {
        static std::string xReverseReferType = "ReverseReferType";    //int
        return xReverseReferType;
    }
    static const std::string& EffectClearOnDead()
    {
        static std::string xEffectClearOnDead = "EffectClearOnDead";    //int
        return xEffectClearOnDead;
    }
    static const std::string& DownSaveType()
    {
        static std::string xDownSaveType = "DownSaveType";    //int
        return xDownSaveType;
    }
    //DataTables

};

class ChatGroup
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xChatGroup = "ChatGroup";
        return xChatGroup;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& Name()
    {
        static std::string xName = "Name";    //string
        return xName;
    }
    static const std::string& CreateObject()
    {
        static std::string xCreateObject = "CreateObject";    //object
        return xCreateObject;
    }
    //DataTables
    static const std::string& R_GroupMemberList()
    {
        static std::string xGroupMemberList = "GroupMemberList";
        return xGroupMemberList;
    }
    static const std::string& R_ChatList()
    {
        static std::string xChatList = "ChatList";
        return xChatList;
    }

    enum GroupMemberList
    {
        GroupMemberList_GUID        = 0, //GUID -> object
        GroupMemberList_Online      = 1, //Online -> int
        GroupMemberList_GameID      = 2, //GameID -> int

    };

    enum ChatList
    {
        ChatList_GUID       = 0, //GUID -> object
        ChatList_msg        = 1, //msg -> string
        ChatList_time       = 2, //time -> int

    };

};

class ConsumeData
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xConsumeData = "ConsumeData";
        return xConsumeData;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& VIPEXP()
    {
        static std::string xVIPEXP = "VIPEXP";    //int
        return xVIPEXP;
    }
    static const std::string& EXP()
    {
        static std::string xEXP = "EXP";    //int
        return xEXP;
    }
    static const std::string& HP()
    {
        static std::string xHP = "HP";    //int
        return xHP;
    }
    static const std::string& SP()
    {
        static std::string xSP = "SP";    //int
        return xSP;
    }
    static const std::string& MP()
    {
        static std::string xMP = "MP";    //int
        return xMP;
    }
    static const std::string& Gold()
    {
        static std::string xGold = "Gold";    //int
        return xGold;
    }
    static const std::string& Money()
    {
        static std::string xMoney = "Money";    //int
        return xMoney;
    }
    //DataTables

};

class Cost
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xCost = "Cost";
        return xCost;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& CostMoney()
    {
        static std::string xCostMoney = "CostMoney";    //int
        return xCostMoney;
    }
    static const std::string& CostDiamond()
    {
        static std::string xCostDiamond = "CostDiamond";    //int
        return xCostDiamond;
    }
    static const std::string& CostVP()
    {
        static std::string xCostVP = "CostVP";    //int
        return xCostVP;
    }
    static const std::string& CostHonour()
    {
        static std::string xCostHonour = "CostHonour";    //int
        return xCostHonour;
    }
    //DataTables

};

class DescData
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xDescData = "DescData";
        return xDescData;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& ShowName()
    {
        static std::string xShowName = "ShowName";    //string
        return xShowName;
    }
    static const std::string& Icon()
    {
        static std::string xIcon = "Icon";    //string
        return xIcon;
    }
    static const std::string& Atlas()
    {
        static std::string xAtlas = "Atlas";    //string
        return xAtlas;
    }
    static const std::string& PrefabPath()
    {
        static std::string xPrefabPath = "PrefabPath";    //string
        return xPrefabPath;
    }
    static const std::string& PerformanceEffect()
    {
        static std::string xPerformanceEffect = "PerformanceEffect";    //string
        return xPerformanceEffect;
    }
    static const std::string& PerformanceSound()
    {
        static std::string xPerformanceSound = "PerformanceSound";    //string
        return xPerformanceSound;
    }
    static const std::string& DescText()
    {
        static std::string xDescText = "DescText";    //string
        return xDescText;
    }
    //DataTables

};

class EffectData
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xEffectData = "EffectData";
        return xEffectData;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& SUCKBLOOD()
    {
        static std::string xSUCKBLOOD = "SUCKBLOOD";    //int
        return xSUCKBLOOD;
    }
    static const std::string& REFLECTDAMAGE()
    {
        static std::string xREFLECTDAMAGE = "REFLECTDAMAGE";    //int
        return xREFLECTDAMAGE;
    }
    static const std::string& CRITICAL()
    {
        static std::string xCRITICAL = "CRITICAL";    //int
        return xCRITICAL;
    }
    static const std::string& MAXHP()
    {
        static std::string xMAXHP = "MAXHP";    //int
        return xMAXHP;
    }
    static const std::string& MAXMP()
    {
        static std::string xMAXMP = "MAXMP";    //int
        return xMAXMP;
    }
    static const std::string& MAXSP()
    {
        static std::string xMAXSP = "MAXSP";    //int
        return xMAXSP;
    }
    static const std::string& HPREGEN()
    {
        static std::string xHPREGEN = "HPREGEN";    //int
        return xHPREGEN;
    }
    static const std::string& SPREGEN()
    {
        static std::string xSPREGEN = "SPREGEN";    //int
        return xSPREGEN;
    }
    static const std::string& MPREGEN()
    {
        static std::string xMPREGEN = "MPREGEN";    //int
        return xMPREGEN;
    }
    static const std::string& ATK_VALUE()
    {
        static std::string xATK_VALUE = "ATK_VALUE";    //int
        return xATK_VALUE;
    }
    static const std::string& DEF_VALUE()
    {
        static std::string xDEF_VALUE = "DEF_VALUE";    //int
        return xDEF_VALUE;
    }
    static const std::string& MOVE_SPEED()
    {
        static std::string xMOVE_SPEED = "MOVE_SPEED";    //int
        return xMOVE_SPEED;
    }
    static const std::string& ATK_SPEED()
    {
        static std::string xATK_SPEED = "ATK_SPEED";    //int
        return xATK_SPEED;
    }
    static const std::string& ATK_FIRE()
    {
        static std::string xATK_FIRE = "ATK_FIRE";    //int
        return xATK_FIRE;
    }
    static const std::string& ATK_LIGHT()
    {
        static std::string xATK_LIGHT = "ATK_LIGHT";    //int
        return xATK_LIGHT;
    }
    static const std::string& ATK_WIND()
    {
        static std::string xATK_WIND = "ATK_WIND";    //int
        return xATK_WIND;
    }
    static const std::string& ATK_ICE()
    {
        static std::string xATK_ICE = "ATK_ICE";    //int
        return xATK_ICE;
    }
    static const std::string& ATK_POISON()
    {
        static std::string xATK_POISON = "ATK_POISON";    //int
        return xATK_POISON;
    }
    static const std::string& DEF_FIRE()
    {
        static std::string xDEF_FIRE = "DEF_FIRE";    //int
        return xDEF_FIRE;
    }
    static const std::string& DEF_LIGHT()
    {
        static std::string xDEF_LIGHT = "DEF_LIGHT";    //int
        return xDEF_LIGHT;
    }
    static const std::string& DEF_WIND()
    {
        static std::string xDEF_WIND = "DEF_WIND";    //int
        return xDEF_WIND;
    }
    static const std::string& DEF_ICE()
    {
        static std::string xDEF_ICE = "DEF_ICE";    //int
        return xDEF_ICE;
    }
    static const std::string& DEF_POISON()
    {
        static std::string xDEF_POISON = "DEF_POISON";    //int
        return xDEF_POISON;
    }
    static const std::string& DIZZY_GATE()
    {
        static std::string xDIZZY_GATE = "DIZZY_GATE";    //int
        return xDIZZY_GATE;
    }
    static const std::string& MOVE_GATE()
    {
        static std::string xMOVE_GATE = "MOVE_GATE";    //int
        return xMOVE_GATE;
    }
    static const std::string& SKILL_GATE()
    {
        static std::string xSKILL_GATE = "SKILL_GATE";    //int
        return xSKILL_GATE;
    }
    static const std::string& PHYSICAL_GATE()
    {
        static std::string xPHYSICAL_GATE = "PHYSICAL_GATE";    //int
        return xPHYSICAL_GATE;
    }
    static const std::string& MAGIC_GATE()
    {
        static std::string xMAGIC_GATE = "MAGIC_GATE";    //int
        return xMAGIC_GATE;
    }
    static const std::string& BUFF_GATE()
    {
        static std::string xBUFF_GATE = "BUFF_GATE";    //int
        return xBUFF_GATE;
    }
    //DataTables

};

class Equip
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xEquip = "Equip";
        return xEquip;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& Sex()
    {
        static std::string xSex = "Sex";    //int
        return xSex;
    }
    static const std::string& IntensiveBuffList()
    {
        static std::string xIntensiveBuffList = "IntensiveBuffList";    //string
        return xIntensiveBuffList;
    }
    static const std::string& EnchantmentBuffList()
    {
        static std::string xEnchantmentBuffList = "EnchantmentBuffList";    //string
        return xEnchantmentBuffList;
    }
    static const std::string& SuitID()
    {
        static std::string xSuitID = "SuitID";    //int
        return xSuitID;
    }
    static const std::string& SuitBuffID()
    {
        static std::string xSuitBuffID = "SuitBuffID";    //string
        return xSuitBuffID;
    }
    static const std::string& ItemType()
    {
        static std::string xItemType = "ItemType";    //int
        return xItemType;
    }
    static const std::string& ItemSubType()
    {
        static std::string xItemSubType = "ItemSubType";    //int
        return xItemSubType;
    }
    static const std::string& Level()
    {
        static std::string xLevel = "Level";    //int
        return xLevel;
    }
    static const std::string& Job()
    {
        static std::string xJob = "Job";    //string
        return xJob;
    }
    static const std::string& Quality()
    {
        static std::string xQuality = "Quality";    //int
        return xQuality;
    }
    static const std::string& ShowName()
    {
        static std::string xShowName = "ShowName";    //string
        return xShowName;
    }
    static const std::string& Desc()
    {
        static std::string xDesc = "Desc";    //string
        return xDesc;
    }
    static const std::string& EffectData()
    {
        static std::string xEffectData = "EffectData";    //string
        return xEffectData;
    }
    static const std::string& PrefabPath()
    {
        static std::string xPrefabPath = "PrefabPath";    //string
        return xPrefabPath;
    }
    static const std::string& DropPrePath()
    {
        static std::string xDropPrePath = "DropPrePath";    //string
        return xDropPrePath;
    }
    static const std::string& BuyPrice()
    {
        static std::string xBuyPrice = "BuyPrice";    //int
        return xBuyPrice;
    }
    static const std::string& SalePrice()
    {
        static std::string xSalePrice = "SalePrice";    //int
        return xSalePrice;
    }
    static const std::string& Icon()
    {
        static std::string xIcon = "Icon";    //string
        return xIcon;
    }
    //DataTables

};

class Guild
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xGuild = "Guild";
        return xGuild;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& Name()
    {
        static std::string xName = "Name";    //string
        return xName;
    }
    static const std::string& PresidentID()
    {
        static std::string xPresidentID = "PresidentID";    //object
        return xPresidentID;
    }
    static const std::string& PresidentName()
    {
        static std::string xPresidentName = "PresidentName";    //string
        return xPresidentName;
    }
    static const std::string& Rank()
    {
        static std::string xRank = "Rank";    //int
        return xRank;
    }
    static const std::string& GuildAD()
    {
        static std::string xGuildAD = "GuildAD";    //string
        return xGuildAD;
    }
    static const std::string& GuildDesc()
    {
        static std::string xGuildDesc = "GuildDesc";    //string
        return xGuildDesc;
    }
    static const std::string& GuildMoney()
    {
        static std::string xGuildMoney = "GuildMoney";    //int
        return xGuildMoney;
    }
    static const std::string& GuildLevel()
    {
        static std::string xGuildLevel = "GuildLevel";    //int
        return xGuildLevel;
    }
    static const std::string& GuildContinueDay()
    {
        static std::string xGuildContinueDay = "GuildContinueDay";    //int
        return xGuildContinueDay;
    }
    static const std::string& GuilID()
    {
        static std::string xGuilID = "GuilID";    //object
        return xGuilID;
    }
    static const std::string& GuilIDIcon()
    {
        static std::string xGuilIDIcon = "GuilIDIcon";    //int
        return xGuilIDIcon;
    }
    static const std::string& GuildMemeberCount()
    {
        static std::string xGuildMemeberCount = "GuildMemeberCount";    //int
        return xGuildMemeberCount;
    }
    static const std::string& GuildMemeberMaxCount()
    {
        static std::string xGuildMemeberMaxCount = "GuildMemeberMaxCount";    //int
        return xGuildMemeberMaxCount;
    }
    static const std::string& GuildHonor()
    {
        static std::string xGuildHonor = "GuildHonor";    //int
        return xGuildHonor;
    }
    static const std::string& GuildCreateTime()
    {
        static std::string xGuildCreateTime = "GuildCreateTime";    //int
        return xGuildCreateTime;
    }
    static const std::string& GuildCreateter()
    {
        static std::string xGuildCreateter = "GuildCreateter";    //int
        return xGuildCreateter;
    }
    static const std::string& GuildExp()
    {
        static std::string xGuildExp = "GuildExp";    //int
        return xGuildExp;
    }
    static const std::string& GuildStatus()
    {
        static std::string xGuildStatus = "GuildStatus";    //int
        return xGuildStatus;
    }
    static const std::string& DismissTime()
    {
        static std::string xDismissTime = "DismissTime";    //int
        return xDismissTime;
    }
    static const std::string& RecruitAD()
    {
        static std::string xRecruitAD = "RecruitAD";    //string
        return xRecruitAD;
    }
    static const std::string& RecruitLevel()
    {
        static std::string xRecruitLevel = "RecruitLevel";    //int
        return xRecruitLevel;
    }
    static const std::string& KingWarResource()
    {
        static std::string xKingWarResource = "KingWarResource";    //int
        return xKingWarResource;
    }
    static const std::string& AutoRecruit()
    {
        static std::string xAutoRecruit = "AutoRecruit";    //string
        return xAutoRecruit;
    }
    static const std::string& EctypServer()
    {
        static std::string xEctypServer = "EctypServer";    //int
        return xEctypServer;
    }
    static const std::string& EctypID()
    {
        static std::string xEctypID = "EctypID";    //int
        return xEctypID;
    }
    static const std::string& EctypIDGroup()
    {
        static std::string xEctypIDGroup = "EctypIDGroup";    //int
        return xEctypIDGroup;
    }
    //DataTables
    static const std::string& R_GuildBoss()
    {
        static std::string xGuildBoss = "GuildBoss";
        return xGuildBoss;
    }
    static const std::string& R_GuildMemberList()
    {
        static std::string xGuildMemberList = "GuildMemberList";
        return xGuildMemberList;
    }
    static const std::string& R_GuildAppyList()
    {
        static std::string xGuildAppyList = "GuildAppyList";
        return xGuildAppyList;
    }
    static const std::string& R_GuildEvent()
    {
        static std::string xGuildEvent = "GuildEvent";
        return xGuildEvent;
    }
    static const std::string& R_GuildHouse()
    {
        static std::string xGuildHouse = "GuildHouse";
        return xGuildHouse;
    }
    static const std::string& R_GuildSkill()
    {
        static std::string xGuildSkill = "GuildSkill";
        return xGuildSkill;
    }

    enum GuildBoss
    {
        GuildBoss_GUID      = 0, //GUID -> object
        GuildBoss_Name      = 1, //Name -> string
        GuildBoss_Level     = 2, //Level -> int
        GuildBoss_Job       = 3, //Job -> int
        GuildBoss_Donation      = 4, //Donation -> int
        GuildBoss_VIP       = 5, //VIP -> int
        GuildBoss_Offline       = 6, //Offline -> int
        GuildBoss_Power     = 7, //Power -> int

    };

    enum GuildMemberList
    {
        GuildMemberList_GUID        = 0, //GUID -> object
        GuildMemberList_Name        = 1, //Name -> string
        GuildMemberList_Level       = 2, //Level -> int
        GuildMemberList_Job     = 3, //Job -> int
        GuildMemberList_Donation        = 4, //Donation -> int
        GuildMemberList_Receive     = 5, //Receive -> int
        GuildMemberList_VIP     = 6, //VIP -> int
        GuildMemberList_Online      = 7, //Online -> int
        GuildMemberList_Power       = 8, //Power -> int
        GuildMemberList_Title       = 9, //Title -> int
        GuildMemberList_GameID      = 10, //GameID -> int
        GuildMemberList_JoinTime        = 11, //JoinTime -> int
        GuildMemberList_Contribution        = 12, //Contribution -> int
        GuildMemberList_AllContribution     = 13, //AllContribution -> int

    };

    enum GuildAppyList
    {
        GuildAppyList_GUID      = 0, //GUID -> object
        GuildAppyList_Name      = 1, //Name -> string
        GuildAppyList_Level     = 2, //Level -> int
        GuildAppyList_Job       = 3, //Job -> int
        GuildAppyList_Donation      = 4, //Donation -> int
        GuildAppyList_VIP       = 5, //VIP -> int
        GuildAppyList_Power     = 6, //Power -> int

    };

    enum GuildEvent
    {
        GuildEvent_GUID     = 0, //GUID -> object
        GuildEvent_Name     = 1, //Name -> string
        GuildEvent_Level        = 2, //Level -> int
        GuildEvent_Job      = 3, //Job -> int
        GuildEvent_Donation     = 4, //Donation -> int
        GuildEvent_VIP      = 5, //VIP -> int
        GuildEvent_Offline      = 6, //Offline -> int
        GuildEvent_Power        = 7, //Power -> int
        GuildEvent_EventID      = 8, //EventID -> int
        GuildEvent_EventTime        = 9, //EventTime -> int
        GuildEvent_Context      = 10, //Context -> string

    };

    enum GuildHouse
    {
        GuildHouse_GUID     = 0, //GUID -> object
        GuildHouse_Name     = 1, //Name -> string
        GuildHouse_Level        = 2, //Level -> int
        GuildHouse_Job      = 3, //Job -> int
        GuildHouse_Donation     = 4, //Donation -> int
        GuildHouse_VIP      = 5, //VIP -> int
        GuildHouse_Offline      = 6, //Offline -> int
        GuildHouse_Power        = 7, //Power -> int

    };

    enum GuildSkill
    {
        GuildSkill_GUID     = 0, //GUID -> object
        GuildSkill_Name     = 1, //Name -> string
        GuildSkill_Level        = 2, //Level -> int
        GuildSkill_Job      = 3, //Job -> int
        GuildSkill_Donation     = 4, //Donation -> int
        GuildSkill_VIP      = 5, //VIP -> int
        GuildSkill_Offline      = 6, //Offline -> int
        GuildSkill_Power        = 7, //Power -> int

    };

};

class GuildConfig
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xGuildConfig = "GuildConfig";
        return xGuildConfig;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& ConditionPlayerLevel()
    {
        static std::string xConditionPlayerLevel = "ConditionPlayerLevel";    //int
        return xConditionPlayerLevel;
    }
    static const std::string& ConditionPlayerVIP()
    {
        static std::string xConditionPlayerVIP = "ConditionPlayerVIP";    //int
        return xConditionPlayerVIP;
    }
    static const std::string& CostMoney()
    {
        static std::string xCostMoney = "CostMoney";    //int
        return xCostMoney;
    }
    static const std::string& DismissTime()
    {
        static std::string xDismissTime = "DismissTime";    //int
        return xDismissTime;
    }
    static const std::string& GuildLevel()
    {
        static std::string xGuildLevel = "GuildLevel";    //int
        return xGuildLevel;
    }
    static const std::string& MaxMember()
    {
        static std::string xMaxMember = "MaxMember";    //int
        return xMaxMember;
    }
    //DataTables

};

class GuildJob
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xGuildJob = "GuildJob";
        return xGuildJob;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& Job()
    {
        static std::string xJob = "Job";    //int
        return xJob;
    }
    static const std::string& JobCount()
    {
        static std::string xJobCount = "JobCount";    //object
        return xJobCount;
    }
    static const std::string& Appoint()
    {
        static std::string xAppoint = "Appoint";    //object
        return xAppoint;
    }
    static const std::string& Fire()
    {
        static std::string xFire = "Fire";    //object
        return xFire;
    }
    static const std::string& Demise()
    {
        static std::string xDemise = "Demise";    //object
        return xDemise;
    }
    static const std::string& ApplyDismiss()
    {
        static std::string xApplyDismiss = "ApplyDismiss";    //object
        return xApplyDismiss;
    }
    static const std::string& StopDismiss()
    {
        static std::string xStopDismiss = "StopDismiss";    //object
        return xStopDismiss;
    }
    static const std::string& AcceptApply()
    {
        static std::string xAcceptApply = "AcceptApply";    //object
        return xAcceptApply;
    }
    static const std::string& DenyApply()
    {
        static std::string xDenyApply = "DenyApply";    //object
        return xDenyApply;
    }
    static const std::string& Kickout()
    {
        static std::string xKickout = "Kickout";    //object
        return xKickout;
    }
    static const std::string& SetRecruit()
    {
        static std::string xSetRecruit = "SetRecruit";    //object
        return xSetRecruit;
    }
    static const std::string& PublishRecruit()
    {
        static std::string xPublishRecruit = "PublishRecruit";    //object
        return xPublishRecruit;
    }
    static const std::string& EditAD()
    {
        static std::string xEditAD = "EditAD";    //object
        return xEditAD;
    }
    static const std::string& Leave()
    {
        static std::string xLeave = "Leave";    //object
        return xLeave;
    }
    static const std::string& LevelUp()
    {
        static std::string xLevelUp = "LevelUp";    //object
        return xLevelUp;
    }
    //DataTables

};

class GuildName
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xGuildName = "GuildName";
        return xGuildName;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& GuildID()
    {
        static std::string xGuildID = "GuildID";    //object
        return xGuildID;
    }
    //DataTables

};

class InitProperty
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xInitProperty = "InitProperty";
        return xInitProperty;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& Job()
    {
        static std::string xJob = "Job";    //int
        return xJob;
    }
    static const std::string& Level()
    {
        static std::string xLevel = "Level";    //int
        return xLevel;
    }
    static const std::string& EffectData()
    {
        static std::string xEffectData = "EffectData";    //string
        return xEffectData;
    }
    static const std::string& SkillIDRef()
    {
        static std::string xSkillIDRef = "SkillIDRef";    //string
        return xSkillIDRef;
    }
    static const std::string& ModelPtah()
    {
        static std::string xModelPtah = "ModelPtah";    //string
        return xModelPtah;
    }
    //DataTables

};

class Item
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xItem = "Item";
        return xItem;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& ItemType()
    {
        static std::string xItemType = "ItemType";    //int
        return xItemType;
    }
    static const std::string& ItemSubType()
    {
        static std::string xItemSubType = "ItemSubType";    //int
        return xItemSubType;
    }
    static const std::string& Level()
    {
        static std::string xLevel = "Level";    //int
        return xLevel;
    }
    static const std::string& Job()
    {
        static std::string xJob = "Job";    //string
        return xJob;
    }
    static const std::string& Quality()
    {
        static std::string xQuality = "Quality";    //int
        return xQuality;
    }
    static const std::string& DesignDesc()
    {
        static std::string xDesignDesc = "DesignDesc";    //string
        return xDesignDesc;
    }
    static const std::string& DescID()
    {
        static std::string xDescID = "DescID";    //string
        return xDescID;
    }
    static const std::string& EffectData()
    {
        static std::string xEffectData = "EffectData";    //string
        return xEffectData;
    }
    static const std::string& ConsumeData()
    {
        static std::string xConsumeData = "ConsumeData";    //string
        return xConsumeData;
    }
    static const std::string& AwardData()
    {
        static std::string xAwardData = "AwardData";    //string
        return xAwardData;
    }
    static const std::string& AwardProperty()
    {
        static std::string xAwardProperty = "AwardProperty";    //int
        return xAwardProperty;
    }
    static const std::string& CoolDownTime()
    {
        static std::string xCoolDownTime = "CoolDownTime";    //float
        return xCoolDownTime;
    }
    static const std::string& OverlayCount()
    {
        static std::string xOverlayCount = "OverlayCount";    //int
        return xOverlayCount;
    }
    static const std::string& ExpiredType()
    {
        static std::string xExpiredType = "ExpiredType";    //int
        return xExpiredType;
    }
    static const std::string& BuyPrice()
    {
        static std::string xBuyPrice = "BuyPrice";    //int
        return xBuyPrice;
    }
    static const std::string& SalePrice()
    {
        static std::string xSalePrice = "SalePrice";    //int
        return xSalePrice;
    }
    static const std::string& Script()
    {
        static std::string xScript = "Script";    //string
        return xScript;
    }
    static const std::string& Extend()
    {
        static std::string xExtend = "Extend";    //string
        return xExtend;
    }
    static const std::string& Icon()
    {
        static std::string xIcon = "Icon";    //string
        return xIcon;
    }
    static const std::string& ShowName()
    {
        static std::string xShowName = "ShowName";    //string
        return xShowName;
    }
    static const std::string& HeroTye()
    {
        static std::string xHeroTye = "HeroTye";    //int
        return xHeroTye;
    }
    //DataTables

};

class Language
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xLanguage = "Language";
        return xLanguage;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& English()
    {
        static std::string xEnglish = "English";    //string
        return xEnglish;
    }
    static const std::string& Chinese()
    {
        static std::string xChinese = "Chinese";    //string
        return xChinese;
    }
    //DataTables

};

class Map
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xMap = "Map";
        return xMap;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& MaxCount()
    {
        static std::string xMaxCount = "MaxCount";    //int
        return xMaxCount;
    }
    static const std::string& InComeGold()
    {
        static std::string xInComeGold = "InComeGold";    //int
        return xInComeGold;
    }
    static const std::string& InComeDiamond()
    {
        static std::string xInComeDiamond = "InComeDiamond";    //int
        return xInComeDiamond;
    }
    static const std::string& InComeOil()
    {
        static std::string xInComeOil = "InComeOil";    //int
        return xInComeOil;
    }
    static const std::string& X()
    {
        static std::string xX = "X";    //int
        return xX;
    }
    static const std::string& Z()
    {
        static std::string xZ = "Z";    //int
        return xZ;
    }
    static const std::string& MapLevel()
    {
        static std::string xMapLevel = "MapLevel";    //int
        return xMapLevel;
    }
    //DataTables
    static const std::string& R_Station()
    {
        static std::string xStation = "Station";
        return xStation;
    }

    enum Station
    {
        Station_GUID        = 0, //GUID -> object
        Station_GuildID     = 1, //GuildID -> object
        Station_GuildName       = 2, //GuildName -> string
        Station_Level       = 3, //Level -> int
        Station_Title       = 4, //Title -> string
        Station_Slogan      = 5, //Slogan -> string
        Station_State       = 6, //State -> int
        Station_CurMemberCount      = 7, //CurMemberCount -> int
        Station_WinCount        = 8, //WinCount -> int

    };

};

class NPC
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xNPC = "NPC";
        return xNPC;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& SeedID()
    {
        static std::string xSeedID = "SeedID";    //string
        return xSeedID;
    }
    static const std::string& VIPEXP()
    {
        static std::string xVIPEXP = "VIPEXP";    //int
        return xVIPEXP;
    }
    static const std::string& EXP()
    {
        static std::string xEXP = "EXP";    //int
        return xEXP;
    }
    static const std::string& HP()
    {
        static std::string xHP = "HP";    //int
        return xHP;
    }
    static const std::string& SP()
    {
        static std::string xSP = "SP";    //int
        return xSP;
    }
    static const std::string& MP()
    {
        static std::string xMP = "MP";    //int
        return xMP;
    }
    static const std::string& Gold()
    {
        static std::string xGold = "Gold";    //int
        return xGold;
    }
    static const std::string& Money()
    {
        static std::string xMoney = "Money";    //int
        return xMoney;
    }
    static const std::string& X()
    {
        static std::string xX = "X";    //float
        return xX;
    }
    static const std::string& Y()
    {
        static std::string xY = "Y";    //float
        return xY;
    }
    static const std::string& Z()
    {
        static std::string xZ = "Z";    //float
        return xZ;
    }
    static const std::string& TargetX()
    {
        static std::string xTargetX = "TargetX";    //float
        return xTargetX;
    }
    static const std::string& TargetY()
    {
        static std::string xTargetY = "TargetY";    //float
        return xTargetY;
    }
    static const std::string& Prefab()
    {
        static std::string xPrefab = "Prefab";    //string
        return xPrefab;
    }
    static const std::string& MoveType()
    {
        static std::string xMoveType = "MoveType";    //int
        return xMoveType;
    }
    static const std::string& AtkDis()
    {
        static std::string xAtkDis = "AtkDis";    //float
        return xAtkDis;
    }
    static const std::string& DropPackList()
    {
        static std::string xDropPackList = "DropPackList";    //string
        return xDropPackList;
    }
    static const std::string& SkillIDRef()
    {
        static std::string xSkillIDRef = "SkillIDRef";    //string
        return xSkillIDRef;
    }
    static const std::string& Height()
    {
        static std::string xHeight = "Height";    //float
        return xHeight;
    }
    static const std::string& EffectData()
    {
        static std::string xEffectData = "EffectData";    //string
        return xEffectData;
    }
    static const std::string& ConsumeData()
    {
        static std::string xConsumeData = "ConsumeData";    //string
        return xConsumeData;
    }
    static const std::string& LastAttacker()
    {
        static std::string xLastAttacker = "LastAttacker";    //object
        return xLastAttacker;
    }
    static const std::string& ShowName()
    {
        static std::string xShowName = "ShowName";    //string
        return xShowName;
    }
    static const std::string& EquipIDRef()
    {
        static std::string xEquipIDRef = "EquipIDRef";    //string
        return xEquipIDRef;
    }
    static const std::string& Icon()
    {
        static std::string xIcon = "Icon";    //string
        return xIcon;
    }
    static const std::string& ShowCard()
    {
        static std::string xShowCard = "ShowCard";    //string
        return xShowCard;
    }
    static const std::string& HeroType()
    {
        static std::string xHeroType = "HeroType";    //int
        return xHeroType;
    }
    static const std::string& Camp()
    {
        static std::string xCamp = "Camp";    //int
        return xCamp;
    }
    static const std::string& MasterID()
    {
        static std::string xMasterID = "MasterID";    //object
        return xMasterID;
    }
    static const std::string& NPCType()
    {
        static std::string xNPCType = "NPCType";    //int
        return xNPCType;
    }
    static const std::string& SUCKBLOOD()
    {
        static std::string xSUCKBLOOD = "SUCKBLOOD";    //int
        return xSUCKBLOOD;
    }
    static const std::string& REFLECTDAMAGE()
    {
        static std::string xREFLECTDAMAGE = "REFLECTDAMAGE";    //int
        return xREFLECTDAMAGE;
    }
    static const std::string& CRITICAL()
    {
        static std::string xCRITICAL = "CRITICAL";    //int
        return xCRITICAL;
    }
    static const std::string& MAXHP()
    {
        static std::string xMAXHP = "MAXHP";    //int
        return xMAXHP;
    }
    static const std::string& MAXMP()
    {
        static std::string xMAXMP = "MAXMP";    //int
        return xMAXMP;
    }
    static const std::string& MAXSP()
    {
        static std::string xMAXSP = "MAXSP";    //int
        return xMAXSP;
    }
    static const std::string& HPREGEN()
    {
        static std::string xHPREGEN = "HPREGEN";    //int
        return xHPREGEN;
    }
    static const std::string& SPREGEN()
    {
        static std::string xSPREGEN = "SPREGEN";    //int
        return xSPREGEN;
    }
    static const std::string& MPREGEN()
    {
        static std::string xMPREGEN = "MPREGEN";    //int
        return xMPREGEN;
    }
    static const std::string& ATK_VALUE()
    {
        static std::string xATK_VALUE = "ATK_VALUE";    //int
        return xATK_VALUE;
    }
    static const std::string& DEF_VALUE()
    {
        static std::string xDEF_VALUE = "DEF_VALUE";    //int
        return xDEF_VALUE;
    }
    static const std::string& MOVE_SPEED()
    {
        static std::string xMOVE_SPEED = "MOVE_SPEED";    //int
        return xMOVE_SPEED;
    }
    static const std::string& ATK_SPEED()
    {
        static std::string xATK_SPEED = "ATK_SPEED";    //int
        return xATK_SPEED;
    }
    static const std::string& ATK_FIRE()
    {
        static std::string xATK_FIRE = "ATK_FIRE";    //int
        return xATK_FIRE;
    }
    static const std::string& ATK_LIGHT()
    {
        static std::string xATK_LIGHT = "ATK_LIGHT";    //int
        return xATK_LIGHT;
    }
    static const std::string& ATK_WIND()
    {
        static std::string xATK_WIND = "ATK_WIND";    //int
        return xATK_WIND;
    }
    static const std::string& ATK_ICE()
    {
        static std::string xATK_ICE = "ATK_ICE";    //int
        return xATK_ICE;
    }
    static const std::string& ATK_POISON()
    {
        static std::string xATK_POISON = "ATK_POISON";    //int
        return xATK_POISON;
    }
    static const std::string& DEF_FIRE()
    {
        static std::string xDEF_FIRE = "DEF_FIRE";    //int
        return xDEF_FIRE;
    }
    static const std::string& DEF_LIGHT()
    {
        static std::string xDEF_LIGHT = "DEF_LIGHT";    //int
        return xDEF_LIGHT;
    }
    static const std::string& DEF_WIND()
    {
        static std::string xDEF_WIND = "DEF_WIND";    //int
        return xDEF_WIND;
    }
    static const std::string& DEF_ICE()
    {
        static std::string xDEF_ICE = "DEF_ICE";    //int
        return xDEF_ICE;
    }
    static const std::string& DEF_POISON()
    {
        static std::string xDEF_POISON = "DEF_POISON";    //int
        return xDEF_POISON;
    }
    static const std::string& DIZZY_GATE()
    {
        static std::string xDIZZY_GATE = "DIZZY_GATE";    //int
        return xDIZZY_GATE;
    }
    static const std::string& MOVE_GATE()
    {
        static std::string xMOVE_GATE = "MOVE_GATE";    //int
        return xMOVE_GATE;
    }
    static const std::string& SKILL_GATE()
    {
        static std::string xSKILL_GATE = "SKILL_GATE";    //int
        return xSKILL_GATE;
    }
    static const std::string& PHYSICAL_GATE()
    {
        static std::string xPHYSICAL_GATE = "PHYSICAL_GATE";    //int
        return xPHYSICAL_GATE;
    }
    static const std::string& MAGIC_GATE()
    {
        static std::string xMAGIC_GATE = "MAGIC_GATE";    //int
        return xMAGIC_GATE;
    }
    static const std::string& BUFF_GATE()
    {
        static std::string xBUFF_GATE = "BUFF_GATE";    //int
        return xBUFF_GATE;
    }
    //DataTables

};

class Player
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xPlayer = "Player";
        return xPlayer;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& Name()
    {
        static std::string xName = "Name";    //string
        return xName;
    }
    static const std::string& Sex()
    {
        static std::string xSex = "Sex";    //int
        return xSex;
    }
    static const std::string& Race()
    {
        static std::string xRace = "Race";    //int
        return xRace;
    }
    static const std::string& Camp()
    {
        static std::string xCamp = "Camp";    //int
        return xCamp;
    }
    static const std::string& LastContainerID()
    {
        static std::string xLastContainerID = "LastContainerID";    //int
        return xLastContainerID;
    }
    static const std::string& Level()
    {
        static std::string xLevel = "Level";    //int
        return xLevel;
    }
    static const std::string& ShowName()
    {
        static std::string xShowName = "ShowName";    //string
        return xShowName;
    }
    static const std::string& PrefabPath()
    {
        static std::string xPrefabPath = "PrefabPath";    //string
        return xPrefabPath;
    }
    static const std::string& FirstTarget()
    {
        static std::string xFirstTarget = "FirstTarget";    //object
        return xFirstTarget;
    }
    static const std::string& CharType()
    {
        static std::string xCharType = "CharType";    //int
        return xCharType;
    }
    static const std::string& Job()
    {
        static std::string xJob = "Job";    //int
        return xJob;
    }
    static const std::string& VIPLevel()
    {
        static std::string xVIPLevel = "VIPLevel";    //int
        return xVIPLevel;
    }
    static const std::string& VIPEXP()
    {
        static std::string xVIPEXP = "VIPEXP";    //int
        return xVIPEXP;
    }
    static const std::string& EXP()
    {
        static std::string xEXP = "EXP";    //int
        return xEXP;
    }
    static const std::string& HP()
    {
        static std::string xHP = "HP";    //int
        return xHP;
    }
    static const std::string& SP()
    {
        static std::string xSP = "SP";    //int
        return xSP;
    }
    static const std::string& MP()
    {
        static std::string xMP = "MP";    //int
        return xMP;
    }
    static const std::string& Gold()
    {
        static std::string xGold = "Gold";    //int
        return xGold;
    }
    static const std::string& Money()
    {
        static std::string xMoney = "Money";    //int
        return xMoney;
    }
    static const std::string& Account()
    {
        static std::string xAccount = "Account";    //string
        return xAccount;
    }
    static const std::string& ConnectKey()
    {
        static std::string xConnectKey = "ConnectKey";    //string
        return xConnectKey;
    }
    static const std::string& MAXEXP()
    {
        static std::string xMAXEXP = "MAXEXP";    //int
        return xMAXEXP;
    }
    static const std::string& RELIVE_SOUL()
    {
        static std::string xRELIVE_SOUL = "RELIVE_SOUL";    //int
        return xRELIVE_SOUL;
    }
    static const std::string& ATK_PVP()
    {
        static std::string xATK_PVP = "ATK_PVP";    //int
        return xATK_PVP;
    }
    static const std::string& DEF_PVP()
    {
        static std::string xDEF_PVP = "DEF_PVP";    //int
        return xDEF_PVP;
    }
    static const std::string& OnlineCount()
    {
        static std::string xOnlineCount = "OnlineCount";    //int
        return xOnlineCount;
    }
    static const std::string& TotalTime()
    {
        static std::string xTotalTime = "TotalTime";    //int
        return xTotalTime;
    }
    static const std::string& LastOfflineTime()
    {
        static std::string xLastOfflineTime = "LastOfflineTime";    //object
        return xLastOfflineTime;
    }
    static const std::string& OnlineTime()
    {
        static std::string xOnlineTime = "OnlineTime";    //object
        return xOnlineTime;
    }
    static const std::string& TotalLineTime()
    {
        static std::string xTotalLineTime = "TotalLineTime";    //object
        return xTotalLineTime;
    }
    static const std::string& GMLevel()
    {
        static std::string xGMLevel = "GMLevel";    //int
        return xGMLevel;
    }
    static const std::string& LoadPropertyFinish()
    {
        static std::string xLoadPropertyFinish = "LoadPropertyFinish";    //int
        return xLoadPropertyFinish;
    }
    static const std::string& GameID()
    {
        static std::string xGameID = "GameID";    //int
        return xGameID;
    }
    static const std::string& GateID()
    {
        static std::string xGateID = "GateID";    //int
        return xGateID;
    }
    static const std::string& GuildID()
    {
        static std::string xGuildID = "GuildID";    //object
        return xGuildID;
    }
    static const std::string& TeamID()
    {
        static std::string xTeamID = "TeamID";    //object
        return xTeamID;
    }
    static const std::string& SUCKBLOOD()
    {
        static std::string xSUCKBLOOD = "SUCKBLOOD";    //int
        return xSUCKBLOOD;
    }
    static const std::string& REFLECTDAMAGE()
    {
        static std::string xREFLECTDAMAGE = "REFLECTDAMAGE";    //int
        return xREFLECTDAMAGE;
    }
    static const std::string& CRITICAL()
    {
        static std::string xCRITICAL = "CRITICAL";    //int
        return xCRITICAL;
    }
    static const std::string& MAXHP()
    {
        static std::string xMAXHP = "MAXHP";    //int
        return xMAXHP;
    }
    static const std::string& MAXMP()
    {
        static std::string xMAXMP = "MAXMP";    //int
        return xMAXMP;
    }
    static const std::string& MAXSP()
    {
        static std::string xMAXSP = "MAXSP";    //int
        return xMAXSP;
    }
    static const std::string& HPREGEN()
    {
        static std::string xHPREGEN = "HPREGEN";    //int
        return xHPREGEN;
    }
    static const std::string& SPREGEN()
    {
        static std::string xSPREGEN = "SPREGEN";    //int
        return xSPREGEN;
    }
    static const std::string& MPREGEN()
    {
        static std::string xMPREGEN = "MPREGEN";    //int
        return xMPREGEN;
    }
    static const std::string& ATK_VALUE()
    {
        static std::string xATK_VALUE = "ATK_VALUE";    //int
        return xATK_VALUE;
    }
    static const std::string& DEF_VALUE()
    {
        static std::string xDEF_VALUE = "DEF_VALUE";    //int
        return xDEF_VALUE;
    }
    static const std::string& MOVE_SPEED()
    {
        static std::string xMOVE_SPEED = "MOVE_SPEED";    //int
        return xMOVE_SPEED;
    }
    static const std::string& ATK_SPEED()
    {
        static std::string xATK_SPEED = "ATK_SPEED";    //int
        return xATK_SPEED;
    }
    static const std::string& ATK_FIRE()
    {
        static std::string xATK_FIRE = "ATK_FIRE";    //int
        return xATK_FIRE;
    }
    static const std::string& ATK_LIGHT()
    {
        static std::string xATK_LIGHT = "ATK_LIGHT";    //int
        return xATK_LIGHT;
    }
    static const std::string& ATK_WIND()
    {
        static std::string xATK_WIND = "ATK_WIND";    //int
        return xATK_WIND;
    }
    static const std::string& ATK_ICE()
    {
        static std::string xATK_ICE = "ATK_ICE";    //int
        return xATK_ICE;
    }
    static const std::string& ATK_POISON()
    {
        static std::string xATK_POISON = "ATK_POISON";    //int
        return xATK_POISON;
    }
    static const std::string& DEF_FIRE()
    {
        static std::string xDEF_FIRE = "DEF_FIRE";    //int
        return xDEF_FIRE;
    }
    static const std::string& DEF_LIGHT()
    {
        static std::string xDEF_LIGHT = "DEF_LIGHT";    //int
        return xDEF_LIGHT;
    }
    static const std::string& DEF_WIND()
    {
        static std::string xDEF_WIND = "DEF_WIND";    //int
        return xDEF_WIND;
    }
    static const std::string& DEF_ICE()
    {
        static std::string xDEF_ICE = "DEF_ICE";    //int
        return xDEF_ICE;
    }
    static const std::string& DEF_POISON()
    {
        static std::string xDEF_POISON = "DEF_POISON";    //int
        return xDEF_POISON;
    }
    static const std::string& DIZZY_GATE()
    {
        static std::string xDIZZY_GATE = "DIZZY_GATE";    //int
        return xDIZZY_GATE;
    }
    static const std::string& MOVE_GATE()
    {
        static std::string xMOVE_GATE = "MOVE_GATE";    //int
        return xMOVE_GATE;
    }
    static const std::string& SKILL_GATE()
    {
        static std::string xSKILL_GATE = "SKILL_GATE";    //int
        return xSKILL_GATE;
    }
    static const std::string& PHYSICAL_GATE()
    {
        static std::string xPHYSICAL_GATE = "PHYSICAL_GATE";    //int
        return xPHYSICAL_GATE;
    }
    static const std::string& MAGIC_GATE()
    {
        static std::string xMAGIC_GATE = "MAGIC_GATE";    //int
        return xMAGIC_GATE;
    }
    static const std::string& BUFF_GATE()
    {
        static std::string xBUFF_GATE = "BUFF_GATE";    //int
        return xBUFF_GATE;
    }
    //DataTables
    static const std::string& R_PlayerHero()
    {
        static std::string xPlayerHero = "PlayerHero";
        return xPlayerHero;
    }
    static const std::string& R_PlayerFightHero()
    {
        static std::string xPlayerFightHero = "PlayerFightHero";
        return xPlayerFightHero;
    }
    static const std::string& R_HeroPropertyValue()
    {
        static std::string xHeroPropertyValue = "HeroPropertyValue";
        return xHeroPropertyValue;
    }
    static const std::string& R_BagEquipList()
    {
        static std::string xBagEquipList = "BagEquipList";
        return xBagEquipList;
    }
    static const std::string& R_BagItemList()
    {
        static std::string xBagItemList = "BagItemList";
        return xBagItemList;
    }
    static const std::string& R_CommPropertyValue()
    {
        static std::string xCommPropertyValue = "CommPropertyValue";
        return xCommPropertyValue;
    }
    static const std::string& R_TaskMonsterList()
    {
        static std::string xTaskMonsterList = "TaskMonsterList";
        return xTaskMonsterList;
    }
    static const std::string& R_TaskList()
    {
        static std::string xTaskList = "TaskList";
        return xTaskList;
    }

    enum PlayerHero
    {
        PlayerHero_GUID     = 0, //GUID -> object
        PlayerHero_ConfigID     = 1, //ConfigID -> string
        PlayerHero_Level        = 2, //Level -> int
        PlayerHero_Exp      = 3, //Exp -> int
        PlayerHero_Star     = 4, //Star -> int
        PlayerHero_Equip1       = 5, //Equip1 -> object
        PlayerHero_Equip2       = 6, //Equip2 -> object
        PlayerHero_Equip3       = 7, //Equip3 -> object
        PlayerHero_Equip4       = 8, //Equip4 -> object
        PlayerHero_Equip5       = 9, //Equip5 -> object
        PlayerHero_Equip6       = 10, //Equip6 -> object
        PlayerHero_Talent1      = 11, //Talent1 -> string
        PlayerHero_Talent2      = 12, //Talent2 -> string
        PlayerHero_Talent3      = 13, //Talent3 -> string
        PlayerHero_Talent4      = 14, //Talent4 -> string
        PlayerHero_Talent5      = 15, //Talent5 -> string
        PlayerHero_Skill1       = 16, //Skill1 -> string
        PlayerHero_Skill2       = 17, //Skill2 -> string
        PlayerHero_Skill3       = 18, //Skill3 -> string
        PlayerHero_Skill4       = 19, //Skill4 -> string
        PlayerHero_Skill5       = 20, //Skill5 -> string
        PlayerHero_FightSkill       = 21, //FightSkill -> string

    };

    enum PlayerFightHero
    {
        PlayerFightHero_GUID        = 0, //GUID -> object
        PlayerFightHero_FightPos        = 1, //FightPos -> int

    };

    enum HeroPropertyValue
    {
        HeroPropertyValue_SUCKBLOOD     = 0, //SUCKBLOOD -> int
        HeroPropertyValue_REFLECTDAMAGE     = 1, //REFLECTDAMAGE -> int
        HeroPropertyValue_CRITICAL      = 2, //CRITICAL -> int
        HeroPropertyValue_MAXHP     = 3, //MAXHP -> int
        HeroPropertyValue_MAXMP     = 4, //MAXMP -> int
        HeroPropertyValue_MAXSP     = 5, //MAXSP -> int
        HeroPropertyValue_HPREGEN       = 6, //HPREGEN -> int
        HeroPropertyValue_SPREGEN       = 7, //SPREGEN -> int
        HeroPropertyValue_MPREGEN       = 8, //MPREGEN -> int
        HeroPropertyValue_ATK_VALUE     = 9, //ATK_VALUE -> int
        HeroPropertyValue_DEF_VALUE     = 10, //DEF_VALUE -> int
        HeroPropertyValue_MOVE_SPEED        = 11, //MOVE_SPEED -> int
        HeroPropertyValue_ATK_SPEED     = 12, //ATK_SPEED -> int
        HeroPropertyValue_ATK_FIRE      = 13, //ATK_FIRE -> int
        HeroPropertyValue_ATK_LIGHT     = 14, //ATK_LIGHT -> int
        HeroPropertyValue_ATK_WIND      = 15, //ATK_WIND -> int
        HeroPropertyValue_ATK_ICE       = 16, //ATK_ICE -> int
        HeroPropertyValue_ATK_POISON        = 17, //ATK_POISON -> int
        HeroPropertyValue_DEF_FIRE      = 18, //DEF_FIRE -> int
        HeroPropertyValue_DEF_LIGHT     = 19, //DEF_LIGHT -> int
        HeroPropertyValue_DEF_WIND      = 20, //DEF_WIND -> int
        HeroPropertyValue_DEF_ICE       = 21, //DEF_ICE -> int
        HeroPropertyValue_DEF_POISON        = 22, //DEF_POISON -> int
        HeroPropertyValue_DIZZY_GATE        = 23, //DIZZY_GATE -> int
        HeroPropertyValue_MOVE_GATE     = 24, //MOVE_GATE -> int
        HeroPropertyValue_SKILL_GATE        = 25, //SKILL_GATE -> int
        HeroPropertyValue_PHYSICAL_GATE     = 26, //PHYSICAL_GATE -> int
        HeroPropertyValue_MAGIC_GATE        = 27, //MAGIC_GATE -> int
        HeroPropertyValue_BUFF_GATE     = 28, //BUFF_GATE -> int

    };

    enum BagEquipList
    {
        BagEquipList_GUID       = 0, //GUID -> object
        BagEquipList_WearGUID       = 1, //WearGUID -> object
        BagEquipList_ConfigID       = 2, //ConfigID -> string
        BagEquipList_ExpiredType        = 3, //ExpiredType -> int
        BagEquipList_Date       = 4, //Date -> int
        BagEquipList_RandPropertyID     = 5, //RandPropertyID -> string
        BagEquipList_SlotCount      = 6, //SlotCount -> int
        BagEquipList_InlayStone1        = 7, //InlayStone1 -> string
        BagEquipList_InlayStone2        = 8, //InlayStone2 -> string
        BagEquipList_InlayStone3        = 9, //InlayStone3 -> string
        BagEquipList_InlayStone4        = 10, //InlayStone4 -> string
        BagEquipList_InlayStone5        = 11, //InlayStone5 -> string
        BagEquipList_InlayStone6        = 12, //InlayStone6 -> string
        BagEquipList_InlayStone7        = 13, //InlayStone7 -> string
        BagEquipList_InlayStone8        = 14, //InlayStone8 -> string
        BagEquipList_InlayStone9        = 15, //InlayStone9 -> string
        BagEquipList_InlayStone10       = 16, //InlayStone10 -> string
        BagEquipList_IntensifyLevel     = 17, //IntensifyLevel -> string
        BagEquipList_ElementLevel1_FIRE     = 18, //ElementLevel1_FIRE -> int
        BagEquipList_ElementLevel2_LIGHT        = 19, //ElementLevel2_LIGHT -> int
        BagEquipList_ElementLevel3_Wind     = 20, //ElementLevel3_Wind -> int
        BagEquipList_ElementLevel4_ICE      = 21, //ElementLevel4_ICE -> int
        BagEquipList_ElementLevel5_POISON       = 22, //ElementLevel5_POISON -> int

    };

    enum BagItemList
    {
        BagItemList_ConfigID        = 0, //ConfigID -> string
        BagItemList_ItemCount       = 1, //ItemCount -> int
        BagItemList_Bound       = 2, //Bound -> int
        BagItemList_ExpiredType     = 3, //ExpiredType -> int
        BagItemList_Date        = 4, //Date -> int

    };

    enum CommPropertyValue
    {
        CommPropertyValue_SUCKBLOOD     = 0, //SUCKBLOOD -> int
        CommPropertyValue_REFLECTDAMAGE     = 1, //REFLECTDAMAGE -> int
        CommPropertyValue_CRITICAL      = 2, //CRITICAL -> int
        CommPropertyValue_MAXHP     = 3, //MAXHP -> int
        CommPropertyValue_MAXMP     = 4, //MAXMP -> int
        CommPropertyValue_MAXSP     = 5, //MAXSP -> int
        CommPropertyValue_HPREGEN       = 6, //HPREGEN -> int
        CommPropertyValue_SPREGEN       = 7, //SPREGEN -> int
        CommPropertyValue_MPREGEN       = 8, //MPREGEN -> int
        CommPropertyValue_ATK_VALUE     = 9, //ATK_VALUE -> int
        CommPropertyValue_DEF_VALUE     = 10, //DEF_VALUE -> int
        CommPropertyValue_MOVE_SPEED        = 11, //MOVE_SPEED -> int
        CommPropertyValue_ATK_SPEED     = 12, //ATK_SPEED -> int
        CommPropertyValue_ATK_FIRE      = 13, //ATK_FIRE -> int
        CommPropertyValue_ATK_LIGHT     = 14, //ATK_LIGHT -> int
        CommPropertyValue_ATK_WIND      = 15, //ATK_WIND -> int
        CommPropertyValue_ATK_ICE       = 16, //ATK_ICE -> int
        CommPropertyValue_ATK_POISON        = 17, //ATK_POISON -> int
        CommPropertyValue_DEF_FIRE      = 18, //DEF_FIRE -> int
        CommPropertyValue_DEF_LIGHT     = 19, //DEF_LIGHT -> int
        CommPropertyValue_DEF_WIND      = 20, //DEF_WIND -> int
        CommPropertyValue_DEF_ICE       = 21, //DEF_ICE -> int
        CommPropertyValue_DEF_POISON        = 22, //DEF_POISON -> int
        CommPropertyValue_DIZZY_GATE        = 23, //DIZZY_GATE -> int
        CommPropertyValue_MOVE_GATE     = 24, //MOVE_GATE -> int
        CommPropertyValue_SKILL_GATE        = 25, //SKILL_GATE -> int
        CommPropertyValue_PHYSICAL_GATE     = 26, //PHYSICAL_GATE -> int
        CommPropertyValue_MAGIC_GATE        = 27, //MAGIC_GATE -> int
        CommPropertyValue_BUFF_GATE     = 28, //BUFF_GATE -> int

    };

    enum TaskMonsterList
    {
        TaskMonsterList_MonsterID       = 0, //MonsterID -> string
        TaskMonsterList_CurrentKillCount        = 1, //CurrentKillCount -> int
        TaskMonsterList_RequireKillCount        = 2, //RequireKillCount -> int
        TaskMonsterList_TaskID      = 3, //TaskID -> string

    };

    enum TaskList
    {
        TaskList_TaskID     = 0, //TaskID -> string
        TaskList_TaskStatus     = 1, //TaskStatus -> int
        TaskList_Process        = 2, //Process -> int

    };

};

class Scene
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xScene = "Scene";
        return xScene;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& SceneName()
    {
        static std::string xSceneName = "SceneName";    //string
        return xSceneName;
    }
    static const std::string& SceneShowName()
    {
        static std::string xSceneShowName = "SceneShowName";    //string
        return xSceneShowName;
    }
    static const std::string& MaxGroup()
    {
        static std::string xMaxGroup = "MaxGroup";    //int
        return xMaxGroup;
    }
    static const std::string& MaxGroupPlayers()
    {
        static std::string xMaxGroupPlayers = "MaxGroupPlayers";    //int
        return xMaxGroupPlayers;
    }
    static const std::string& FilePath()
    {
        static std::string xFilePath = "FilePath";    //string
        return xFilePath;
    }
    static const std::string& RelivePos()
    {
        static std::string xRelivePos = "RelivePos";    //string
        return xRelivePos;
    }
    static const std::string& Width()
    {
        static std::string xWidth = "Width";    //int
        return xWidth;
    }
    static const std::string& SoundList()
    {
        static std::string xSoundList = "SoundList";    //string
        return xSoundList;
    }
    static const std::string& Share()
    {
        static std::string xShare = "Share";    //int
        return xShare;
    }
    static const std::string& CanClone()
    {
        static std::string xCanClone = "CanClone";    //int
        return xCanClone;
    }
    static const std::string& ActorID()
    {
        static std::string xActorID = "ActorID";    //int
        return xActorID;
    }
    static const std::string& LoadingUI()
    {
        static std::string xLoadingUI = "LoadingUI";    //string
        return xLoadingUI;
    }
    static const std::string& CamOffestPos()
    {
        static std::string xCamOffestPos = "CamOffestPos";    //string
        return xCamOffestPos;
    }
    static const std::string& CamOffestRot()
    {
        static std::string xCamOffestRot = "CamOffestRot";    //string
        return xCamOffestRot;
    }
    static const std::string& SyncObject()
    {
        static std::string xSyncObject = "SyncObject";    //int
        return xSyncObject;
    }
    //DataTables

};

class Server
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xServer = "Server";
        return xServer;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& ServerID()
    {
        static std::string xServerID = "ServerID";    //int
        return xServerID;
    }
    static const std::string& Name()
    {
        static std::string xName = "Name";    //string
        return xName;
    }
    static const std::string& MaxOnline()
    {
        static std::string xMaxOnline = "MaxOnline";    //int
        return xMaxOnline;
    }
    static const std::string& CpuCount()
    {
        static std::string xCpuCount = "CpuCount";    //int
        return xCpuCount;
    }
    static const std::string& IP()
    {
        static std::string xIP = "IP";    //string
        return xIP;
    }
    static const std::string& Port()
    {
        static std::string xPort = "Port";    //int
        return xPort;
    }
    static const std::string& Type()
    {
        static std::string xType = "Type";    //int
        return xType;
    }
    //DataTables

};

class Shop
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xShop = "Shop";
        return xShop;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& Type()
    {
        static std::string xType = "Type";    //int
        return xType;
    }
    static const std::string& ItemID()
    {
        static std::string xItemID = "ItemID";    //string
        return xItemID;
    }
    static const std::string& Gold()
    {
        static std::string xGold = "Gold";    //int
        return xGold;
    }
    static const std::string& Steel()
    {
        static std::string xSteel = "Steel";    //int
        return xSteel;
    }
    static const std::string& Stone()
    {
        static std::string xStone = "Stone";    //int
        return xStone;
    }
    static const std::string& Diamond()
    {
        static std::string xDiamond = "Diamond";    //int
        return xDiamond;
    }
    static const std::string& Level()
    {
        static std::string xLevel = "Level";    //int
        return xLevel;
    }
    //DataTables

};

class Skill
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xSkill = "Skill";
        return xSkill;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& SkillType()
    {
        static std::string xSkillType = "SkillType";    //int
        return xSkillType;
    }
    static const std::string& AnimaState()
    {
        static std::string xAnimaState = "AnimaState";    //int
        return xAnimaState;
    }
    static const std::string& ShowName()
    {
        static std::string xShowName = "ShowName";    //string
        return xShowName;
    }
    static const std::string& Desc()
    {
        static std::string xDesc = "Desc";    //string
        return xDesc;
    }
    static const std::string& ConsumeProperty()
    {
        static std::string xConsumeProperty = "ConsumeProperty";    //string
        return xConsumeProperty;
    }
    static const std::string& ConsumeValue()
    {
        static std::string xConsumeValue = "ConsumeValue";    //string
        return xConsumeValue;
    }
    static const std::string& ConsumeType()
    {
        static std::string xConsumeType = "ConsumeType";    //int
        return xConsumeType;
    }
    static const std::string& DamageProperty()
    {
        static std::string xDamageProperty = "DamageProperty";    //string
        return xDamageProperty;
    }
    static const std::string& DamageValue()
    {
        static std::string xDamageValue = "DamageValue";    //string
        return xDamageValue;
    }
    static const std::string& DamageType()
    {
        static std::string xDamageType = "DamageType";    //int
        return xDamageType;
    }
    static const std::string& GetBuffList()
    {
        static std::string xGetBuffList = "GetBuffList";    //string
        return xGetBuffList;
    }
    static const std::string& SendBuffList()
    {
        static std::string xSendBuffList = "SendBuffList";    //string
        return xSendBuffList;
    }
    static const std::string& CoolDownTime()
    {
        static std::string xCoolDownTime = "CoolDownTime";    //float
        return xCoolDownTime;
    }
    static const std::string& RequireDistance()
    {
        static std::string xRequireDistance = "RequireDistance";    //float
        return xRequireDistance;
    }
    static const std::string& DamageDistance()
    {
        static std::string xDamageDistance = "DamageDistance";    //float
        return xDamageDistance;
    }
    static const std::string& TargetType()
    {
        static std::string xTargetType = "TargetType";    //int
        return xTargetType;
    }
    static const std::string& NewObject()
    {
        static std::string xNewObject = "NewObject";    //string
        return xNewObject;
    }
    static const std::string& Icon()
    {
        static std::string xIcon = "Icon";    //string
        return xIcon;
    }
    static const std::string& Atlas()
    {
        static std::string xAtlas = "Atlas";    //string
        return xAtlas;
    }
    static const std::string& UpLevel()
    {
        static std::string xUpLevel = "UpLevel";    //int
        return xUpLevel;
    }
    static const std::string& AfterUpID()
    {
        static std::string xAfterUpID = "AfterUpID";    //string
        return xAfterUpID;
    }
    static const std::string& PlayerSkill()
    {
        static std::string xPlayerSkill = "PlayerSkill";    //int
        return xPlayerSkill;
    }
    static const std::string& AtkDis()
    {
        static std::string xAtkDis = "AtkDis";    //float
        return xAtkDis;
    }
    static const std::string& NeedTar()
    {
        static std::string xNeedTar = "NeedTar";    //int
        return xNeedTar;
    }
    static const std::string& DefaultHitTime()
    {
        static std::string xDefaultHitTime = "DefaultHitTime";    //float
        return xDefaultHitTime;
    }
    //DataTables

};

class SkillRef
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xSkillRef = "SkillRef";
        return xSkillRef;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& NORMALATTACK1()
    {
        static std::string xNORMALATTACK1 = "NORMALATTACK1";    //string
        return xNORMALATTACK1;
    }
    static const std::string& NORMALATTACK2()
    {
        static std::string xNORMALATTACK2 = "NORMALATTACK2";    //string
        return xNORMALATTACK2;
    }
    static const std::string& NORMALATTACK3()
    {
        static std::string xNORMALATTACK3 = "NORMALATTACK3";    //string
        return xNORMALATTACK3;
    }
    static const std::string& NORMALTHUMP()
    {
        static std::string xNORMALTHUMP = "NORMALTHUMP";    //string
        return xNORMALTHUMP;
    }
    static const std::string& SKILL1()
    {
        static std::string xSKILL1 = "SKILL1";    //string
        return xSKILL1;
    }
    static const std::string& SKILL2()
    {
        static std::string xSKILL2 = "SKILL2";    //string
        return xSKILL2;
    }
    static const std::string& SKILL3()
    {
        static std::string xSKILL3 = "SKILL3";    //string
        return xSKILL3;
    }
    static const std::string& SKILL4()
    {
        static std::string xSKILL4 = "SKILL4";    //string
        return xSKILL4;
    }
    static const std::string& SKILL5()
    {
        static std::string xSKILL5 = "SKILL5";    //string
        return xSKILL5;
    }
    static const std::string& SKILL6()
    {
        static std::string xSKILL6 = "SKILL6";    //string
        return xSKILL6;
    }
    static const std::string& SKILL7()
    {
        static std::string xSKILL7 = "SKILL7";    //string
        return xSKILL7;
    }
    static const std::string& SKILL8()
    {
        static std::string xSKILL8 = "SKILL8";    //string
        return xSKILL8;
    }
    static const std::string& SKILL9()
    {
        static std::string xSKILL9 = "SKILL9";    //string
        return xSKILL9;
    }
    static const std::string& SKILL10()
    {
        static std::string xSKILL10 = "SKILL10";    //string
        return xSKILL10;
    }
    //DataTables

};

class SqlServer
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xSqlServer = "SqlServer";
        return xSqlServer;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& ServerID()
    {
        static std::string xServerID = "ServerID";    //int
        return xServerID;
    }
    static const std::string& IP()
    {
        static std::string xIP = "IP";    //string
        return xIP;
    }
    static const std::string& Port()
    {
        static std::string xPort = "Port";    //int
        return xPort;
    }
    static const std::string& SqlIP()
    {
        static std::string xSqlIP = "SqlIP";    //string
        return xSqlIP;
    }
    static const std::string& SqlPort()
    {
        static std::string xSqlPort = "SqlPort";    //int
        return xSqlPort;
    }
    static const std::string& SqlUser()
    {
        static std::string xSqlUser = "SqlUser";    //string
        return xSqlUser;
    }
    static const std::string& SqlPwd()
    {
        static std::string xSqlPwd = "SqlPwd";    //string
        return xSqlPwd;
    }
    static const std::string& SqlName()
    {
        static std::string xSqlName = "SqlName";    //string
        return xSqlName;
    }
    //DataTables

};

class StateFuncResources
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xStateFuncResources = "StateFuncResources";
        return xStateFuncResources;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& Atlas_ResID()
    {
        static std::string xAtlas_ResID = "Atlas_ResID";    //string
        return xAtlas_ResID;
    }
    //DataTables

};

class StateFunction
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xStateFunction = "StateFunction";
        return xStateFunction;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& EFT_INFO()
    {
        static std::string xEFT_INFO = "EFT_INFO";    //int
        return xEFT_INFO;
    }
    static const std::string& EFT_BOOOST()
    {
        static std::string xEFT_BOOOST = "EFT_BOOOST";    //int
        return xEFT_BOOOST;
    }
    static const std::string& EFT_LVLUP()
    {
        static std::string xEFT_LVLUP = "EFT_LVLUP";    //int
        return xEFT_LVLUP;
    }
    static const std::string& EFT_CREATE_SOLDER()
    {
        static std::string xEFT_CREATE_SOLDER = "EFT_CREATE_SOLDER";    //int
        return xEFT_CREATE_SOLDER;
    }
    static const std::string& EFT_CREATE_SPEEL()
    {
        static std::string xEFT_CREATE_SPEEL = "EFT_CREATE_SPEEL";    //int
        return xEFT_CREATE_SPEEL;
    }
    static const std::string& EFT_RESEARCH()
    {
        static std::string xEFT_RESEARCH = "EFT_RESEARCH";    //int
        return xEFT_RESEARCH;
    }
    static const std::string& EFT_COLLECT_GOLD()
    {
        static std::string xEFT_COLLECT_GOLD = "EFT_COLLECT_GOLD";    //int
        return xEFT_COLLECT_GOLD;
    }
    static const std::string& EFT_COLLECT_STONE()
    {
        static std::string xEFT_COLLECT_STONE = "EFT_COLLECT_STONE";    //int
        return xEFT_COLLECT_STONE;
    }
    static const std::string& EFT_COLLECT_STEEL()
    {
        static std::string xEFT_COLLECT_STEEL = "EFT_COLLECT_STEEL";    //int
        return xEFT_COLLECT_STEEL;
    }
    static const std::string& EFT_COLLECT_DIAMOND()
    {
        static std::string xEFT_COLLECT_DIAMOND = "EFT_COLLECT_DIAMOND";    //int
        return xEFT_COLLECT_DIAMOND;
    }
    static const std::string& EFT_SELL()
    {
        static std::string xEFT_SELL = "EFT_SELL";    //int
        return xEFT_SELL;
    }
    static const std::string& EFT_REPAIR()
    {
        static std::string xEFT_REPAIR = "EFT_REPAIR";    //int
        return xEFT_REPAIR;
    }
    static const std::string& EFT_CANCEL()
    {
        static std::string xEFT_CANCEL = "EFT_CANCEL";    //int
        return xEFT_CANCEL;
    }
    static const std::string& EFT_FINISH()
    {
        static std::string xEFT_FINISH = "EFT_FINISH";    //int
        return xEFT_FINISH;
    }
    //DataTables

};

class Talent
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xTalent = "Talent";
        return xTalent;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& ShowName()
    {
        static std::string xShowName = "ShowName";    //string
        return xShowName;
    }
    static const std::string& Desc()
    {
        static std::string xDesc = "Desc";    //string
        return xDesc;
    }
    static const std::string& EffectData()
    {
        static std::string xEffectData = "EffectData";    //string
        return xEffectData;
    }
    static const std::string& Icon()
    {
        static std::string xIcon = "Icon";    //string
        return xIcon;
    }
    static const std::string& Atlas()
    {
        static std::string xAtlas = "Atlas";    //string
        return xAtlas;
    }
    static const std::string& UpLevel()
    {
        static std::string xUpLevel = "UpLevel";    //int
        return xUpLevel;
    }
    static const std::string& AfterUpID()
    {
        static std::string xAfterUpID = "AfterUpID";    //string
        return xAfterUpID;
    }
    //DataTables

};

class Task
{
public:
    //Class name
    static const std::string& ThisName()
    {
        static std::string xTask = "Task";
        return xTask;
    }
    //IObject
    static const std::string& ID()
    {
        static std::string xID = "ID";    //string
        return xID;
    }
    static const std::string& ClassName()
    {
        static std::string xClassName = "ClassName";    //string
        return xClassName;
    }
    static const std::string& SceneID()
    {
        static std::string xSceneID = "SceneID";    //int
        return xSceneID;
    }
    static const std::string& GroupID()
    {
        static std::string xGroupID = "GroupID";    //int
        return xGroupID;
    }
    static const std::string& ConfigID()
    {
        static std::string xConfigID = "ConfigID";    //string
        return xConfigID;
    }
    //DataNodes
    static const std::string& Type()
    {
        static std::string xType = "Type";    //int
        return xType;
    }
    static const std::string& NextTaskID()
    {
        static std::string xNextTaskID = "NextTaskID";    //string
        return xNextTaskID;
    }
    static const std::string& KillMonsterName()
    {
        static std::string xKillMonsterName = "KillMonsterName";    //string
        return xKillMonsterName;
    }
    static const std::string& KillCount()
    {
        static std::string xKillCount = "KillCount";    //string
        return xKillCount;
    }
    static const std::string& LevelReq()
    {
        static std::string xLevelReq = "LevelReq";    //string
        return xLevelReq;
    }
    static const std::string& AwardExp()
    {
        static std::string xAwardExp = "AwardExp";    //int
        return xAwardExp;
    }
    static const std::string& AwardGold()
    {
        static std::string xAwardGold = "AwardGold";    //int
        return xAwardGold;
    }
    static const std::string& AwardPack()
    {
        static std::string xAwardPack = "AwardPack";    //string
        return xAwardPack;
    }
    static const std::string& Desc()
    {
        static std::string xDesc = "Desc";    //string
        return xDesc;
    }
    //DataTables

};

}
