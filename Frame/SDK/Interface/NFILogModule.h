// -------------------------------------------------------------------------
//    @FileName         :    NFILogModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    NFILogModule
//
// -------------------------------------------------------------------------

#pragma once

#include "NFIModule.h"

class AFILogModule
    : public NFIModule
{

public:

    enum NF_LOG_LEVEL
    {
        LOG_DEBUG,        // debug调试信息(Debug和Release版本都会打印)
        LOG_INFO,         // info级别
        LOG_WARNING,      // warning级别
        LOG_ERROR,        // error级别
        LOG_FATAL,        // fatal级别
        LOG_MAX,
    };

    virtual void LogDebug(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogInfo(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogWarning(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogError(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogFatal(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;

    virtual void LogDebug(const AFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogInfo(const AFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogWarning(const AFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogError(const AFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogFatal(const AFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc = "", int nLine = 0) = 0;

    virtual void LogDebug(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogInfo(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogWarning(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogError(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogFatal(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;

    virtual bool LogDebugFunctionDump(const AFGUID ident, const int nMsg, const std::string& strArg, const char* pFunc = "", int nLine = 0) = 0;
    virtual bool ChangeLogLevel(const std::string& strLevel, const std::string& strStatus) = 0;
    virtual bool SetSwitchingValue(const bool bValue) = 0;
};