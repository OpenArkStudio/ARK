
#include "AFCNetClient.h"
#include <thread>
#include <string>
#include "SDK/Core/AFTime.h"
#pragma comment(lib,"ws2_32.lib")

#ifdef NF_DEBUG_MODE
#pragma comment(lib,"AFNetEvpp_d.lib")
#pragma comment(lib,"Core_d.lib")
#else
#pragma comment(lib,"AFNetEvpp.lib")
#pragma comment(lib,"Core.lib")
#endif
class TestClientClass
{
public:
    TestClientClass(const int nID)
    {
        //pNet = new AFCNet(this, &TestClientClass::ReciveHandler, &TestClientClass::EventHandler);
        pNet = new AFCNetClient(this, &TestClientClass::ReciveHandler, &TestClientClass::EventHandler);
        pNet->Initialization("192.168.1.143:8088", 1);
        pNet->StopAfter(60);
        bConnected = false;
        nSendMsgCount = 0;
        nReciveMsgCount = 0;
        mnID = nID;
        mnStarTime = AFTime::GetUTCTime();
        mbTestSendMsg = true;
    }

    void ReciveHandler(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        std::string str;
        str.assign(msg, nLen);
        nReciveMsgCount++;
        int nSpanTime = AFTime::GetUTCTime() - mnStarTime;
        std::cout << " mnID: " << mnID << " nSendMsgCount: " << nSendMsgCount << "nReciveMsgCount" << nReciveMsgCount  << " msg_id: " << nMsgID /*<<  " data: " << str */ << " thread_id: " << std::this_thread::get_id() << "SpanTime:" << nSpanTime << std::endl;
        pNet->SendMsgWithOutHead(nMsgID, msg, nLen, xClientID);
    };

    void EventHandler(const NetEventType e, const AFGUID& xClientID, const int nServerID)
    {
        std::cout << " event_id: " << e << " thread_id: " << std::this_thread::get_id() << std::endl;
        if(e == CONNECTED)
        {
            bConnected = true;

            TestSend();
        }
    }

    void Execute()
    {
        pNet->Execute();
    }

    bool TestSend()
    {
        if(!bConnected)
        {
            return false;
        }

        std::string strData = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
        pNet->SendMsgWithOutHead(1, strData.data(), strData.size(), 0);
        nSendMsgCount++;

        return true;
    }

    bool IsStop()
    {
        return pNet->IsStop();
    }

public:
    bool mbTestSendMsg;

public:
    AFINet* pNet;
    bool bConnected;
    int nSendMsgCount;
    int nReciveMsgCount;
    int mnID;
    int mnStarTime;
private:
};

int main(int argc, char** argv)
{
    std::list<TestClientClass*> list;

    /*   for(int i = 0; i < 1; ++i)
       {
           TestClientClass* x = new TestClientClass(i);;
           list.push_back(x);
       }*/
    TestClientClass* pNet = new TestClientClass(0);;

    int nTime = AFTime::GetUTCTime();
    int nLastTime = AFTime::GetUTCTime();
    int nCount = 0;
    while(!pNet->IsStop())
    {
        pNet->Execute();
        ////int nNowTime = AFTime::GetUTCTime();
        //std::list<TestClientClass*>::iterator it = list.begin();
        //for(it; it != list.end(); ++it)
        //{
        //    //if ((nNowTime - nTime) < 180 * 1000)
        //    //{
        //    //    /*     {
        //    //             nCount++;
        //    //         }
        //    //         else
        //    //         {

        //    //         }*/
        //    //    //std::cout << "Frame Time :" << (nNowTime - nLastTime) << std::endl;
        //    //}

        //    (*it)->Execute();
        //}

        //nLastTime = nNowTime;
        //NFSLEEP(1);
    }

    std::cout << "nSendMsgCount " << pNet->nSendMsgCount << "nReciveMsgCount:" << pNet->nReciveMsgCount << std::endl;
    system("pause");


    return 0;
}
