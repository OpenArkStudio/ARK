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

#include "AFCLogModule.h"
#include <stdarg.h>
#include "common/easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int AFCLogModule::idx = 0;

bool AFCLogModule::CheckLogFileExist(const char* filename)
{
    std::stringstream stream;
    stream << filename << "." << ++idx;
    std::fstream file;
    file.open(stream.str(), std::ios::in);
    if(file)
    {
        return CheckLogFileExist(filename);
    }

    return false;
}

void AFCLogModule::rolloutHandler(const char* filename, std::size_t size)
{
    std::stringstream stream;
    if(!CheckLogFileExist(filename))
    {
        stream << filename << "." << idx;
        rename(filename, stream.str().c_str());
    }
}

AFCLogModule::AFCLogModule(AFIPluginManager* p)
{
    pPluginManager = p;
    mbSwitchingValue = true;
    memset(szBuffer, 0, sizeof(szBuffer));
    memset(mbLogSwitchs, 0, sizeof(mbLogSwitchs));
}

bool AFCLogModule::Init()
{
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
    el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
#if ARK_PLATFORM == PLATFORM_WIN
    el::Configurations conf("log_win.conf");
#else
    el::Configurations conf("log.conf");
#endif
    el::Loggers::reconfigureAllLoggers(conf);
    el::Helpers::installPreRollOutCallback(rolloutHandler);

    el::Logger* pLogger = el::Loggers::getLogger("default");
    if(pLogger == nullptr)
    {
        return false;
    }

    el::Configurations* pConfigurations = pLogger->configurations();

    //open debug as default
    el::Configuration debugConfiguration(el::Level::Debug, el::ConfigurationType::Enabled, "true");
    pConfigurations->set(&debugConfiguration);

    mbLogSwitchs[ARK_LOG_LEVEL_DEBUG] = pLogger->typedConfigurations()->enabled(el::Level::Debug);
    mbLogSwitchs[ARK_LOG_LEVEL_INFO] = pLogger->typedConfigurations()->enabled(el::Level::Info);
    mbLogSwitchs[ARK_LOG_LEVEL_WARNING] = pLogger->typedConfigurations()->enabled(el::Level::Warning);
    mbLogSwitchs[ARK_LOG_LEVEL_ERROR] = pLogger->typedConfigurations()->enabled(el::Level::Error);
    mbLogSwitchs[ARK_LOG_LEVEL_FATAL] = pLogger->typedConfigurations()->enabled(el::Level::Fatal);

    el::Loggers::reconfigureAllLoggers(*pConfigurations);
    return true;
}

bool AFCLogModule::Shut()
{
    el::Helpers::uninstallPreRollOutCallback();
    return true;
}

bool AFCLogModule::PreShut()
{
    return true;
}

bool AFCLogModule::PostInit()
{
    return true;
}

void AFCLogModule::Update()
{

}

bool AFCLogModule::Log(const ARK_LOG_LEVEL level, const char* file, const char* function, const int line, const char* format, ...)
{
    //error level
    if(level >= ARK_LOG_LEVEL_MAX || level < ARK_LOG_LEVEL_DEBUG)
    {
        return false;
    }

    if(!mbLogSwitchs[level])
    {
        return false;
    }

    //(file)(func:line)
    std::string func_line = std::string("(") + file + ")(" + function + ":" + ARK_TO_STRING(line) + ")";

    memset(szBuffer, 0, sizeof(szBuffer));

    va_list args;
    va_start(args, format);
    vsnprintf(szBuffer, sizeof(szBuffer) - 1, format, args);
    va_end(args);

    switch(level)
    {
    case AFILogModule::ARK_LOG_LEVEL_DEBUG:
        {
            LOG(DEBUG) << func_line << szBuffer;
        }
        break;
    case AFILogModule::ARK_LOG_LEVEL_INFO:
        {
            if(mbSwitchingValue)
            {
                LOG(INFO) << func_line << szBuffer;
            }
        }
        break;
    case AFILogModule::ARK_LOG_LEVEL_WARNING:
        {
            if(mbSwitchingValue)
            {
                LOG(WARNING) << func_line << szBuffer;
            }
        }
        break;
    case AFILogModule::ARK_LOG_LEVEL_ERROR:
        LOG(ERROR) << func_line << szBuffer;
        break;
    case AFILogModule::ARK_LOG_LEVEL_FATAL:
        LOG(FATAL) << func_line << szBuffer;
        break;
    default:
        LOG(INFO) << func_line << szBuffer;
        break;
    }

    return true;
}

