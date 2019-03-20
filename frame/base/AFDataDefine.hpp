/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
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

namespace ark
{

class IObject
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xIObject = "IObject"; return xIObject; }
	//IObject
	//DataNodes
	static const std::string& Id() { static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName() { static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID() { static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID() { static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID() { static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataTables

};

class ConsumeData
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xConsumeData = "ConsumeData"; return xConsumeData; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& VIPEXP() { static std::string xVIPEXP = "VIPEXP"; return xVIPEXP; } //int
	static const std::string& EXP() { static std::string xEXP = "EXP"; return xEXP; } //int
	static const std::string& HP() { static std::string xHP = "HP"; return xHP; } //int
	static const std::string& SP() { static std::string xSP = "SP"; return xSP; } //int
	static const std::string& MP() { static std::string xMP = "MP"; return xMP; } //int
	static const std::string& Gold() { static std::string xGold = "Gold"; return xGold; } //int
	static const std::string& Money() { static std::string xMoney = "Money"; return xMoney; } //int
	//DataTables

};

class Cost
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xCost = "Cost"; return xCost; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& CostMoney() { static std::string xCostMoney = "CostMoney"; return xCostMoney; } //int
	static const std::string& CostDiamond() { static std::string xCostDiamond = "CostDiamond"; return xCostDiamond; } //int
	static const std::string& CostVP() { static std::string xCostVP = "CostVP"; return xCostVP; } //int
	static const std::string& CostHonour() { static std::string xCostHonour = "CostHonour"; return xCostHonour; } //int
	//DataTables

};

class Equip
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xEquip = "Equip"; return xEquip; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& Sex() { static std::string xSex = "Sex"; return xSex; } //int
	static const std::string& IntensiveBuffList() { static std::string xIntensiveBuffList = "IntensiveBuffList"; return xIntensiveBuffList; } //string
	static const std::string& EnchantmentBuffList() { static std::string xEnchantmentBuffList = "EnchantmentBuffList"; return xEnchantmentBuffList; } //string
	static const std::string& SuitID() { static std::string xSuitID = "SuitID"; return xSuitID; } //int
	static const std::string& SuitBuffID() { static std::string xSuitBuffID = "SuitBuffID"; return xSuitBuffID; } //string
	static const std::string& ItemType() { static std::string xItemType = "ItemType"; return xItemType; } //int
	static const std::string& ItemSubType() { static std::string xItemSubType = "ItemSubType"; return xItemSubType; } //int
	static const std::string& Level() { static std::string xLevel = "Level"; return xLevel; } //int
	static const std::string& Job() { static std::string xJob = "Job"; return xJob; } //string
	static const std::string& Quality() { static std::string xQuality = "Quality"; return xQuality; } //int
	static const std::string& ShowName() { static std::string xShowName = "ShowName"; return xShowName; } //string
	static const std::string& Desc() { static std::string xDesc = "Desc"; return xDesc; } //string
	static const std::string& EffectData() { static std::string xEffectData = "EffectData"; return xEffectData; } //string
	static const std::string& PrefabPath() { static std::string xPrefabPath = "PrefabPath"; return xPrefabPath; } //string
	static const std::string& DropPrePath() { static std::string xDropPrePath = "DropPrePath"; return xDropPrePath; } //string
	static const std::string& BuyPrice() { static std::string xBuyPrice = "BuyPrice"; return xBuyPrice; } //int
	static const std::string& SalePrice() { static std::string xSalePrice = "SalePrice"; return xSalePrice; } //int
	static const std::string& Icon() { static std::string xIcon = "Icon"; return xIcon; } //string
	//DataTables

};

class InitProperty
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xInitProperty = "InitProperty"; return xInitProperty; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& Job() { static std::string xJob = "Job"; return xJob; } //int
	static const std::string& Level() { static std::string xLevel = "Level"; return xLevel; } //int
	static const std::string& EffectData() { static std::string xEffectData = "EffectData"; return xEffectData; } //string
	static const std::string& SkillIDRef() { static std::string xSkillIDRef = "SkillIDRef"; return xSkillIDRef; } //string
	static const std::string& ModelPtah() { static std::string xModelPtah = "ModelPtah"; return xModelPtah; } //string
	//DataTables

};

