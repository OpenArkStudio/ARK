/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file  	AFIWorldNet_ServerModule.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFIWorldNet_ServerModule                                                  *
*****************************************************************************/
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
    virtual bool OnRecordEnterPack(AFRecord* pRecord, AFMsg::ObjectRecordBase* pObjectRecordBase) = 0;

    virtual ARK_SHARE_PTR<ServerData> GetSuitProxyForEnter() = 0;
    virtual AFINetServerModule* GetNetModule() = 0;

};

#endif

