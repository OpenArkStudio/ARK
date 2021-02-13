/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "base/AFPlatform.hpp"
#include "base/AFBus.hpp"
#include "base/AFMacros.hpp"
#include "base/AFDep.hpp"
#include "base/time/AFDateTime.hpp"
#include "base/AFMisc.hpp"
#include "base/AFXml.hpp"
#include "base/AFPluginManager.hpp"

using namespace ark;

bool g_exit_loop = false;

#ifdef ARK_PLATFORM_WIN
// command control
std::thread g_cmd_thread;

// mini-dump
void CreateDumpFile(const std::string& strDumpFilePathName, EXCEPTION_POINTERS* pException)
{
    HANDLE hDumpFile =
        CreateFile(strDumpFilePathName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

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
    std::string dump_name =
        ARK_FORMAT("{}-{:04d}{:02d}{:02d}_{:02d}_{:02d}_{:02d}.dmp", AFPluginManager::instance()->GetAppName(),
            now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond());

    CreateDumpFile(dump_name.c_str(), pException);

    FatalAppExit(-1, dump_name.c_str());
    return EXCEPTION_EXECUTE_HANDLER;
}

void CloseXButton()
{
    HWND hWnd = GetConsoleWindow();

    if (hWnd)
    {
        HMENU hMenu = GetSystemMenu(hWnd, FALSE);
        EnableMenuItem(hMenu, SC_CLOSE, MF_DISABLED | MF_BYCOMMAND);
    }
}

#elif defined(ARK_PLATFORM_LINUX) || defined(ARK_PLATFORM_DARWIN)

void KillHandler(int signum)
{
    ARK_LOG_INFO("Server stopped by receiving a signal {}", signum);
    g_exit_loop = true;
}

void InitDaemon()
{
#ifdef ARK_PLATFORM_DARWIN
    pid_t pid;
    int ret = posix_spawn(&pid, "/dev/null", nullptr, nullptr, nullptr, nullptr);
#else
    int ret = daemon(1, 0);
#endif
    ARK_ASSERT_NO_EFFECT(ret == 0);
}
#endif

void PrintLogo()
{
#ifdef ARK_PLATFORM_WIN
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif

    std::string logo = R"(
*******************************************************************************
__   __   ______      _____                        ___         _    
\ \ / /   |  _  \    |  _  |                      / _ \       | |   
 \ V /    | | | |    | | | | _ __    ___  _ __   / /_\ \ _ __ | | __
 /   \    | | | |    | | | || '_ \  / _ \| '_ \  |  _  || '__|| |/ /
/ /^\ \ _ | |/ /_    \ \_/ /| |_) ||  __/| | | | | | | || |   |   < 
\/   \/(_)|___/(_)    \___/ | .__/  \___||_| |_| \_| |_/|_|   |_|\_\
                            | |                                     
                            |_|                                     
                            
Copyright 2020 © X.D. NetWork Inc.
*******************************************************************************
)";

    ARK_LOG_INFO("{}", logo);
    ARK_LOG_INFO("ARK Server start ...");

#ifdef ARK_PLATFORM_WIN
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

#ifdef ARK_PLATFORM_WIN
void CreateBackThread()
{
    auto thread_func = []() {
        while (!g_exit_loop)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            std::string s;
            std::cin >> s;
            AFStringUtils::to_lower(s);
            if (s == "exit")
            {
                g_exit_loop = true;
            }
        }
    };

    g_cmd_thread = std::thread(thread_func);
}
#endif // ARK_PLATFORM_WIN

