#include "NFCLogModule.h"
#include <stdarg.h>
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int NFCLogModule::idx = 0;

bool NFCLogModule::CheckLogFileExist(const char* filename)
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

void NFCLogModule::rolloutHandler(const char* filename, std::size_t size)
{
    std::stringstream stream;
    if(!CheckLogFileExist(filename))
    {
        stream << filename << "." << idx;
        rename(filename, stream.str().c_str());
    }
}

NFCLogModule::NFCLogModule(NFIPluginManager* p)
{
    pPluginManager = p;
    mbSwitchingValue = true;
    memset(szBuffer, 0, sizeof(szBuffer));
    memset(mbLogSwiths, 0, sizeof(mbLogSwiths));
}

bool NFCLogModule::Init()
{
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
    el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
#if NF_PLATFORM == NF_PLATFORM_WIN
    el::Configurations conf("log_win.conf");
#else
    el::Configurations conf("log.conf");
#endif
    el::Loggers::reconfigureAllLoggers(conf);
    el::Helpers::installPreRollOutCallback(rolloutHandler);

    el::Logger* pLogger = el::Loggers::getLogger("default");
    if(NULL == pLogger)
    {
        return false;
    }

    el::Configurations* pConfigurations = pLogger->configurations();

    //open debug as default
    el::Configuration debugConfiguration(el::Level::Debug, el::ConfigurationType::Enabled, "true");
    pConfigurations->set(&debugConfiguration);

    //获取每种级别log的输出开关
    mbLogSwiths[LOG_DEBUG] = pLogger->typedConfigurations()->enabled(el::Level::Debug);
    mbLogSwiths[LOG_INFO] = pLogger->typedConfigurations()->enabled(el::Level::Info);
    mbLogSwiths[LOG_WARNING] = pLogger->typedConfigurations()->enabled(el::Level::Warning);
    mbLogSwiths[LOG_ERROR] = pLogger->typedConfigurations()->enabled(el::Level::Error);
    mbLogSwiths[LOG_FATAL] = pLogger->typedConfigurations()->enabled(el::Level::Fatal);

    el::Loggers::reconfigureAllLoggers(*pConfigurations);
    return true;
}

bool NFCLogModule::Shut()
{
    el::Helpers::uninstallPreRollOutCallback();
    return true;
}

bool NFCLogModule::BeforeShut()
{
    return true;
}

bool NFCLogModule::AfterInit()
{
    return true;
}

bool NFCLogModule::Execute()
{
    return true;
}

bool NFCLogModule::Log(const NF_LOG_LEVEL nll, const char* format, ...)
{
    //error level
    if(nll >= LOG_MAX || nll < LOG_DEBUG)
    {
        return false;
    }

    //开关不开启，不用执行格式化和输出内容
    if(!mbLogSwiths[nll])
    {
        return false;
    }

    memset(szBuffer, 0, sizeof(szBuffer));

    va_list args;
    va_start(args, format);
    vsnprintf(szBuffer, sizeof(szBuffer) - 1, format, args);
    va_end(args);

    switch(nll)
    {
    case NFILogModule::LOG_DEBUG:
        {
            LOG(DEBUG) << szBuffer;
        }
        break;
    case NFILogModule::LOG_INFO:
        {
            if(mbSwitchingValue)
            {
                LOG(INFO) << szBuffer;
            }
        }
        break;
    case NFILogModule::LOG_WARNING:
        {
            if(mbSwitchingValue)
            {
                LOG(WARNING) << szBuffer;
            }
        }
        break;
    case NFILogModule::LOG_ERROR:
        LOG(ERROR) << szBuffer;
        break;
    case NFILogModule::LOG_FATAL:
        LOG(FATAL) << szBuffer;
        break;
    default:
        LOG(INFO) << szBuffer;
        break;
    }

    return true;
}

void NFCLogModule::LogStack()
{
    //To Add
    //#ifdef NF_DEBUG_MODE
    //    time_t t = time(0);
    //    char szDmupName[MAX_PATH];
    //    tm* ptm = localtime(&t);
    //
    //    sprintf(szDmupName, "%d_%d_%d_%d_%d_%d.dmp",  ptm->tm_year + 1900, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    //    // 创建Dump文件
    //    HANDLE hDumpFile = CreateFile(szDmupName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    //
    //    // Dump信息
    //    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    //    //dumpInfo.ExceptionPointers = pException;
    //    dumpInfo.ThreadId = GetCurrentThreadId();
    //    dumpInfo.ClientPointers = TRUE;
    //
    //    // 写入Dump文件内容
    //    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    //
    //    CloseHandle(hDumpFile);
    //#endif
}

