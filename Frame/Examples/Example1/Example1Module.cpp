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

#include "Example1Module.h"
#include "SDK/Core/Base/AFTimer.hpp"
#include "SDK/Core/Base/AFMacros.hpp"

bool Example1Module::Init()
{
    std::cout << typeid(Example1Module).name() << ", Init" << std::endl;
    return true;
}

bool Example1Module::PostInit()
{
    m_pTimerModule = pPluginManager->FindModule<AFITimerModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();

    ARK_ASSERT_RET_VAL(m_pTimerModule != nullptr, false);
    ARK_ASSERT_RET_VAL(m_pLogModule != nullptr, false);

    AFGUID test_id = AFGUID(0, 1);
    ARK_LOG_INFO("Test log, id = %s", test_id.ToString().c_str());

    std::cout << typeid(Example1Module).name() << ", PostInit" << std::endl;

    AFCData data1(DT_STRING, "test1");
    AFCData data2(DT_STRING, "test2");

    data1 = data2;
    const char* str1 = data1.GetString();

    std::cout << pPluginManager->GetNowTime() << std::endl;

    m_pTimerModule->AddSingleTimer("test", test_id, 10 * 1000/*ms*/, 2, this, &Example1Module::TestTimer);

    return true;
}

void Example1Module::Update()
{

}

bool Example1Module::PreShut()
{
    std::cout << typeid(Example1Module).name() << ", PreShut" << std::endl;
    return true;
}

bool Example1Module::Shut()
{
    std::cout << typeid(Example1Module).name() << ", Shut" << std::endl; 
    return true;
}

void Example1Module::TestTimer(const std::string& name, const AFGUID& entity_id)
{
    std::cout << pPluginManager->GetNowTime() << std::endl;
    std::cout << "Test Timer: " << name << " id = " << entity_id.ToString() << std::endl;
}