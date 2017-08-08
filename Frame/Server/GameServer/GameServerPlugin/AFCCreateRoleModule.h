// -------------------------------------------------------------------------
//    @FileName      :    AFCCreateRoleModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-03-29
//    @Module           :    AFCCreateRoleModule
//
// -------------------------------------------------------------------------

#ifndef _AFC_CREATEROLE_MODULE_H_
#define _AFC_CREATEROLE_MODULE_H_

#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFIKernelModule.h"
//#include "SDK/Interface/AFIDataNoSqlModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"
//#include "SDK/Interface/AFIEventProcessModule.h"
//#include "AFComm/NFPluginModule/AFIShareMemoryModule.h"
#include "SDK/Interface/AFIElementModule.h"


class AFCCreateRoleModule
    : public AFIModule
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
    int OnLoadRoleBeginEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnLoadRoleFinalEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnCreateRoleEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnDeleteRoleEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    int OnAcountDisConnectEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

private:

    //新建立的连接对象，等待他们自己发验证KEY，KEY验证后删掉
    //-1
    int mnConnectContainer;

    //选人大厅容器
    //-3
    int mnRoleHallContainer;

    AFIGameLogicModule* m_pGameLogicModule;
    //AFIEventProcessModule* m_pEventProcessModule;
    //static AFIDataBaseModule* m_pDataBaseModule;
    //AFIDataNoSqlModule* m_pNoSqlModule;
    AFIKernelModule* m_pKernelModule;
    AFIElementModule* m_pElementInfoModule;
    AFCCreateRoleModule* m_pThis;
};

#endif