void NFCLogModule::LogDebug(const NFGUID self, const std::string& strDesc, const std::string& strInfo/* = NULL_STR*/, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_DEBUG, "GUID[%s] %s %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str(), pFunc, nLine);
    }
    else
    {
        Log(LOG_DEBUG, "GUID[%s] %s %s", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str());
    }
}

void NFCLogModule::LogInfo(const NFGUID self, const std::string& strDesc, const std::string& strInfo/* = NULL_STR*/, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_INFO, "GUID[%s] %s %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str(), pFunc, nLine);
    }
    else
    {
        Log(LOG_INFO, "GUID[%s] %s %s", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str());
    }
}

void NFCLogModule::LogWarning(const NFGUID self, const std::string& strDesc, const std::string& strInfo/* = NULL_STR*/, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_WARNING, "GUID[%s] %s %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str(), pFunc, nLine);
    }
    else
    {
        Log(LOG_WARNING, "GUID[%s] %s %s", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str());
    }
}

void NFCLogModule::LogError(const NFGUID self, const std::string& strDesc, const std::string& strInfo/* = NULL_STR*/, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_ERROR, "GUID[%s] %s %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str(), pFunc, nLine);
    }
    else
    {
        Log(LOG_ERROR, "GUID[%s] %s %s", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str());
    }
}

void NFCLogModule::LogFatal(const NFGUID self, const std::string& strDesc, const std::string& strInfo/* = NULL_STR*/, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_FATAL, "GUID[%s] %s %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str(), pFunc, nLine);
    }
    else
    {
        Log(LOG_FATAL, "GUID[%s] %s %s", self.ToString().c_str(), strDesc.c_str(), strInfo.c_str());
    }
}

void NFCLogModule::LogDebug(const NFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_DEBUG, "GUID[%s] %s %d FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), nInfo, pFunc, nLine);
    }
    else
    {
        Log(LOG_DEBUG, "GUID[%s] %s %d", self.ToString().c_str(), strDesc.c_str(), nInfo);
    }
}

void NFCLogModule::LogInfo(const NFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_INFO, "GUID[%s] %s %d FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), nInfo, pFunc, nLine);
    }
    else
    {
        Log(LOG_INFO, "GUID[%s] %s %d", self.ToString().c_str(), strDesc.c_str(), nInfo);
    }
}

void NFCLogModule::LogWarning(const NFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_WARNING, "GUID[%s] %s %d FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), nInfo, pFunc, nLine);
    }
    else
    {
        Log(LOG_WARNING, "GUID[%s] %s %d", self.ToString().c_str(), strDesc.c_str(), nInfo);
    }
}

void NFCLogModule::LogError(const NFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_ERROR, "GUID[%s] %s %d FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), nInfo, pFunc, nLine);
    }
    else
    {
        Log(LOG_ERROR, "GUID[%s] %s %d", self.ToString().c_str(), strDesc.c_str(), nInfo);
    }
}

void NFCLogModule::LogFatal(const NFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_FATAL, "GUID[%s] %s %d FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.c_str(), nInfo, pFunc, nLine);
    }
    else
    {
        Log(LOG_FATAL, "GUID[%s] %s %d", self.ToString().c_str(), strDesc.c_str(), nInfo);
    }
}

void NFCLogModule::LogDebug(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_DEBUG, "GUID[%s] %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.str().c_str(), pFunc, nLine);
    }
    else
    {
        Log(LOG_DEBUG, "GUID[%s] %s", self.ToString().c_str(), strDesc.str().c_str());
    }
}

void NFCLogModule::LogInfo(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_INFO, "GUID[%s] %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.str().c_str(), pFunc, nLine);
    }
    else
    {
        Log(LOG_INFO, "GUID[%s] %s", self.ToString().c_str(), strDesc.str().c_str());
    }
}

