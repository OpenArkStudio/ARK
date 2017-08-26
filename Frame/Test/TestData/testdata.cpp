/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file  	testdata.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     testdata                                                  *
*****************************************************************************/
#include "SDK/Base/AFCDataList.h"
#include "SDK/Base/AFTime.hpp"

using namespace ArkFrame;

std::string gen_random(const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string data;
    for(int i = 0; i < len; ++i)
    {
        data += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return data;
}

std::vector<std::string> RandStrVector;

void gen_rand_str_vetor()
{
    for(int i = 0; i < 100000; ++i)
    {
        RandStrVector.push_back(gen_random(10));
    }
}

//////////////////////////////////////////////////////////////////////////

void test_old_datalist()
{
    AFCDataList xList;

    for(int i = 0; i < 100000; ++i)
    {
        xList << i;
    }

    for(int i = 0; i < 100000; ++i)
    {
        xList << (float)i;
    }

    for(int i = 0; i < 100000; ++i)
    {
        xList << (double)i;
    }

    for(int i = 0; i < RandStrVector.size(); ++i)
    {
        xList << RandStrVector[i];
    }

    std::cout << "test " << xList.GetCount() << " old DataList finished." << std::endl;
}

void test_new_datalist()
{
    AFCDataList xList;

    for(int i = 0; i < 100000; ++i)
    {
        xList << i;
    }

    for(int i = 0; i < 100000; ++i)
    {
        xList << (float)i;
    }

    for(int i = 0; i < 100000; ++i)
    {
        xList << (double)i;
    }

    for(int i = 0; i < RandStrVector.size(); ++i)
    {
        xList << RandStrVector[i];
    }

    std::cout << "test " << xList.GetCount() << " new DataList finished." << std::endl;
}

int64_t GetNowMS()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void main()
{
#ifdef _DEBUG
    std::cout << "start DEBUG test....." << std::endl;
#else
    std::cout << "start RELEASE test....." << std::endl;
#endif

    //Éú³ÉËæ»ú×Ö·û´®
    gen_rand_str_vetor();

    int64_t now = GetNowMS();
    test_old_datalist();
    int64_t delta_old = GetNowMS() - now;

    std::cout << "old_data_list: time = " << delta_old << " ms = " << delta_old / 1000.f << " s" << std::endl;

    now = GetNowMS();
    test_new_datalist();
    int64_t delta_new = GetNowMS() - now;

    std::cout << "new_data_list: time = " << delta_new << " ms = " << delta_new / 1000.f << " s" << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "performance =  " << delta_old / delta_new << std::endl;

    system("pause");
}
