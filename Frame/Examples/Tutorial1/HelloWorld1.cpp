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
#include "SDK/Core/Base/timer.hpp"
#include "SDK/Core/Base/AFMemAlloc.h"

using timer_t = timer<std::function<void(timerid_t)> >;
timer_t* t = new timer_t();
timer_t* t2 = new timer_t();
int32_t time_id_1 = 0;
int32_t time_id_2 = 0;

bool HelloWorld1::Init()
{
	std::cout << typeid(HelloWorld1).name() << std::endl;
    std::cout << "Hello, world1, Init" << std::endl;

    AFMemAlloc::InitPool();
    AFMemAlloc::Start();

    return true;
}

bool HelloWorld1::AfterInit()
{
    std::cout << "Hello, world1, AfterInit" << std::endl;

    //// test timer
    //time_id_1 = t->repeat(1000, 10, [](timerid_t id) { std::cout << "T1 ID = " << id << ", interval=1000ms print timer update" << std::endl; });
    //time_id_2 = t2->repeat(1500, 10, [](timerid_t id) { std::cout << "T2 ID = " << id << ", interval=1500ms print timer update" << std::endl; });

    //////////////////////////////////////////////////////////////////////////
    //test memory alloc
    void* ptr1 = ARK_ALLOC(100);
    memset(ptr1, 0, 100);

    void* ptr2 = ARK_ALLOC(10);

    AFMemAlloc::CheckLeak();

    ARK_FREE(ptr2);

    AFMemAlloc::CheckLeak();

    ARK_FREE(ptr1);
    AFMemAlloc::CheckLeak();
    //////////////////////////////////////////////////////////////////////////

    return true;
}

void HelloWorld1::Update()
{
    //t->update();
    //t2->update();
}

bool HelloWorld1::BeforeShut()
{
    std::cout << "Hello, world1, BeforeShut-------------" << std::endl;
    return true;
}

bool HelloWorld1::Shut()
{
    std::cout << "Hello, world1, Shut" << std::endl;
    return true;
}