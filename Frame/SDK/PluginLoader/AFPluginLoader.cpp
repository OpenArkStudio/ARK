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
// * @file  	AFPluginLoader.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFPluginLoader                                                  *
*****************************************************************************/
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include "AFCPluginManager.h"
#include "SDK/Base/AFPlatform.hpp"

#if ARK_PLATFORM == PLATFORM_UNIX
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#endif

bool bExitApp = false;
std::thread gThread;
std::string strArgvList;
std::string strPluginName;

#if ARK_PLATFORM == PLATFORM_WIN

#include <Dbghelp.h>
#pragma comment( lib, "DbgHelp" )
// 创建Dump文件
void CreateDumpFile(const std::string& strDumpFilePathName, EXCEPTION_POINTERS* pException)
{
    // 创建Dump文件
    HANDLE hDumpFile = CreateFile(strDumpFilePathName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Dump信息
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    // 写入Dump文件内容
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
}

// 处理Unhandled Exception的回调函数
long ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
    time_t t = time(0);
    char szDmupName[MAX_PATH];
    tm* ptm = localtime(&t);

    sprintf_s(szDmupName, "%04d_%02d_%02d_%02d_%02d_%02d.dmp",  ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    CreateDumpFile(szDmupName, pException);

    FatalAppExit(-1,  szDmupName);

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void CloseXButton()
{
#if ARK_PLATFORM == PLATFORM_WIN
    HWND hWnd = GetConsoleWindow();
    if(hWnd)
    {
        HMENU hMenu = GetSystemMenu(hWnd, FALSE);
        EnableMenuItem(hMenu, SC_CLOSE, MF_DISABLED | MF_BYCOMMAND);
    }
#endif
}

void ThreadFunc()
{
    while(!bExitApp)
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        //         std::string s;
        //         std::cin >> s;
        //         if ( 0 == stricmp( s.c_str(), "exit" ) )
        //         {
        //             bExitApp = true;
        //         }
    }
}

void CreateBackThread()
{
    //gThread = std::thread(std::bind(&ThreadFunc));
    //auto f = std::async (std::launch::async, std::bind(ThreadFunc));
    //std::cout << "CreateBackThread, thread ID = " << gThread.get_id() << std::endl;
}

void InitDaemon()
{
#if ARK_PLATFORM == PLATFORM_UNIX
    daemon(1, 0);

    // ignore signals
    signal(SIGINT,  SIG_IGN);
    signal(SIGHUP,  SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
#endif
}

void PrintfLogo()
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif

    std::cout << "\n" << std::endl;
    std::cout << "★★★★★★★★★★★★★★★★★★★★★★★★" << std::endl;
    std::cout << "★                                            ★" << std::endl;
    std::cout << "★                 ArkGame                    ★" << std::endl;
    std::cout << "★   Copyright (c) 2011-2017  ArkGame Studio  ★" << std::endl;
    std::cout << "★             All rights reserved.           ★" << std::endl;
    std::cout << "★                                            ★" << std::endl;
    std::cout << "★★★★★★★★★★★★★★★★★★★★★★★★" << std::endl;
    std::cout << "\n" << std::endl;
    std::cout << "-d Run it as daemon mode, only on linux" << std::endl;
    std::cout << "-x Closethe 'X' button, only on windows" << std::endl;
    std::cout << "name.xml File's name to instead of \"Plugin.xml\" when programs be launched, all platform" << std::endl;
    std::cout << "\n" << std::endl;

#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

int main(int argc, char* argv[])
{
    for(int i = 0; i < argc; i++)
    {
        strArgvList += " ";
        strArgvList += argv[i];
    }

#if ARK_PLATFORM == PLATFORM_WIN
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
    if(strArgvList.find("-x") != string::npos)
    {
        CloseXButton();
    }
#elif ARK_PLATFORM == PLATFORM_UNIX
    //run it as a daemon process
    if(strArgvList.find("-d") != string::npos)
    {
        InitDaemon();
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
#endif

    if(strArgvList.find(".xml") != string::npos)
    {
        for(int i = 0; i < argc; i++)
        {
            strPluginName = argv[i];
            if(strPluginName.find(".xml") != string::npos)
            {
                break;
            }
        }

        AFCPluginManager::GetInstancePtr()->SetConfigName(strPluginName);
    }


    AFCPluginManager::GetInstancePtr()->Init();
    AFCPluginManager::GetInstancePtr()->AfterInit();
    AFCPluginManager::GetInstancePtr()->CheckConfig();

    PrintfLogo();
    CreateBackThread();

    while(!bExitApp)     //DEBUG版本崩溃，RELEASE不崩
    {
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            if(bExitApp)
            {
                break;
            }

#if ARK_PLATFORM == PLATFORM_WIN
            __try
            {
#endif
                AFCPluginManager::GetInstancePtr()->Execute();
#if ARK_PLATFORM == PLATFORM_WIN
            }
            __except(ApplicationCrashHandler(GetExceptionInformation()))
            {
            }
#endif
        }
    }

    AFCPluginManager::GetInstancePtr()->BeforeShut();
    AFCPluginManager::GetInstancePtr()->Shut();

    AFCPluginManager::GetInstancePtr()->ReleaseInstance();

    return 0;
}