class Item
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xItem = "Item"; return xItem; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& ItemType() { static std::string xItemType = "ItemType"; return xItemType; } //int
	static const std::string& ItemSubType() { static std::string xItemSubType = "ItemSubType"; return xItemSubType; } //int
	static const std::string& Level() { static std::string xLevel = "Level"; return xLevel; } //int
	static const std::string& Job() { static std::string xJob = "Job"; return xJob; } //string
	static const std::string& Quality() { static std::string xQuality = "Quality"; return xQuality; } //int
	static const std::string& DesignDesc() { static std::string xDesignDesc = "DesignDesc"; return xDesignDesc; } //string
	static const std::string& DescID() { static std::string xDescID = "DescID"; return xDescID; } //string
	static const std::string& EffectData() { static std::string xEffectData = "EffectData"; return xEffectData; } //string
	static const std::string& ConsumeData() { static std::string xConsumeData = "ConsumeData"; return xConsumeData; } //string
	static const std::string& AwardData() { static std::string xAwardData = "AwardData"; return xAwardData; } //string
	static const std::string& AwardProperty() { static std::string xAwardProperty = "AwardProperty"; return xAwardProperty; } //int
	static const std::string& CoolDownTime() { static std::string xCoolDownTime = "CoolDownTime"; return xCoolDownTime; } //float
	static const std::string& OverlayCount() { static std::string xOverlayCount = "OverlayCount"; return xOverlayCount; } //int
	static const std::string& ExpiredType() { static std::string xExpiredType = "ExpiredType"; return xExpiredType; } //int
	static const std::string& BuyPrice() { static std::string xBuyPrice = "BuyPrice"; return xBuyPrice; } //int
	static const std::string& SalePrice() { static std::string xSalePrice = "SalePrice"; return xSalePrice; } //int
	static const std::string& Script() { static std::string xScript = "Script"; return xScript; } //string
	static const std::string& Extend() { static std::string xExtend = "Extend"; return xExtend; } //string
	static const std::string& Icon() { static std::string xIcon = "Icon"; return xIcon; } //string
	static const std::string& ShowName() { static std::string xShowName = "ShowName"; return xShowName; } //string
	static const std::string& HeroTye() { static std::string xHeroTye = "HeroTye"; return xHeroTye; } //int
	//DataTables

};

class Language
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xLanguage = "Language"; return xLanguage; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& English() { static std::string xEnglish = "English"; return xEnglish; } //string
	static const std::string& Chinese() { static std::string xChinese = "Chinese"; return xChinese; } //string
	//DataTables

};

class Map
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xMap = "Map"; return xMap; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& MaxCount() { static std::string xMaxCount = "MaxCount"; return xMaxCount; } //int
	static const std::string& InComeGold() { static std::string xInComeGold = "InComeGold"; return xInComeGold; } //int
	static const std::string& InComeDiamond() { static std::string xInComeDiamond = "InComeDiamond"; return xInComeDiamond; } //int
	static const std::string& InComeOil() { static std::string xInComeOil = "InComeOil"; return xInComeOil; } //int
	static const std::string& X() { static std::string xX = "X"; return xX; } //int
	static const std::string& Z() { static std::string xZ = "Z"; return xZ; } //int
	static const std::string& MapLevel() { static std::string xMapLevel = "MapLevel"; return xMapLevel; } //int
	//DataTables
	static const std::string& R_Station(){ static std::string xStation = "Station"; return xStation;}

	enum Station
	{
		Station_GUID		= 0, //GUID -> int64
		Station_GuildID		= 1, //GuildID -> int64
		Station_GuildName		= 2, //GuildName -> string
		Station_Level		= 3, //Level -> int
		Station_Title		= 4, //Title -> string
		Station_Slogan		= 5, //Slogan -> string
		Station_State		= 6, //State -> int
		Station_CurMemberCount		= 7, //CurMemberCount -> int
		Station_WinCount		= 8, //WinCount -> int

	};

};

