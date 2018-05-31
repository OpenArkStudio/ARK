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
// * @file      TestServer.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     TestServer                                                  *
*****************************************************************************/
#include "AFCNetServer.h"
#include <thread>
#include <string>
#include <processthreadsapi.h>
#include "SDK/Core/Base/AFTime.hpp"
#include "SDK/Core/Base/AFPlatform.hpp"
#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
#pragma comment(lib,"AFNet_d.lib")
#pragma comment(lib,"AFCore_d.lib")
#pragma comment(lib,"brynet.lib")
#else
#pragma comment(lib,"AFNet.lib")
#pragma comment(lib,"brynet.lib")
#pragma comment(lib,"AFCore.lib")
#endif

class TestServerClass
{
public:
    TestServerClass()
    {
        //pNet = new AFCNet(this, &TestServerClass::ReciveHandler, &TestServerClass::EventHandler);
        //pNet = new AFCNetServer(this, &TestServerClass::ReciveHandler, &TestServerClass::EventHandler);
        pNet = new AFCNetServer(this, &TestServerClass::ReciveHandler, &TestServerClass::EventHandler);
        pNet->Start(10000, "127.0.0.1:8088", 2, 1);
        nSendMsgCount = 0;
        nReciveMsgCount = 0;
        nStartTime = AFCTimeBase::GetInstance().GetUTCTime();
        nLastTime = nStartTime;
        nLastSendCount = 0;
        nLasterReciveCount = 0;
    }

    void ReciveHandler(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID)
    {
        nReciveMsgCount++;
        nSendMsgCount++;
        pNet->SendMsgWithOutHead(nMsgID, msg, nLen, xClientID, 0);
        //std::cout << " nSendMsgCount: " << nSendMsgCount << "nReciveMsgCount" << nReciveMsgCount << " fd: " << nSockIndex << " msg_id: " << nMsgID /*<<  " data: " << str*/ << " thread_id: " << GetCurrentThreadId() << std::endl;
    }

    void EventHandler(const NetEventType e, const AFGUID& xClientID, const int nServerID)
    {
        std::cout << e << " event_id: " << e << " thread_id: " << std::this_thread::get_id() << std::endl;
    }

    void Update()
    {
        pNet->Update();

        int nNowTime = AFCTimeBase::GetInstance().GetUTCTime();
        int nSpanTime = nNowTime - nLastTime;
        int nAllSpanTime = nNowTime - nStartTime;
        if(nSpanTime > 5 && nAllSpanTime > 0)
        {
            nLastTime = nNowTime;

            const int nLastPerSend = (nSendMsgCount - nLastSendCount) / nSpanTime;
            const int nLastPerReceive = (nReciveMsgCount - nLasterReciveCount) / nSpanTime;


            const int nToltalPerSend = nSendMsgCount / nAllSpanTime;
            const int nToltalPerReceive = nReciveMsgCount / nAllSpanTime;

            nLastSendCount = nSendMsgCount;
            nLasterReciveCount = nReciveMsgCount;

            std::cout << " All Send: [" << nSendMsgCount << "] All Receive: [" << nReciveMsgCount << "] All Per Send per second : [" << nToltalPerSend << "] All Per Receive per second : [" << nToltalPerReceive <<
                      "] Last Second Per Send :[" << nLastPerSend << "] Last Second Per Received [" << nLastPerReceive /*<<  " data: " << str*/ << "] thread_id: " << GetCurrentThreadId() << std::endl;
        }
    }

protected:
    AFINet * pNet;
    int nSendMsgCount;
    int nReciveMsgCount;
    int64_t nStartTime;
    int nLastTime;

    int nLasterReciveCount;
    int nLastSendCount;
};

int main(int argc, char** argv)
{
    TestServerClass x;

    int64_t nNowMS = 0;
    int64_t nFrameTime = 0;
    int64_t nPreFrameMS = 0;
    while(1)
    {
        /*       nNowMS = AFTime::GetNowMillisecond();
               nFrameTime = nNowMS - nPreFrameMS;
               nPreFrameMS = nNowMS;
               if(nFrameTime < 10 && nFrameTime >= 0)
               {
                   int64_t nNeeSleepMS = 10 - (nNowMS - nPreFrameMS);
                   std::this_thread::sleep_for(std::chrono::milliseconds(nNeeSleepMS));
               }*/
        x.Update();
    }

    return 0;
}

