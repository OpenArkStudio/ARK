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

#pragma once

#include "AFIModule.h"

#define ARK_LOG_DEBUG(format, ...) m_pLogModule->Log(AFILogModule::ARK_LOG_LEVEL_DEBUG, ARK_FILE_FUNCTION_LINE, format, ##__VA_ARGS__);
#define ARK_LOG_INFO(format, ...) m_pLogModule->Log(AFILogModule::ARK_LOG_LEVEL_INFO, ARK_FILE_FUNCTION_LINE, format, ##__VA_ARGS__);
#define ARK_LOG_WARN(format, ...) m_pLogModule->Log(AFILogModule::ARK_LOG_LEVEL_WARNING, ARK_FILE_FUNCTION_LINE, format, ##__VA_ARGS__);
#define ARK_LOG_ERROR(format, ...) m_pLogModule->Log(AFILogModule::ARK_LOG_LEVEL_ERROR, ARK_FILE_FUNCTION_LINE, format, ##__VA_ARGS__);
#define ARK_LOG_FATAL(format, ...) m_pLogModule->Log(AFILogModule::ARK_LOG_LEVEL_FATAL, ARK_FILE_FUNCTION_LINE, format, ##__VA_ARGS__);

class AFILogModule : public AFIModule
{
public:
    enum ARK_LOG_LEVEL
    {
        ARK_LOG_LEVEL_DEBUG,        // debug level
        ARK_LOG_LEVEL_INFO,         // info level
        ARK_LOG_LEVEL_WARNING,      // warning level
        ARK_LOG_LEVEL_ERROR,        // error level
        ARK_LOG_LEVEL_FATAL,        // fatal level
        ARK_LOG_LEVEL_MAX,
    };

    virtual bool Log(const ARK_LOG_LEVEL level, const char* file, const char* function, const int line, const char* format, ...) = 0;

    //virtual void LogDebug(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogInfo(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogWarning(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogError(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogFatal(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;

    //virtual void LogDebug(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogInfo(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogWarning(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogError(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogFatal(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc = "", int nLine = 0) = 0;

    //virtual void LogDebug(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogInfo(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogWarning(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogError(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    //virtual void LogFatal(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;

    //virtual bool LogDebugFunctionDump(const AFGUID ident, const int nMsg, const std::string& strArg, const char* pFunc = "", int nLine = 0) = 0;
    virtual bool ChangeLogLevel(const std::string& strLevel, const std::string& strStatus) = 0;
    virtual bool SetSwitchingValue(const bool bValue) = 0;
};