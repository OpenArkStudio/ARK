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
#include "SDK/Proto/NFMsgDefine.h"
#include "AFComm/NFPluginModule/AFINetModule.h"

class AFIWorldNet_ServerModule
    : public AFIModule
{

public:
    virtual bool SendMsgToGame(const int nGameID, const NFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const AFGUID nPlayer = AFGUID()) = 0;
    virtual int OnObjectListEnter(const AFDataList& self, const AFDataList& argVar) = 0;
    virtual int OnObjectListLeave(const AFDataList& self, const AFDataList& argVar) = 0;
    virtual int OnPropertyEnter(const AFDataList& argVar, const AFDataList& argGameID, const AFGUID& self) = 0;
    virtual int OnRecordEnter(const AFDataList& argVar, const AFDataList& argGameID, const AFGUID& self) = 0;
    virtual bool SendMsgToGame(const AFDataList& argObjectVar, const AFDataList& argGameID,  const NFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData) = 0;
    virtual bool OnRecordEnterPack(NF_SHARE_PTR<AFIRecord> pRecord, NFMsg::ObjectRecordBase* pObjectRecordBase) = 0;

    virtual NF_SHARE_PTR<ServerData> GetSuitProxyForEnter() = 0;
    virtual AFINetModule* GetNetModule() = 0;

};

#endif