class NPC
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xNPC = "NPC"; return xNPC; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& SeedID() { static std::string xSeedID = "SeedID"; return xSeedID; } //string
	static const std::string& VIPEXP() { static std::string xVIPEXP = "VIPEXP"; return xVIPEXP; } //int
	static const std::string& EXP() { static std::string xEXP = "EXP"; return xEXP; } //int
	static const std::string& HP() { static std::string xHP = "HP"; return xHP; } //int
	static const std::string& SP() { static std::string xSP = "SP"; return xSP; } //int
	static const std::string& MP() { static std::string xMP = "MP"; return xMP; } //int
	static const std::string& Gold() { static std::string xGold = "Gold"; return xGold; } //int
	static const std::string& Money() { static std::string xMoney = "Money"; return xMoney; } //int
	static const std::string& X() { static std::string xX = "X"; return xX; } //float
	static const std::string& Y() { static std::string xY = "Y"; return xY; } //float
	static const std::string& Z() { static std::string xZ = "Z"; return xZ; } //float
	static const std::string& TargetX() { static std::string xTargetX = "TargetX"; return xTargetX; } //float
	static const std::string& TargetY() { static std::string xTargetY = "TargetY"; return xTargetY; } //float
	static const std::string& Prefab() { static std::string xPrefab = "Prefab"; return xPrefab; } //string
	static const std::string& MoveType() { static std::string xMoveType = "MoveType"; return xMoveType; } //int
	static const std::string& AtkDis() { static std::string xAtkDis = "AtkDis"; return xAtkDis; } //float
	static const std::string& DropPackList() { static std::string xDropPackList = "DropPackList"; return xDropPackList; } //string
	static const std::string& SkillIDRef() { static std::string xSkillIDRef = "SkillIDRef"; return xSkillIDRef; } //string
	static const std::string& Height() { static std::string xHeight = "Height"; return xHeight; } //float
	static const std::string& EffectData() { static std::string xEffectData = "EffectData"; return xEffectData; } //string
	static const std::string& ConsumeData() { static std::string xConsumeData = "ConsumeData"; return xConsumeData; } //string
	static const std::string& LastAttacker() { static std::string xLastAttacker = "LastAttacker"; return xLastAttacker; } //int64
	static const std::string& ShowName() { static std::string xShowName = "ShowName"; return xShowName; } //string
	static const std::string& EquipIDRef() { static std::string xEquipIDRef = "EquipIDRef"; return xEquipIDRef; } //string
	static const std::string& Icon() { static std::string xIcon = "Icon"; return xIcon; } //string
	static const std::string& ShowCard() { static std::string xShowCard = "ShowCard"; return xShowCard; } //string
	static const std::string& HeroType() { static std::string xHeroType = "HeroType"; return xHeroType; } //int
	static const std::string& Camp() { static std::string xCamp = "Camp"; return xCamp; } //int
	static const std::string& MasterID() { static std::string xMasterID = "MasterID"; return xMasterID; } //int64
	static const std::string& NPCType() { static std::string xNPCType = "NPCType"; return xNPCType; } //int
	static const std::string& SUCKBLOOD() { static std::string xSUCKBLOOD = "SUCKBLOOD"; return xSUCKBLOOD; } //int
	static const std::string& REFLECTDAMAGE() { static std::string xREFLECTDAMAGE = "REFLECTDAMAGE"; return xREFLECTDAMAGE; } //int
	static const std::string& CRITICAL() { static std::string xCRITICAL = "CRITICAL"; return xCRITICAL; } //int
	static const std::string& MAXHP() { static std::string xMAXHP = "MAXHP"; return xMAXHP; } //int
	static const std::string& MAXMP() { static std::string xMAXMP = "MAXMP"; return xMAXMP; } //int
	static const std::string& MAXSP() { static std::string xMAXSP = "MAXSP"; return xMAXSP; } //int
	static const std::string& HPREGEN() { static std::string xHPREGEN = "HPREGEN"; return xHPREGEN; } //int
	static const std::string& SPREGEN() { static std::string xSPREGEN = "SPREGEN"; return xSPREGEN; } //int
	static const std::string& MPREGEN() { static std::string xMPREGEN = "MPREGEN"; return xMPREGEN; } //int
	static const std::string& ATK_VALUE() { static std::string xATK_VALUE = "ATK_VALUE"; return xATK_VALUE; } //int
	static const std::string& DEF_VALUE() { static std::string xDEF_VALUE = "DEF_VALUE"; return xDEF_VALUE; } //int
	static const std::string& MOVE_SPEED() { static std::string xMOVE_SPEED = "MOVE_SPEED"; return xMOVE_SPEED; } //int
	static const std::string& ATK_SPEED() { static std::string xATK_SPEED = "ATK_SPEED"; return xATK_SPEED; } //int
	static const std::string& ATK_FIRE() { static std::string xATK_FIRE = "ATK_FIRE"; return xATK_FIRE; } //int
	static const std::string& ATK_LIGHT() { static std::string xATK_LIGHT = "ATK_LIGHT"; return xATK_LIGHT; } //int
	static const std::string& ATK_WIND() { static std::string xATK_WIND = "ATK_WIND"; return xATK_WIND; } //int
	static const std::string& ATK_ICE() { static std::string xATK_ICE = "ATK_ICE"; return xATK_ICE; } //int
	static const std::string& ATK_POISON() { static std::string xATK_POISON = "ATK_POISON"; return xATK_POISON; } //int
	static const std::string& DEF_FIRE() { static std::string xDEF_FIRE = "DEF_FIRE"; return xDEF_FIRE; } //int
	static const std::string& DEF_LIGHT() { static std::string xDEF_LIGHT = "DEF_LIGHT"; return xDEF_LIGHT; } //int
	static const std::string& DEF_WIND() { static std::string xDEF_WIND = "DEF_WIND"; return xDEF_WIND; } //int
	static const std::string& DEF_ICE() { static std::string xDEF_ICE = "DEF_ICE"; return xDEF_ICE; } //int
	static const std::string& DEF_POISON() { static std::string xDEF_POISON = "DEF_POISON"; return xDEF_POISON; } //int
	static const std::string& DIZZY_GATE() { static std::string xDIZZY_GATE = "DIZZY_GATE"; return xDIZZY_GATE; } //int
	static const std::string& MOVE_GATE() { static std::string xMOVE_GATE = "MOVE_GATE"; return xMOVE_GATE; } //int
	static const std::string& SKILL_GATE() { static std::string xSKILL_GATE = "SKILL_GATE"; return xSKILL_GATE; } //int
	static const std::string& PHYSICAL_GATE() { static std::string xPHYSICAL_GATE = "PHYSICAL_GATE"; return xPHYSICAL_GATE; } //int
	static const std::string& MAGIC_GATE() { static std::string xMAGIC_GATE = "MAGIC_GATE"; return xMAGIC_GATE; } //int
	static const std::string& BUFF_GATE() { static std::string xBUFF_GATE = "BUFF_GATE"; return xBUFF_GATE; } //int
	//DataTables

};

