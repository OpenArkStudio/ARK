// -------------------------------------------------------------------------
//    @FileName         £º    NFIWorldNet_ServerModule.h
//    @Author           £º    Ark Game Tech
//    @Date             £º    2012-12-15
//    @Module           £º    NFIWorldNet_ServerModule
//
// -------------------------------------------------------------------------

#ifndef NFI_WORLDNET_SERVERMODULE_H
#define NFI_WORLDNET_SERVERMODULE_H

#include <iostream>
#include "NFIModule.h"
#include "NFComm/NFMessageDefine/NFMsgDefine.h"
#include "NFComm/NFPluginModule/NFINetModule.h"

class NFIWorldNet_ServerModule
    : public NFIModule
{

public:
    virtual bool SendMsgToGame(const int nGameID, const NFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const NFGUID nPlayer = NFGUID()) = 0;
    virtual int OnObjectListEnter(const AFDataList& self, const AFDataList& argVar) = 0;
    virtual int OnObjectListLeave(const AFDataList& self, const AFDataList& argVar) = 0;
    virtual int OnPropertyEnter(const AFDataList& argVar, const AFDataList& argGameID, const NFGUID& self) = 0;
    virtual int OnRecordEnter(const AFDataList& argVar, const AFDataList& argGameID, const NFGUID& self) = 0;
    virtual bool SendMsgToGame(const AFDataList& argObjectVar, const AFDataList& argGameID,  const NFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData) = 0;
    virtual bool OnRecordEnterPack(NF_SHARE_PTR<NFIRecord> pRecord, NFMsg::ObjectRecordBase* pObjectRecordBase) = 0;

    virtual NF_SHARE_PTR<ServerData> GetSuitProxyForEnter() = 0;
    virtual NFINetModule* GetNetModule() = 0;

};

#endif