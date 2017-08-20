// -------------------------------------------------------------------------
//    @FileName         £º    AFIWorldNet_ServerModule.h
//    @Author           £º    Ark Game Tech
//    @Date             £º    2012-12-15
//    @Module           £º    AFIWorldNet_ServerModule
//
// -------------------------------------------------------------------------

#ifndef AFI_WORLDNET_SERVERMODULE_H
#define AFI_WORLDNET_SERVERMODULE_H

#include <iostream>
#include "AFIModule.h"
#include "SDK/Proto/AFMsgDefine.h"
#include "AFINetServerModule.h"

class AFIWorldNet_ServerModule
    : public AFIModule
{

public:
    virtual bool SendMsgToGame(const int nGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer = AFGUID()) = 0;
    virtual int OnObjectListEnter(const AFIDataList& self, const AFIDataList& argVar) = 0;
    virtual int OnObjectListLeave(const AFIDataList& self, const AFIDataList& argVar) = 0;
    virtual int OnPropertyEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self) = 0;
    virtual int OnRecordEnter(const AFIDataList& argVar, const AFIDataList& argGameID, const AFGUID& self) = 0;
    virtual bool SendMsgToGame(const AFIDataList& argObjectVar, const AFIDataList& argGameID,  const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData) = 0;
    virtual bool OnRecordEnterPack(ARK_SHARE_PTR<AFIRecord> pRecord, AFMsg::ObjectRecordBase* pObjectRecordBase) = 0;

    virtual ARK_SHARE_PTR<ServerData> GetSuitProxyForEnter() = 0;
    virtual AFINetServerModule* GetNetModule() = 0;

};

#endif