class Player
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xPlayer = "Player"; return xPlayer; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& Name() { static std::string xName = "Name"; return xName; } //string
	static const std::string& Gender() { static std::string xGender = "Gender"; return xGender; } //int
	static const std::string& Career() { static std::string xCareer = "Career"; return xCareer; } //int
	static const std::string& Camp() { static std::string xCamp = "Camp"; return xCamp; } //int
	static const std::string& LastMapID() { static std::string xLastMapID = "LastMapID"; return xLastMapID; } //int
	static const std::string& Level() { static std::string xLevel = "Level"; return xLevel; } //int
	static const std::string& CharType() { static std::string xCharType = "CharType"; return xCharType; } //int
	static const std::string& EXP() { static std::string xEXP = "EXP"; return xEXP; } //int
	static const std::string& HP() { static std::string xHP = "HP"; return xHP; } //int
	static const std::string& MP() { static std::string xMP = "MP"; return xMP; } //int
	static const std::string& Gold() { static std::string xGold = "Gold"; return xGold; } //int
	static const std::string& Money() { static std::string xMoney = "Money"; return xMoney; } //int
	static const std::string& Account() { static std::string xAccount = "Account"; return xAccount; } //string
	static const std::string& ConnectKey() { static std::string xConnectKey = "ConnectKey"; return xConnectKey; } //string
	static const std::string& OnlineCount() { static std::string xOnlineCount = "OnlineCount"; return xOnlineCount; } //int
	static const std::string& TotalOnlineTime() { static std::string xTotalOnlineTime = "TotalOnlineTime"; return xTotalOnlineTime; } //int
	static const std::string& LastOfflineTime() { static std::string xLastOfflineTime = "LastOfflineTime"; return xLastOfflineTime; } //int
	static const std::string& LoadDataFinish() { static std::string xLoadDataFinish = "LoadDataFinish"; return xLoadDataFinish; } //int
	static const std::string& GameID() { static std::string xGameID = "GameID"; return xGameID; } //int
	static const std::string& GateID() { static std::string xGateID = "GateID"; return xGateID; } //int
	//DataTables
	static const std::string& R_BagEquipList(){ static std::string xBagEquipList = "BagEquipList"; return xBagEquipList;}
	static const std::string& R_BagItemList(){ static std::string xBagItemList = "BagItemList"; return xBagItemList;}
	static const std::string& R_CommPropertyValue(){ static std::string xCommPropertyValue = "CommPropertyValue"; return xCommPropertyValue;}

	enum BagEquipList
	{
		BagEquipList_GUID		= 0, //GUID -> int64
		BagEquipList_WearGUID		= 1, //WearGUID -> int64
		BagEquipList_ConfigID		= 2, //ConfigID -> string
		BagEquipList_ExpiredType		= 3, //ExpiredType -> int
		BagEquipList_Date		= 4, //Date -> int
		BagEquipList_RandPropertyID		= 5, //RandPropertyID -> string
		BagEquipList_SlotCount		= 6, //SlotCount -> int
		BagEquipList_InlayStone1		= 7, //InlayStone1 -> string
		BagEquipList_InlayStone2		= 8, //InlayStone2 -> string
		BagEquipList_InlayStone3		= 9, //InlayStone3 -> string
		BagEquipList_InlayStone4		= 10, //InlayStone4 -> string
		BagEquipList_InlayStone5		= 11, //InlayStone5 -> string
		BagEquipList_InlayStone6		= 12, //InlayStone6 -> string
		BagEquipList_InlayStone7		= 13, //InlayStone7 -> string
		BagEquipList_InlayStone8		= 14, //InlayStone8 -> string
		BagEquipList_InlayStone9		= 15, //InlayStone9 -> string
		BagEquipList_InlayStone10		= 16, //InlayStone10 -> string
		BagEquipList_IntensifyLevel		= 17, //IntensifyLevel -> string
		BagEquipList_ElementLevel1_FIRE		= 18, //ElementLevel1_FIRE -> int
		BagEquipList_ElementLevel2_LIGHT		= 19, //ElementLevel2_LIGHT -> int
		BagEquipList_ElementLevel3_Wind		= 20, //ElementLevel3_Wind -> int
		BagEquipList_ElementLevel4_ICE		= 21, //ElementLevel4_ICE -> int
		BagEquipList_ElementLevel5_POISON		= 22, //ElementLevel5_POISON -> int

	};

	enum BagItemList
	{
		BagItemList_ConfigID		= 0, //ConfigID -> string
		BagItemList_ItemCount		= 1, //ItemCount -> int
		BagItemList_Bound		= 2, //Bound -> int
		BagItemList_ExpiredType		= 3, //ExpiredType -> int
		BagItemList_Date		= 4, //Date -> int

	};

	enum CommPropertyValue
	{
		CommPropertyValue_SUCKBLOOD		= 0, //SUCKBLOOD -> int
		CommPropertyValue_REFLECTDAMAGE		= 1, //REFLECTDAMAGE -> int
		CommPropertyValue_CRITICAL		= 2, //CRITICAL -> int
		CommPropertyValue_MAXHP		= 3, //MAXHP -> int
		CommPropertyValue_MAXMP		= 4, //MAXMP -> int
		CommPropertyValue_MAXSP		= 5, //MAXSP -> int
		CommPropertyValue_HPREGEN		= 6, //HPREGEN -> int
		CommPropertyValue_SPREGEN		= 7, //SPREGEN -> int
		CommPropertyValue_MPREGEN		= 8, //MPREGEN -> int
		CommPropertyValue_ATK_VALUE		= 9, //ATK_VALUE -> int
		CommPropertyValue_DEF_VALUE		= 10, //DEF_VALUE -> int
		CommPropertyValue_MOVE_SPEED		= 11, //MOVE_SPEED -> int
		CommPropertyValue_ATK_SPEED		= 12, //ATK_SPEED -> int
		CommPropertyValue_ATK_FIRE		= 13, //ATK_FIRE -> int
		CommPropertyValue_ATK_LIGHT		= 14, //ATK_LIGHT -> int
		CommPropertyValue_ATK_WIND		= 15, //ATK_WIND -> int
		CommPropertyValue_ATK_ICE		= 16, //ATK_ICE -> int
		CommPropertyValue_ATK_POISON		= 17, //ATK_POISON -> int
		CommPropertyValue_DEF_FIRE		= 18, //DEF_FIRE -> int
		CommPropertyValue_DEF_LIGHT		= 19, //DEF_LIGHT -> int
		CommPropertyValue_DEF_WIND		= 20, //DEF_WIND -> int
		CommPropertyValue_DEF_ICE		= 21, //DEF_ICE -> int
		CommPropertyValue_DEF_POISON		= 22, //DEF_POISON -> int
		CommPropertyValue_DIZZY_GATE		= 23, //DIZZY_GATE -> int
		CommPropertyValue_MOVE_GATE		= 24, //MOVE_GATE -> int
		CommPropertyValue_SKILL_GATE		= 25, //SKILL_GATE -> int
		CommPropertyValue_PHYSICAL_GATE		= 26, //PHYSICAL_GATE -> int
		CommPropertyValue_MAGIC_GATE		= 27, //MAGIC_GATE -> int
		CommPropertyValue_BUFF_GATE		= 28, //BUFF_GATE -> int

	};

};

