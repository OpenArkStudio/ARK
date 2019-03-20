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
	static const st