void NFCLogModule::LogWarning(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_WARNING, "GUID[%s] %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.str().c_str(), pFunc, nLine);
    }
    else
    {
        Log(LOG_WARNING, "GUID[%s] %s", self.ToString().c_str(), strDesc.str().c_str());
    }
}

void NFCLogModule::LogError(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_ERROR, "GUID[%s] %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.str().c_str(), pFunc, nLine);
    }
    else
    {
        Log(LOG_ERROR, "GUID[%s] %s", self.ToString().c_str(), strDesc.str().c_str());
    }
}

void NFCLogModule::LogFatal(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc/* = ""*/, int nLine/* = 0*/)
{
    if(nLine > 0 && pFunc != NULL)
    {
        Log(LOG_FATAL, "GUID[%s] %s FUNC[%s] LINE[%d]", self.ToString().c_str(), strDesc.str().c_str(), pFunc, nLine);
    }
    else
    {
        Log(LOG_FATAL, "GUID[%s] %s", self.ToString().c_str(), strDesc.str().c_str());
    }
}

bool NFCLogModule::LogDebugFunctionDump(const NFGUID ident, const int nMsg, const std::string& strArg, const char* func /*= ""*/, const int line /*= 0*/)
{
    //#ifdef NF_DEBUG_MODE
    std::ostringstream strLog;
    strLog << strArg << " MsgID: " << nMsg;
    LogWarning(ident, strLog, func, line);
    //#endif
    return true;
}

bool NFCLogModule::ChangeLogLevel(const std::string& strLevel, const std::string& strStatus)
{
    LogDebug(NULL_GUID, "Load log.cnf ------------Begin", " ");

    // 先用默认配置初始化，然后再调整log等级
    //Init();

    el::Level logLevel = el::LevelHelper::convertFromString(strLevel.c_str());
    el::Logger* pLogger = el::Loggers::getLogger("default");
    if(NULL == pLogger)
    {
        return false;
    }

    el::Configurations* pConfigurations = pLogger->configurations();
    el::base::TypedConfigurations* pTypeConfigurations = pLogger->typedConfigurations();
    if(NULL == pConfigurations)
    {
        return false;
    }

    // log级别为debug, info, warning, error, fatal(级别逐渐提高)
    // 当传入为info时，则高于(包含)info的级别会输出
    // !!!!!! NOTICE:故意没有break，请千万注意 !!!!!!
    switch(logLevel)
    {
    case el::Level::Fatal:
        {
            el::Configuration errorConfiguration(el::Level::Fatal, el::ConfigurationType::Enabled, strStatus);
            pConfigurations->set(&errorConfiguration);
            mbLogSwiths[LOG_FATAL] = pLogger->typedConfigurations()->enabled(el::Level::Fatal);
            break;
        }
    case el::Level::Error:
        {
            el::Configuration warnConfiguration(el::Level::Error, el::ConfigurationType::Enabled, strStatus);
            pConfigurations->set(&warnConfiguration);
            mbLogSwiths[LOG_ERROR] = pLogger->typedConfigurations()->enabled(el::Level::Error);
            break;
        }
    case el::Level::Warning:
        {
            el::Configuration infoConfiguration(el::Level::Warning, el::ConfigurationType::Enabled, strStatus);
            pConfigurations->set(&infoConfiguration);
            mbLogSwiths[LOG_WARNING] = pLogger->typedConfigurations()->enabled(el::Level::Warning);
            break;
        }
    case el::Level::Info:
        {
            el::Configuration debugConfiguration(el::Level::Info, el::ConfigurationType::Enabled, strStatus);
            pConfigurations->set(&debugConfiguration);
            mbLogSwiths[LOG_INFO] = pLogger->typedConfigurations()->enabled(el::Level::Info);
            break;
        }
    case el::Level::Debug:
        break;
    default:
        break;
    }

    el::Loggers::reconfigureAllLoggers(*pConfigurations);

    std::ostringstream strLog;
    strLog << "[Log] Change log level = " << strLevel;
    LogDebug(NULL_GUID, strLog, __FUNCTION__, __LINE__);

    LogDebug(NULL_GUID, "Load log.cnf ------------End");

    return true;
}

bool NFCLogModule::SetSwitchingValue(const bool bValue)
{
    mbSwitchingValue = bValue;

    return mbSwitchingValue;
}