class Scene
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xScene = "Scene"; return xScene; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& SceneName() { static std::string xSceneName = "SceneName"; return xSceneName; } //string
	static const std::string& SceneShowName() { static std::string xSceneShowName = "SceneShowName"; return xSceneShowName; } //string
	static const std::string& MaxGroup() { static std::string xMaxGroup = "MaxGroup"; return xMaxGroup; } //int
	static const std::string& MaxGroupPlayers() { static std::string xMaxGroupPlayers = "MaxGroupPlayers"; return xMaxGroupPlayers; } //int
	static const std::string& FilePath() { static std::string xFilePath = "FilePath"; return xFilePath; } //string
	static const std::string& RelivePos() { static std::string xRelivePos = "RelivePos"; return xRelivePos; } //string
	static const std::string& Width() { static std::string xWidth = "Width"; return xWidth; } //int
	static const std::string& SoundList() { static std::string xSoundList = "SoundList"; return xSoundList; } //string
	static const std::string& Share() { static std::string xShare = "Share"; return xShare; } //int
	static const std::string& CanClone() { static std::string xCanClone = "CanClone"; return xCanClone; } //int
	static const std::string& ActorID() { static std::string xActorID = "ActorID"; return xActorID; } //int
	static const std::string& LoadingUI() { static std::string xLoadingUI = "LoadingUI"; return xLoadingUI; } //string
	static const std::string& CamOffestPos() { static std::string xCamOffestPos = "CamOffestPos"; return xCamOffestPos; } //string
	static const std::string& CamOffestRot() { static std::string xCamOffestRot = "CamOffestRot"; return xCamOffestRot; } //string
	static const std::string& SyncObject() { static std::string xSyncObject = "SyncObject"; return xSyncObject; } //int
	//DataTables

};

