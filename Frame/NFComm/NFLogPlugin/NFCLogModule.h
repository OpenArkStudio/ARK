// -------------------------------------------------------------------------
//    @FileName			:    NFCLogModule.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-15
//    @Module           :    NFCLogModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef NFC_LOG_MODULE_H
#define NFC_LOG_MODULE_H

#include "NFComm/NFPluginModule/NFILogModule.h"

class NFCLogModule
    : public NFILogModule
{
public:

    NFCLogModule(NFIPluginManager* p);
    virtual ~NFCLogModule() {}

    virtual bool Init();
    virtual bool Shut();

    virtual bool BeforeShut();
    virtual bool AfterInit();

    virtual bool Execute(const float fLasFrametime, const float fStartedTime);

    ///////////////////////////////////////////////////////////////////////
    virtual void LogStack();

    virtual void LogDebug(const NFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0);
    virtual void LogInfo(const NFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0);
    virtual void LogWarning(const NFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0);
    virtual void LogError(const NFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0);
    virtual void LogFatal(const NFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0);

    virtual void LogDebug(const NFGUID self, const std::string& strDesc, const int nInfo, const char* pFunc = "", int nLine = 0);
    virtual void LogInfo(const NFGUID self, const std::string& strDesc, const int nInfo, const char* pFunc = "", int nLine = 0);
    virtual void LogWarning(const NFGUID self, const std::string& strDesc, const int nInfo, const char* pFunc = "", int nLine = 0);
    virtual void LogError(const NFGUID self, const std::string& strDesc, const int nInfo, const char* pFunc = "", int nLine = 0);
    virtual void LogFatal(const NFGUID self, const std::string& strDesc, const int nInfo, const char* pFunc = "", int nLine = 0);

    virtual void LogDebug(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0);
    virtual void LogInfo(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0);
    virtual void LogWarning(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0);
    virtual void LogError(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0);
    virtual void LogFatal(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0);

    virtual bool LogDebugFunctionDump(const NFGUID ident, const int nMsg, const std::string& strArg, const char* pFunc = "", int nLine = 0);
    virtual bool ChangeLogLevel(const std::string& strLevel, const std::string& strStatus);
    virtual bool SetSwitchingValue(const bool bValue);

protected:
    //friend class NFCKernelModule;

    virtual bool Log(const NF_LOG_LEVEL nll, const char* format, ...);

    static bool CheckLogFileExist(const char* filename);
    static void rolloutHandler(const char* filename, std::size_t size);

    //NFIConcurrentModule* m_pConcurrentModule;
    //NFIActor* m_pActor;
    //NFIActor<std::string>* m_pActor;
    //NFIActor<std::string>* m_pActor1;
    //NFIActor<std::string>* m_pActor2;
    //NFIActor<std::string>* m_pActor3;
    //NFIReceiver<std::string>* m_pReceiver;

private:
    static int idx;
    bool mbSwitchingValue;
};

#endif
