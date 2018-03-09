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

#include "AFCPluginManager.h"
#include "SDK/Core/Base/AFPlatform.hpp"
#include <future>

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
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::string s;
        std::cin >> s;
        if (0 == strcmp(s.c_str(), "exit"))
        {
            bExitApp = true;
        }
    }
}

void CreateBackThread()
{
    gThread = std::thread(std::bind(&ThreadFunc));
    std::cout << "CreateBackThread, thread ID = " << gThread.get_id() << std::endl;
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

void PrintLogo()
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif

    std::cout << "********************************************" << std::endl;
    std::cout << "ARK" << std::endl;
    std::cout << "COPYRIGHT © 2013-2018 ARK-GAME" << std::endl;
    std::cout << "All RIGHTS RESERVED." << std::endl;
    std::cout << "HTTPS://ARKGAME.NET" << std::endl;
    std::cout << "********************************************" << std::endl;

#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

int main(int argc, char* argv[])
{
    PrintLogo();

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

    //back thread, for some cmd
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
                AFCPluginManager::GetInstancePtr()->Update();
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

    gThread.join();

    return 0;
}

