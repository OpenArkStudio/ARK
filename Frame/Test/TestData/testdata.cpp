#include "NFComm/NFCore/AFDataList.hpp"
#include "NFComm/NFCore/AFCDataList.h"
#include "NFComm/NFCore/NFTime.h"
#include <chrono>

std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string data;
    for (int i = 0; i < len; ++i) {
        data += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return data;
}

std::vector<std::string> RandStrVector;

void gen_rand_str_vetor()
{
    for (int i = 0; i < 100000; ++i)
    {
        RandStrVector.push_back(gen_random(10));
    }
}

//////////////////////////////////////////////////////////////////////////

void test_old_datalist()
{
    AFDataList xList;

    for (int i = 0; i < 100000; ++i)
    {
        xList << i;
    }

    for (int i = 0; i < 100000; ++i)
    {
        xList << (float)i;
    }

    for (int i = 0; i < 100000; ++i)
    {
        xList << (double)i;
    }

    for (int i = 0; i < RandStrVector.size(); ++i)
    {
        xList << RandStrVector[i];
    }

    std::cout << "test " << xList.GetCount() << " old DataList finished." << std::endl;
}

void test_new_datalist()
{
    ArkFrame::AFXDataList xList;

    for (int i = 0; i < 100000; ++i)
    {
        xList << i;
    }

    for (int i = 0; i < 100000; ++i)
    {
        xList << (float)i;
    }

    for (int i = 0; i < 100000; ++i)
    {
        xList << (double)i;
    }

    for (int i = 0; i < RandStrVector.size(); ++i)
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