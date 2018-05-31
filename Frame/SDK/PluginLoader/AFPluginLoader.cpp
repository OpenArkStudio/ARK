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

#if ARK_PLATFORM == PLATFORM_UNIX
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/prctl.h>
#endif

bool bExitApp = false;
std::thread gBackThread;

#if ARK_PLATFORM == PLATFORM_WIN
#include <Dbghelp.h>
#pragma comment(lib, "DbgHelp")
#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
#pragma comment(lib, "AFCore_d.lib")
#else
#pragma comment(lib, "AFCore.lib")
#endif

// 閸掓稑缂揇ump閺傚洣娆?
void CreateDumpFile(const std::string& strDumpFilePathName, EXCEPTION_POINTERS* pException)
{
    // 閸掓稑缂揇ump閺傚洣娆?
    HANDLE hDumpFile = CreateFile(strDumpFilePathName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Dump娣団剝浼?
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    // 閸愭瑥鍙咲ump閺傚洣娆㈤崘鍛啇
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
}

// 婢跺嫮鎮奤nhandled Exception閻ㄥ嫬娲栫拫鍐ㄥ毐閺?
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
#else
    //Do nothing
#endif
}

void InitDaemon()
{
#if ARK_PLATFORM == PLATFORM_UNIX
    daemon(1, 0);

    // ignore signals
    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
#endif
}

void EchoArkLogo()
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif

    std::cout << "     _         _        ____  _             _ _       " << std::endl;
    std::cout << "    / \\   _ __| | __   / ___|| |_ _   _  __| (_) ___  " << std::endl;
    std::cout << "   / _ \\ | '__| |/ /   \\___ \\| __| | | |/ _` | |/ _ \\ " << std::endl;
    std::cout << "  / ___ \\| |  |   <     ___) | |_| |_| | (_| | | (_) |" << std::endl;
    std::cout << " /_/   \\_\\_|  |_|\\_\\   |____/ \\__|\\__,_|\\__,_|_|\\___/ " << std::endl;
    std::cout << std::endl;
    std::cout << "COPYRIGHT (c) 2013-2018 Ark Studio" << std::endl;
    std::cout << "All RIGHTS RESERVED." << std::endl;
    std::cout << "HTTPS://ARKGAME.NET" << std::endl;
    std::cout << "********************************************" << std::endl;

#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

void Usage()
{
    std::cout << "Ark PluginLoader usage:" << std::endl;
    std::cout << "./PluginLoader [options]" << std::endl;
    std::cout << "Option:" << std::endl;
    std::cout << "\t" << "-d, Run as daemon, just take effect in Linux" << std::endl;
    std::cout << "\t" << "-x, Remove close button, just take effect in Windows" << std::endl;
    std::cout << "\t" << "cfg=plugin.xml, plugin configuration files" << std::endl;
    std::cout << "\t" << "app_id=1, set application's id" << std::endl;
    std::cout << "\t" << "app_name=GameServer, set application's name" << std::endl;
    std::cout << "i.e. ./PluginLoader -d -x cfg=plugin.xml app_id=1 app_name=my_test" << std::endl;
}

void ThreadFunc()
{
    while(!bExitApp)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::string s;
        std::cin >> s;
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        if(s == "exit")
        {
            bExitApp = true;
        }
        else if(s == "help")
        {
            Usage();
        }
    }
}

void CreateBackThread()
{
    gBackThread = std::thread(std::bind(&ThreadFunc));
    //std::cout << "CreateBackThread, thread ID = " << gThread.get_id() << std::endl;
}

struct ApplicationConfig
{
    bool deamon = true;                     //run as deamon, Linux
    bool xbutton = true;                    //close X button in windows
    std::string plugin_file = "Plugin.xml"; //config file
    int app_id = 0;                 //app id
    std::string app_name = "";              //app name
};

#if ARK_PLATFORM == PLATFORM_UNIX
extern char** environ;

struct environ_guard
{
public:
    ~environ_guard()
    {
        if(env_buf)
        {
            delete[] env_buf;
            env_buf = nullptr;
        }
        if(environ)
        {
            delete[] environ;
            environ = nullptr;
        }
    }

    char* env_buf;
    char** environ;
};

environ_guard g_new_environ_guard{ nullptr, nullptr };

int realloc_environ()
{
    int var_count = 0;
    int env_size = 0;
    {
        char** ep = environ;
        while(*ep)
        {
            env_size += std::strlen(*ep) + 1;
            ++var_count;
            ++ep;
        }
    }

    char* new_env_buf = new char[env_size];
    std::memcpy((void *)new_env_buf, (void *)*environ, env_size);

    char** new_env = new char*[var_count + 1];
    {
        int var = 0;
        int offset = 0;
        char** ep = environ;
        while(*ep)
        {
            new_env[var++] = (new_env_buf + offset);
            offset += std::strlen(*ep) + 1;
            ++ep;
        }
    }
    new_env[var_count] = 0;

    // RAII to prevent memory leak
    g_new_environ_guard = environ_guard{ new_env_buf, new_env };

    environ = new_env;

    return env_size;
}

