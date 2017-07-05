// -------------------------------------------------------------------------
//    @FileName      :    AFCCreateRoleModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-03-29
//    @Module           :    AFCCreateRoleModule
//
// -------------------------------------------------------------------------

#ifndef _AFC_CREATEROLE_MODULE_H_
#define _AFC_CREATEROLE_MODULE_H_

#include "AFComm/NFPluginModule/AFIPluginManager.h"
#include "AFComm/NFPluginModule/AFIKernelModule.h"
#include "AFComm/NFPluginModule/AFIDataNoSqlModule.h"
#include "AFComm/NFPluginModule/AFIGameLogicModule.h"
#include "AFComm/NFPluginModule/AFIEventProcessModule.h"
//#include "AFComm/NFPluginModule/AFIShareMemoryModule.h"
#include "AFComm/NFPluginModule/AFIElementInfoModule.h"


class AFCCreateRoleModule
    : public AFILogicModule
{
public:
    AFCCreateRoleModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCCreateRoleModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute(const float fLasFrametime, const float fStartedTime);
    virtual bool AfterInit();

protected:
    static int OnLoadRoleBeginEvent(const AFIDENTID& object, const int nEventID, const AFIValueList& var);

    static int OnLoadRoleFinalEvent(const AFIDENTID& object, const int nEventID, const AFIValueList& var);

    static int OnCreateRoleEvent(const AFIDENTID& object, const int nEventID, const AFIValueList& var);

    static int OnDeleteRoleEvent(const AFIDENTID& object, const int nEventID, const AFIValueList& var);

    static int OnAcountDisConnectEvent(const AFIDENTID& object, const int nEventID, const AFIValueList& var);

private:

    //新建立的连接对象，等待他们自己发验证KEY，KEY验证后删掉
    //-1
    static int mnConnectContainer;

    //选人大厅容器
    //-3
    static int mnRoleHallContainer;

    static AFIGameLogicModule* m_pGameLogicModule;
    static AFIEventProcessModule* m_pEventProcessModule;
    //static AFIDataBaseModule* m_pDataBaseModule;
    static AFIDataNoSqlModule* m_pNoSqlModule;
    static AFIKernelModule* m_pKernelModule;
    static AFIElementInfoModule* m_pElementInfoModule;
    static AFCCreateRoleModule* m_pThis;
};

#endif
