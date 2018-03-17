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
#include "SDK/Core/Base/AFMemAlloc.h"
//#include "SDK/Core/Base/AFTimer.h"
//
//AFTimerManager::TimerManagerPtr gTimerManager;
//AFTimer::TimerWeakPtr timerPtr;

bool HelloWorld1::Init()
{
	std::cout << typeid(HelloWorld1).name() << std::endl;
    std::cout << "Hello, world1, Init" << std::endl;

    AFMemAlloc::InitPool();
    AFMemAlloc::Start();
    //gTimerManager = std::shared_ptr<AFTimerManager>();

    return true;
}

bool HelloWorld1::AfterInit()
{
    std::cout << "Hello, world1, AfterInit" << std::endl;
    AFCData data1(DT_STRING, "test1");
    AFCData data2(DT_STRING, "test2");

    data1 = data2;
    const char* str1 = data1.GetString();
    //////////////////////////////////////////////////////////////////////////
    ////test memory alloc
    //void* ptr1 = ARK_ALLOC(100);
    //memset(ptr1, 0, 100);

    //void* ptr2 = ARK_ALLOC(10);

    //AFMemAlloc::CheckLeak();

    //ARK_FREE(ptr2);

    //AFMemAlloc::CheckLeak();

    //ARK_FREE(ptr1);
    //AFMemAlloc::CheckLeak();
    //////////////////////////////////////////////////////////////////////////

    //std::chrono::nanoseconds interval_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()) + std::chrono::nanoseconds{ 100000000 };
    //timerPtr = gTimerManager->AddTimer(interval_time, [=]()
    //{
    //    std::cout << "Run timer" << std::endl;
    //});

    return true;
}

void HelloWorld1::Update()
{

}

bool HelloWorld1::BeforeShut()
{
    std::cout << "Hello, world1, BeforeShut-------------" << std::endl;
    //timerPtr->Cancel();
    return true;
}

bool HelloWorld1::Shut()
{
    std::cout << "Hello, world1, Shut" << std::endl;
    return true;
}