void setproctitle(const char* title, int argc, char** argv)
{
    int argv_size = 0;
    for(int i = 0; i < argc; ++i)
    {
        int len = std::strlen(argv[i]);
        std::memset(argv[i], 0, len);
        argv_size += len;
    }

    int to_be_copied = std::strlen(title);
    if(argv_size <= to_be_copied)
    {
        int env_size = realloc_environ();
        if(env_size < to_be_copied)
        {
            to_be_copied = env_size;
        }
    }

    std::strncpy(argv[0], title, to_be_copied);
    *(argv[0] + to_be_copied) = 0;
}

#endif

bool ProcArgList(int argc, char* argv[])
{
    //Echo logo
    EchoArkLogo();

    //Analyse arg list
    ApplicationConfig config;
    for(int i = 0; i < argc; ++i)
    {
        std::string arg = argv[i];
        if(arg == "-d")
        {
            config.deamon = true;
        }
        else if(arg == "-x")
        {
            config.xbutton = true;
        }
        else if(arg.find("cfg") != std::string::npos)
        {
            size_t pos = arg.find("=");
            if(pos != std::string::npos)
            {
                config.plugin_file = arg.substr(pos + 1, arg.length() - pos - 1);
            }
        }
        else if(arg.find("app_id") != std::string::npos)
        {
            size_t pos = arg.find("=");
            if(pos != std::string::npos)
            {
                config.app_id = ARK_LEXICAL_CAST<int>(arg.substr(pos + 1, arg.length() - pos - 1));
            }
        }
        else if(arg.find("app_name") != std::string::npos)
        {
            size_t pos = arg.find("=");
            if(pos != std::string::npos)
            {
                config.app_name = arg.substr(pos + 1, arg.length() - pos - 1);
            }
        }
    }

    if(config.deamon)
    {
#if ARK_PLATFORM == PLATFORM_UNIX
        //Run as a daemon process
        signal(SIGPIPE, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);
#endif
    }

    if(config.xbutton)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
        CloseXButton();
#endif
    }

    if(config.app_id == 0)
    {
        std::cout << "parameter app_id is invalid, please check." << std::endl;
        return false;
    }

    if(config.app_name.empty())
    {
        std::cout << "parameter app_name is invalid, please check." << std::endl;
        return false;
    }

    //Set plugin file
    AFCPluginManager::GetInstancePtr()->SetConfigName(config.plugin_file);
    AFCPluginManager::GetInstancePtr()->SetAppID(config.app_id);
    AFCPluginManager::GetInstancePtr()->SetAppName(config.app_name);

    std::string process_name = "[" + config.app_name + ":" + ARK_TO_STRING(config.app_id) + "]";
    //Set process name
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTitle(process_name.c_str());
#elif ARK_PLATFORM == PLATFORM_UNIX
    setproctitle(process_name.c_str(), argc, argv);
#endif

    //Create back thread, for some cmd
    CreateBackThread();

    return true;
}

void MainLoop()
{
#if ARK_PLATFORM == PLATFORM_WIN
    __try
    {
        AFCPluginManager::GetInstancePtr()->Update();
    }
    __except(ApplicationCrashHandler(GetExceptionInformation()))
    {
    }
#else
    AFCPluginManager::GetInstancePtr()->Update();
#endif
}

int main(int argc, char* argv[])
{
    //arg list
    //-d, Run as daemon, just take effect in Linux
    //-x, Remove close button, just take effect in Windows
    //cfg=plugin.xml, plugin configuration files
    //app_id=1, set application's id
    //app_name=GameServer, set application's name
    if(!ProcArgList(argc, argv))
    {
        std::cout << "Application parameter is invalid, please check it..." << std::endl;
        Usage();

        std::this_thread::sleep_for(std::chrono::seconds(30));
        return -1;
    }

    AFCPluginManager::GetInstancePtr()->Init();
    AFCPluginManager::GetInstancePtr()->PostInit();
    AFCPluginManager::GetInstancePtr()->CheckConfig();
    AFCPluginManager::GetInstancePtr()->PreUpdate();

    while(!bExitApp)
    {
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            if(bExitApp)
            {
                break;
            }

            MainLoop();
        }
    }

    AFCPluginManager::GetInstancePtr()->PreShut();
    AFCPluginManager::GetInstancePtr()->Shut();

    gBackThread.join();

    return 0;
}
