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

class AFILogModule : public AFIModule
{
public:
    enum ARK_LOG_LEVEL
    {
        ARK_LOG_DEBUG,        // debug调试信息(Debug和Release版本都会打印)
        ARK_LOG_INFO,         // info级别
        ARK_LOG_WARNING,      // warning级别
        ARK_LOG_ERROR,        // error级别
        ARK_LOG_FATAL,        // fatal级别
        ARK_LOG_MAX,
    };

    virtual void LogDebug(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogInfo(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogWarning(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogError(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogFatal(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;

    virtual void LogDebug(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogInfo(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogWarning(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogError(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogFatal(const AFGUID self, const std::string& strDesc, const int64_t nInfo, const char* pFunc = "", int nLine = 0) = 0;

    virtual void LogDebug(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogInfo(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogWarning(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogError(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogFatal(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;

    virtual bool LogDebugFunctionDump(const AFGUID ident, const int nMsg, const std::string& strArg, const char* pFunc = "", int nLine = 0) = 0;
    virtual bool ChangeLogLevel(const std::string& strLevel, const std::string& strStatus) = 0;
    virtual bool SetSwitchingValue(const bool bValue) = 0;
};

