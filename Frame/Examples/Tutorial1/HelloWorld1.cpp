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

#include "HelloWorld1.h"
#include "SDK/Core/Base/AFTimer.hpp"
#include "SDK/Core/Base/AFMacros.hpp"

bool HelloWorld1::Init()
{
    std::cout << typeid(HelloWorld1).name() << std::endl;
    std::cout << "Hello, world1, Init" << std::endl;

    return true;
}

bool HelloWorld1::PostInit()
{
    m_pTimerModule = pPluginManager->FindModule<AFITimerModule>();

    ARK_ASSERT_RET_VAL(m_pTimerModule != nullptr, false);

    std::cout << "Hello, world1, PostInit" << std::endl;
    AFCData data1(DT_STRING, "test1");
    AFCData data2(DT_STRING, "test2");

    data1 = data2;
    const char* str1 = data1.GetString();

    std::cout << pPluginManager->GetNowTime() << std::endl;

    m_pTimerModule->AddSingleTimer("test", AFGUID(0, 1), 10 * 1000/*ms*/, 2, this, &HelloWorld1::TestTimer);

    return true;
}

void HelloWorld1::Update()
{

}

bool HelloWorld1::PreShut()
{
    std::cout << "Hello, world1, PreShut-------------" << std::endl;
    return true;
}

bool HelloWorld1::Shut()
{
    std::cout << "Hello, world1, Shut" << std::endl;
    return true;
}

void HelloWorld1::TestTimer(const std::string& name, const AFGUID& entity_id)
{
    std::cout << pPluginManager->GetNowTime() << std::endl;
    std::cout << "Test Timer: " << name << " id = " << entity_id.ToString() << std::endl;
}