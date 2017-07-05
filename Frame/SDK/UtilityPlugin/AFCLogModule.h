// -------------------------------------------------------------------------
//    @FileName         :    AFCLogModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCLogModule
//    @Desc             :
// -------------------------------------------------------------------------

#pragma once

#include "SDK/Interface/AFILogModule.h"

class AFCLogModule
    : public AFILogModule
{
public:

    AFCLogModule(AFIPluginManager* p);
    virtual ~AFCLogModule() {}

    virtual bool Init();
    virtual bool Shut();

    virtual bool BeforeShut();
    virtual bool AfterInit();

    virtual bool Execute();

    ///////////////////////////////////////////////////////////////////////
    virtual void LogStack();

    virtual void LogDebug(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0);
    virtual void LogInfo(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0);
    virtual void LogWarning(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0);
    virtual void LogError(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0);
    virtual void LogFatal(const AFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0);

    virtual void LogDebug(const AFGUID self, const std::string& strDesc, const AFINT64 nInfo, const char* pFunc = "", int nLine = 0);
    virtual void LogInfo(const AFGUID self, const std::string& strDesc, const AFINT64 nInfo, const char* pFunc = "", int nLine = 0);
    virtual void LogWarning(const AFGUID self, const std::string& strDesc, const AFINT64 nInfo, const char* pFunc = "", int nLine = 0);
    virtual void LogError(const AFGUID self, const std::string& strDesc, const AFINT64 nInfo, const char* pFunc = "", int nLine = 0);
    virtual void LogFatal(const AFGUID self, const std::string& strDesc, const AFINT64 nInfo, const char* pFunc = "", int nLine = 0);

    virtual void LogDebug(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0);
    virtual void LogInfo(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0);
    virtual void LogWarning(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0);
    virtual void LogError(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0);
    virtual void LogFatal(const AFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0);

    virtual bool LogDebugFunctionDump(const AFGUID ident, const int nMsg, const std::string& strArg, const char* pFunc = "", int nLine = 0);
    virtual bool ChangeLogLevel(const std::string& strLevel, const std::string& strStatus);
    virtual bool SetSwitchingValue(const bool bValue);

protected:
    bool Log(const NF_LOG_LEVEL nll, const char* format, ...);

    static bool CheckLogFileExist(const char* filename);
    static void rolloutHandler(const char* filename, std::size_t size);

private:
    static int idx;
    bool mbSwitchingValue;
    char szBuffer[1024 * 10];
    bool mbLogSwiths[LOG_MAX];
};