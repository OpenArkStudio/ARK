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

#include <args/args.hxx>
#include "Common/AFBaseStruct.hpp"
#include "SDK/Core/AFMacros.hpp"
#include "SDK/Core/AFDateTime.hpp"
#include "AFCPluginManager.h"

using namespace ark;

bool g_exit_loop = false;
std::thread g_cmd_thread;

#if ARK_PLATFORM == PLATFORM_WIN
#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
#pragma comment(lib, "AFCore_d.lib")
#else
#pragma comment(lib, "AFCore.lib")
#endif

//mini-dump
void CreateDumpFile(const std::string& strDumpFilePathName, EXCEPTION_POINTERS* pException)
{
    HANDLE hDumpFile = CreateFile(strDumpFilePathName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
}

long ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
    AFDateTime now;
    tm* ptm = now.GetLocalTime();
    std::string dump_name = ARK_FORMAT("{}-{:04d}{:02d}{:02d}_{:02d}_{:02d}_{:02d}.dmp", AFCPluginManager::GetInstancePtr()->AppName(), ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    CreateDumpFile(dump_name.c_str(), pException);

    FatalAppExit(-1, dump_name.c_str());
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void CloseXButton()
{
#if ARK_PLATFORM == PLATFORM_WIN
    HWND hWnd = GetConsoleWindow();

    if (hWnd)
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
    int ret = daemon(1, 1);
    ARK_ASSERT_NO_EFFECT(ret == 0);

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

void PrintLogo()
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif

    CONSOLE_LOG_NO_FILE << "     _         _          ____                      " << std::endl;
    CONSOLE_LOG_NO_FILE << "    / \\   _ __| | __     / ___| __ _ _ __ ___   ___ " << std::endl;
    CONSOLE_LOG_NO_FILE << "   / _ \\ | '__| |/ /    | |  _ / _` | '_ ` _ \\ / _ \\" << std::endl;
    CONSOLE_LOG_NO_FILE << "  / ___ \\| |  |   <     | |_| | (_| | | | | | |  __/" << std::endl;
    CONSOLE_LOG_NO_FILE << " /_/   \\_\\_|  |_|\\_\\     \\____|\\__,_|_| |_| |_|\\___|" << std::endl;
    CONSOLE_LOG_NO_FILE << "                                                    " << std::endl;
    CONSOLE_LOG_NO_FILE << std::endl;
    CONSOLE_LOG_NO_FILE << "Copyright 2018 (c) Ark Studio. All Rights Reserved." << std::endl;
    CONSOLE_LOG_NO_FILE << "Website: https://arkgame.net" << std::endl;
    CONSOLE_LOG_NO_FILE << "Github:  https://github.com/ArkGame" << std::endl;
    CONSOLE_LOG_NO_FILE << "***********************************************************" << std::endl;
    CONSOLE_LOG_NO_FILE << std::endl;

#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

void ThreadFunc()
{
    while (!g_exit_loop)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::string s;
        std::cin >> s;
        s = AFMisc::ToLower(s);
        if (s == "exit")
        {
            g_exit_loop = true;
        }
    }
}

void CreateBackThread()
{
    g_cmd_thread = std::thread(std::bind(&ThreadFunc));
}

#if ARK_PLATFORM == PLATFORM_UNIX
extern char** environ;

class environ_guard
{
public:
    ~environ_guard()
    {
        if (env_buf)
        {
            delete[] env_buf;
            env_buf = nullptr;
        }

        if (environ)
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

    do
    {
        char** ep = environ;

        while (*ep)
        {
            env_size += std::strlen(*ep) + 1;
            ++var_count;
            ++ep;
        }
    } while (0);

    char* new_env_buf = new char[env_size];
    std::memcpy((void*)new_env_buf, (void*)*environ, env_size);

    char** new_env = new char* [var_count + 1];

    do
    {
        int var = 0;
        int offset = 0;
        char** ep = environ;

        while (*ep)
        {
            new_env[var++] = (new_env_buf + offset);
            offset += std::strlen(*ep) + 1;
            ++ep;
        }
    } while (0);

    new_env[var_count] = 0;

    // RAII to prevent memory leak
    g_new_environ_guard = environ_guard{ new_env_buf, new_env };

    environ = new_env;

    return env_size;
}

void setproctitle(const char* title, int argc, char** argv)
{
    int argv_size = 0;

    for (int i = 0; i < argc; ++i)
    {
        int len = std::strlen(argv[i]);
        std::memset(argv[i], 0, len);
        argv_size += len;
    }

    int to_be_copied = std::strlen(title);

    if (argv_size <= to_be_copied)
    {
        int env_size = realloc_environ();

        if (env_size < to_be_copied)
        {
            to_be_copied = env_size;
        }
    }

    std::strncpy(argv[0], title, to_be_copied);
    *(argv[0] + to_be_copied) = 0;
}

#endif

bool ParseArgs(int argc, char* argv[])
{
    args::ArgumentParser parser("Here is ark plugin loader argument tools", "If you have any questions, please report an issue in GitHub.");
    args::HelpFlag help(parser, "help", "Display the help menu", {'h', "help"});
    args::ActionFlag xbutton(parser, "close", "Close [x] button in Windows", { 'x' }, [&]()
    {
#if ARK_PLATFORM == PLATFORM_WIN
        SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
        CloseXButton();
#endif
    });

    args::ActionFlag daemon(parser, "daemon", "Run application as daemon", { 'd' }, [&]()
    {
#if ARK_PLATFORM == PLATFORM_UNIX
        InitDaemon();
#endif
    });

    args::ValueFlag<std::string> busid(parser, "busid", "Set application id(like IP address: 8.8.8.8)", { 'b', "busid" }, "8.8.8.8", args::Options::Required | args::Options::Single);
    args::ValueFlag<std::string> name(parser, "name", "Set application name", { 'n', "name" }, "my-server", args::Options::Required | args::Options::Single);
    args::ValueFlag<std::string> plugin_cfg(parser, "plugin config path", "Set application plugin config", { 'p', "plugin" }, "plugin.xml", args::Options::Required | args::Options::Single);
    std::string default_log_path = ARK_FORMAT("..{}binlog", ark_folder_sep);
    args::ValueFlag<std::string> logpath(parser, "logpath", "Set application log output path", { 'l', "logpath" }, default_log_path, args::Options::Required | args::Options::Single);

    //start parse argument list
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cerr << parser;
        return false;
    }
    catch (args::ParseError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return false;
    }
    catch (args::ValidationError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return false;
    }

    //Set bus id
    if (busid)
    {
        AFCDataList temp_bus_id;
        if (!temp_bus_id.Split(busid.Get(), "."))
        {
            CONSOLE_LOG << "bus id is invalid, it likes 8.8.8.8" << std::endl;
            return false;
        }

        AFBusAddr busaddr;
        busaddr.channel_id = ARK_LEXICAL_CAST<int>(temp_bus_id.String(0));
        busaddr.zone_id = ARK_LEXICAL_CAST<int>(temp_bus_id.String(1));
        busaddr.proc_id = ARK_LEXICAL_CAST<int>(temp_bus_id.String(2));
        busaddr.inst_id = ARK_LEXICAL_CAST<int>(temp_bus_id.String(3));

        AFCPluginManager::GetInstancePtr()->SetBusID(busaddr.bus_id);
    }
    else
    {
        return false;
    }

    //Set app name
    if (name)
    {
        AFCPluginManager::GetInstancePtr()->SetAppName(name.Get());

        std::string process_name = name.Get() + "-" + busid.Get();
        //Set process name
#if ARK_PLATFORM == PLATFORM_WIN
        SetConsoleTitle(process_name.c_str());
#elif ARK_PLATFORM == PLATFORM_UNIX
        setproctitle(process_name.c_str(), argc, argv);
#endif
    }
    else
    {
        return false;
    }

    //Set plugin file
    if (plugin_cfg)
    {
        AFCPluginManager::GetInstancePtr()->SetPluginConf(plugin_cfg.Get());
    }
    else
    {
        return false;
    }

    if (logpath)
    {
        AFCPluginManager::GetInstancePtr()->SetLogPath(logpath.Get());
    }
    else
    {
        return false;
    }

    //Create back thread, for some command
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
    __except (ApplicationCrashHandler(GetExceptionInformation()))
    {
        //Do nothing for now.
    }

#else
    AFCPluginManager::GetInstancePtr()->Update();
#endif
}

int main(int argc, char* argv[])
{
    //std::error_code ec;
    //AFEndpoint ep = AFEndpoint::from_string("tcp://127.0.0.1:8080", ec);
    //if (!ec)
    //{
    //    return -1;
    //}

    if (!ParseArgs(argc, argv))
    {
        CONSOLE_LOG_NO_FILE << "Application parameter is invalid, please check it..." << std::endl;
        return 0;
    }

    PrintLogo();

    AFCPluginManager::GetInstancePtr()->Init();
    AFCPluginManager::GetInstancePtr()->PostInit();
    AFCPluginManager::GetInstancePtr()->CheckConfig();
    AFCPluginManager::GetInstancePtr()->PreUpdate();

    while (!g_exit_loop)
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            if (g_exit_loop)
            {
                break;
            }

            MainLoop();
        }
    }

    AFCPluginManager::GetInstancePtr()->PreShut();
    AFCPluginManager::GetInstancePtr()->Shut();

    g_cmd_thread.join();

    return 0;
}