//void AFCLogModule::LogStack()
//{
//    //TODO:
//}
//
//void AFCLogModule::LogDebug(const AFGUID self, const std::string& strDesc, const std::string& strInfo/* = NULL_STR*/, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_DEBUG, "GUID[%s] %s %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str(), pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_DEBUG, "GUID[%s] %s %s", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str());
//    }
//}
//
//void AFCLogModule::LogInfo(const AFGUID self, const std::string& strDesc, const std::string& strInfo/* = NULL_STR*/, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_INFO, "GUID[%s] %s %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str(), pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_INFO, "GUID[%s] %s %s", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str());
//    }
//}
//
//void AFCLogModule::LogWarning(const AFGUID self, const std::string& strDesc, const std::string& strInfo/* = NULL_STR*/, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_WARNING, "GUID[%s] %s %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str(), pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_WARNING, "GUID[%s] %s %s", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str());
//    }
//}
//
//void AFCLogModule::LogError(const AFGUID self, const std::string& strDesc, const std::string& strInfo/* = NULL_STR*/, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_ERROR, "GUID[%s] %s %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str(), pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_ERROR, "GUID[%s] %s %s", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str());
//    }
//}
//
//void AFCLogModule::LogFatal(const AFGUID self, const std::string& strDesc, const std::string& strInfo/* = NULL_STR*/, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_FATAL, "GUID[%s] %s %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str(), pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_FATAL, "GUID[%s] %s %s", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str());
//    }
//}
//
//void AFCLogModule::LogDebug(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_DEBUG, "GUID[%s] %s %d FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), nInfo, pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_DEBUG, "GUID[%s] %s %d", self.ToString().c_str(), strDesc.c_str(), nInfo);
//    }
//}
//
//void AFCLogModule::LogInfo(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_INFO, "GUID[%s] %s %d FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), nInfo, pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_INFO, "GUID[%s] %s %d", self.ToString().c_str(), strDesc.c_str(), nInfo);
//    }
//}
//
//void AFCLogModule::LogWarning(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_WARNING, "GUID[%s] %s %d FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), nInfo, pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_WARNING, "GUID[%s] %s %d", self.ToString().c_str(), strDesc.c_str(), nInfo);
//    }
//}
//
//void AFCLogModule::LogError(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_ERROR, "GUID[%s] %s %d FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), nInfo, pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_ERROR, "GUID[%s] %s %d", self.ToString().c_str(), strDesc.c_str(), nInfo);
//    }
//}
//
//void AFCLogModule::LogFatal(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_FATAL, "GUID[%s] %s %d FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), nInfo, pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_FATAL, "GUID[%s] %s %d", self.ToString().c_str(), strDesc.c_str(), nInfo);
//    }
//}
//
//void AFCLogModule::LogDebug(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_DEBUG, "GUID[%s] %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.str().c_str(), pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_DEBUG, "GUID[%s] %s", self.ToString().c_str(), strDesc.str().c_str());
//    }
//}
//
//void AFCLogModule::LogInfo(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_INFO, "GUID[%s] %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.str().c_str(), pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_INFO, "GUID[%s] %s", self.ToString().c_str(), strDesc.str().c_str());
//    }
//}
//
//void AFCLogModule::LogWarning(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_WARNING, "GUID[%s] %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.str().c_str(), pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_WARNING, "GUID[%s] %s", self.ToString().c_str(), strDesc.str().c_str());
//    }
//}
//
//void AFCLogModule::LogError(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_ERROR, "GUID[%s] %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.str().c_str(), pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_ERROR, "GUID[%s] %s", self.ToString().c_str(), strDesc.str().c_str());
//    }
//}
//
//void AFCLogModule::LogFatal(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc/* = ""*/, int nLine/* = 0*/)
//{
//    if(nLine > 0 && pFunc != nullptr)
//    {
//        Log(ARK_LOG_LEVEL_FATAL, "GUID[%s] %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.str().c_str(), pFunc, nLine);
//    }
//    else
//    {
//        Log(ARK_LOG_LEVEL_FATAL, "GUID[%s] %s", self.ToString().c_str(), strDesc.str().c_str());
//    }
//}
//
//bool AFCLogModule::LogDebugFunctionDump(const AFGUID ident, const int nMsg, const std::string& strArg, const char* func /*= ""*/, const int line /*= 0*/)
//{
//    std::ostringstream strLog;
//    strLog << strArg << " MsgID: " << nMsg;
//    LogWarning(ident, strLog, func, line);
//
//    return true;
//}