bool ParseArgs(int argc, char* argv[])
{
#if defined(ARK_PLATFORM_LINUX) || defined(ARK_PLATFORM_DARWIN)
    // ignore signals
    signal(SIGHUP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    // interupt signals
    signal(SIGINT, KillHandler);
    signal(SIGTERM, KillHandler);
    signal(SIGSTOP, KillHandler);
#endif

    auto close_x_button = []() {
#ifdef ARK_PLATFORM_WIN
        SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
        CloseXButton();
#endif
    };

    auto use_daemon = []() {
#if defined(ARK_PLATFORM_LINUX) || defined(ARK_PLATFORM_DARWIN)
        InitDaemon();
#endif
    };

    args::ArgumentParser parser(
        "Here is ark plugin loader argument tools", "If you have any questions, please report an issue in GitHub.");
    args::HelpFlag help(parser, "help", "Display the help menu", {'h', "help"});
    args::ActionFlag xbutton(parser, "close", "Close [x] button in Windows", {'x'}, close_x_button);
    args::ActionFlag daemon(parser, "daemon", "Run application as daemon", {'d'}, use_daemon);

    args::ValueFlag<std::string> busid(parser, "busid", "Set application id(like IP address: 8.8.8.8)", {'b', "busid"},
        "8.8.8.8", args::Options::Required | args::Options::Single);

    args::ValueFlag<std::string> name(parser, "name", "Set application name", {'n', "name"}, "my-server",
        args::Options::Required | args::Options::Single);

    args::ValueFlag<std::string> app_conf(parser, "app config path", "Set application configuration", {'a', "app_conf"},
        "some.app.conf", args::Options::Required | args::Options::Single);

    args::ValueFlag<std::string> log_conf(parser, "log config path", "Set application log configuration",
        {'l', "log_conf"}, "./conf/log.conf", args::Options::Single);

    // start parse argument list
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help& h)
    {
        std::cout << parser << ", error = " << h.what() << std::endl;
        return false;
    }
    catch (args::ParseError& e)
    {
        std::cout << parser << ", error = " << e.what() << std::endl;
        return false;
    }
    catch (args::ValidationError& e)
    {
        std::cout << parser << ", error = " << e.what() << std::endl;
        return false;
    }

    // Set bus id
    if (busid)
    {
        std::vector<std::string> slices;
        AFStringUtils::split(slices, busid.Get(), '.');
        if (slices.size() != 4)
        {
            std::cout << "Bus id is invalid, it likes 8.8.8.8" << std::endl;
            return false;
        }

        AFBusAddr bus_addr;
        bus_addr.FromString(busid.Get());
        AFPluginManager::instance()->SetBusID(bus_addr.bus_id);
    }
    else
    {
        return false;
    }

    // Set app name
    if (name)
    {
        AFPluginManager::instance()->SetAppName(name.Get());

        std::string process_name =
            ARK_FORMAT("{}-{}-{}", name.Get(), busid.Get(), AFPluginManager::instance()->GetBusID());
        // Set process name
#ifdef ARK_PLATFORM_WIN
        SetConsoleTitle(process_name.c_str());
#elif defined(ARK_PLATFORM_LINUX)
        // Do not need to change process name
#endif
    }
    else
    {
        return false;
    }

    // Set plugin file
    if (app_conf)
    {
        AFPluginManager::instance()->SetAppConf(app_conf.Get());
    }
    else
    {
        return false;
    }

    // log_conf is optional arg.
    AFLogger::Init(nullptr);
    auto log_file_name = ARK_FORMAT("{}{}{}{}", AFPluginManager::instance()->GetAppName(), "_",
        AFMisc::Bus2Str(AFPluginManager::instance()->GetBusID()), ".log");
    AFLogger::instance()->InitLogger(log_conf.Get(), log_file_name);

#ifdef ARK_PLATFORM_WIN
    // Create back thread, for some command
    CreateBackThread();
#endif

    return true;
}

void MainLoop()
{
#ifdef ARK_PLATFORM_WIN
    __try
    {
        AFPluginManager::instance()->Update(AFDateTime::GetNowTime());
    }
    __except (ApplicationCrashHandler(GetExceptionInformation()))
    {
        // Do nothing for now.
    }
#else
    AFPluginManager::instance()->Update(AFDateTime::GetNowTime());
#endif
}

// load app configuration
bool LoadPluginConf(std::string const& app_conf_path, OUT std::string& plugin_library_dir,
    OUT std::string& plugin_conf_dir, OUT std::unordered_map<std::string, std::string>& plugins)
{
    AFXml xml_doc(app_conf_path);

    auto root_node = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root_node.IsValid(), false);

    auto plugins_node = root_node.FindNode("plugins");
    ARK_ASSERT_RET_VAL(plugins_node.IsValid(), false);

    plugin_library_dir = plugins_node.GetString("plugin_lib_dir");
    plugin_conf_dir = plugins_node.GetString("plugin_conf_dir");

    for (auto plugin_node = plugins_node.FindNode("plugin"); plugin_node.IsValid(); plugin_node.NextNode())
    {
        std::string plugin_name = plugin_node.GetString("name");
        std::string plugin_conf = plugin_node.GetString("conf");
        auto ret = plugins.insert(std::make_pair(plugin_name, plugin_conf)).second;
        ARK_ASSERT_CONTINUE(ret);
    }

    return true;
}

int main(int argc, char* argv[])
{
    if (!ParseArgs(argc, argv))
    {
        std::cout << "Application parameter is invalid, please check it..." << std::endl;
        return 0;
    }

    PrintLogo();

    const auto& app_conf_path = AFPluginManager::instance()->GetAppConf();
    std::string plugin_library_dir;
    std::string plugin_conf_dir;
    std::unordered_map<std::string, std::string> plugins;
    // load app conf
    if (!LoadPluginConf(app_conf_path, plugin_library_dir, plugin_conf_dir, plugins))
    {
        return 0;
    }

    AFPluginManager::instance()->Start(AFDateTime::GetNowTime(), plugin_library_dir, plugin_conf_dir, plugins);

    for (;; std::this_thread::sleep_for(std::chrono::milliseconds(1)))
    {
        if (g_exit_loop)
        {
            break;
        }

        MainLoop();
    }

    AFPluginManager::instance()->Stop(AFDateTime::GetNowTime());

#ifdef ARK_PLATFORM_WIN
    g_cmd_thread.join();
#endif

    ARK_LOG_INFO("{}", "ARK Server stop ...");

    return 0;
}
