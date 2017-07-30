
#include "AFCNetServer.h"
#include <thread>
#include <string>
#include <processthreadsapi.h>
#include "SDK/Core/AFTime.h"
#ifdef NF_DEBUG_MODE
#pragma comment(lib,"AFNetEvpp_d.lib")
#pragma comment(lib,"Core_d.lib")
#else
#pragma comment(lib,"AFNetEvpp.lib")
#pragma comment(lib,"Core.lib")
#endif

class TestServerClass
{
public:
    TestServerClass()
    {
        //pNet = new AFCNet(this, &TestServerClass::ReciveHandler, &TestServerClass::EventHandler);
        pNet = new AFCNetServer(this, &TestServerClass::ReciveHandler, &TestServerClass::EventHandler);
        pNet->Initialization(10000, "192.168.1.143:8088", 2, 1);
        nSendMsgCount = 0;
        nReciveMsgCount = 0;
        nStartTime = AFTime::GetUTCTime();
        nLastTime = nStartTime;
        nLastSendCount = 0;
        nLasterReciveCount = 0;
    }

    void ReciveHandler(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        std::string str;
        str.assign(msg, nLen);

        nReciveMsgCount++;
        nSendMsgCount++;
        pNet->SendMsgWithOutHead(nMsgID, msg, nLen, xClientID);
        //std::cout << " nSendMsgCount: " << nSendMsgCount << "nReciveMsgCount" << nReciveMsgCount << " fd: " << nSockIndex << " msg_id: " << nMsgID /*<<  " data: " << str*/ << " thread_id: " << GetCurrentThreadId() << std::endl;
    }

    void EventHandler(const NetEventType e, const AFGUID& xClientID, const int nServerID)
    {
        std::cout << e << " event_id: " << e << " thread_id: " << std::this_thread::get_id() << std::endl;
    }

    void Execute()
    {
        pNet->Execute();

        int nNowTime = AFTime::GetUTCTime();
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
    AFINet* pNet;
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
        x.Execute();
    }

    return 0;
}