class Shop
{
public:
	//Class name
	static const std::string& ThisName(){ static std::string xShop = "Shop"; return xShop; }
	//IObject
	static const std::string& Id(){ static std::string xId = "Id"; return xId; } //string
	static const std::string& ClassName(){ static std::string xClassName = "ClassName"; return xClassName; } //string
	static const std::string& MapID(){ static std::string xMapID = "MapID"; return xMapID; } //int
	static const std::string& InstanceID(){ static std::string xInstanceID = "InstanceID"; return xInstanceID; } //int
	static const std::string& ConfigID(){ static std::string xConfigID = "ConfigID"; return xConfigID; } //string
	//DataNodes
	static const std::string& Type() { static std::string xType = "Type"; return xType; } //int
	static const std::string& ItemID() { static std::string xItemID = "ItemID"; return xItemID; } //string
	static const std::string& Gold() { static std::string xGold = "Gold"; return xGold; } //int
	static const std::string& Steel() { static std::string xSteel = "Steel"; return xSteel; } //int
	static const std::string& Stone() { static std::string xStone = "Stone"; return xStone; } //int
	static const std::string& Diamond() { static std::string xDiamond = "Diamond"; return xDiamond; } //int
	static const std::string& Level() { static std::string xLevel = "Level"; return xLevel; } //int
	//DataTables

};


}