bool AFCLogModule::ChangeLogLevel(const std::string& strLevel, const std::string& strStatus)
{
    Log(ARK_LOG_LEVEL_DEBUG, ARK_FILE_FUNCTION_LINE, "Load log.cnf ------------Begin");

    el::Level logLevel = el::LevelHelper::convertFromString(strLevel.c_str());
    el::Logger* pLogger = el::Loggers::getLogger("default");
    if(pLogger == nullptr)
    {
        return false;
    }

    el::Configurations* pConfigurations = pLogger->configurations();
    el::base::TypedConfigurations* pTypeConfigurations = pLogger->typedConfigurations();
    if(pConfigurations == nullptr)
    {
        return false;
    }

    switch(logLevel)
    {
    case el::Level::Fatal:
        {
            el::Configuration errorConfiguration(el::Level::Fatal, el::ConfigurationType::Enabled, strStatus);
            pConfigurations->set(&errorConfiguration);
            mbLogSwitchs[ARK_LOG_LEVEL_FATAL] = pLogger->typedConfigurations()->enabled(el::Level::Fatal);
            break;
        }
    case el::Level::Error:
        {
            el::Configuration warnConfiguration(el::Level::Error, el::ConfigurationType::Enabled, strStatus);
            pConfigurations->set(&warnConfiguration);
            mbLogSwitchs[ARK_LOG_LEVEL_ERROR] = pLogger->typedConfigurations()->enabled(el::Level::Error);
            break;
        }
    case el::Level::Warning:
        {
            el::Configuration infoConfiguration(el::Level::Warning, el::ConfigurationType::Enabled, strStatus);
            pConfigurations->set(&infoConfiguration);
            mbLogSwitchs[ARK_LOG_LEVEL_WARNING] = pLogger->typedConfigurations()->enabled(el::Level::Warning);
            break;
        }
    case el::Level::Info:
        {
            el::Configuration debugConfiguration(el::Level::Info, el::ConfigurationType::Enabled, strStatus);
            pConfigurations->set(&debugConfiguration);
            mbLogSwitchs[ARK_LOG_LEVEL_INFO] = pLogger->typedConfigurations()->enabled(el::Level::Info);
            break;
        }
    case el::Level::Debug:
        break;
    default:
        break;
    }

    el::Loggers::reconfigureAllLoggers(*pConfigurations);

    Log(ARK_LOG_LEVEL_DEBUG, ARK_FILE_FUNCTION_LINE, "[Log] Change log level = %s", strLevel.c_str());
    Log(ARK_LOG_LEVEL_DEBUG, ARK_FILE_FUNCTION_LINE, "Load log.cnf ------------End");
    return true;
}

bool AFCLogModule::SetSwitchingValue(const bool bValue)
{
    mbSwitchingValue = bValue;

    return mbSwitchingValue;
}