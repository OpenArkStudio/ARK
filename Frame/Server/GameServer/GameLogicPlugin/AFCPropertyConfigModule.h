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

#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_iterators.hpp"
#include "RapidXML/rapidxml_print.hpp"
#include "RapidXML/rapidxml_utils.hpp"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Proto/ARKDataDefine.hpp"
#include "Server/Interface/AFIPropertyConfigModule.h"

class AFCPropertyConfigModule
    : public AFIPropertyConfigModule
{
public:
    explicit AFCPropertyConfigModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCPropertyConfigModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Update();
    virtual bool PostInit();

    virtual int CalculateBaseValue(const int nJob, const int nLevel, const std::string& strProperty);
    virtual bool LegalLevel(const int nJob, const int nLevel);

protected:
    void Load();

private:
    //
    //diffent job, diffrent PropertyID[Level->EffectData]
    AFMapEx<int, AFMapEx<int, std::string> > mhtCoefficienData;

    AFIClassModule* m_pClassModule;
    AFIElementModule* m_pElementModule;
};