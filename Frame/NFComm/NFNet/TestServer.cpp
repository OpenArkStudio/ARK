
#include "NFCNet.h"
#include <thread>
#include <string>
#include <processthreadsapi.h>
#include "NFCMulNet.h"
#ifdef NF_DEBUG_MODE
#pragma comment(lib,"NFNet_d.lib")
#pragma comment(lib,"NFCore_d.lib")
#else
#pragma comment(lib,"NFNet.lib")
#pragma comment(lib,"NFCore.lib")
#endif

class TestServerClass
{
public:
    TestServerClass()
    {
        pNet = new NFCMulNet(this, &TestServerClass::ReciveHandler, &TestServerClass::EventHandler);
        pNet->Initialization(1000, 8088, 2, 4);
        nSendMsgCount = 0;
        nReciveMsgCount = 0;

    }

    void ReciveHandler(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
    {
        std::string str;
        str.assign(msg, nLen);

        nReciveMsgCount++;
        nSendMsgCount++;
        pNet->SendMsgWithOutHead(nMsgID, msg, nLen, nSockIndex);
        std::cout << " nSendMsgCount: " << nSendMsgCount << "nReciveMsgCount" << nReciveMsgCount << " fd: " << nSockIndex << " msg_id: " << nMsgID /*<<  " data: " << str*/ << " thread_id: " << GetCurrentThreadId() << std::endl;
    }

    void EventHandler(const int nSockIndex, const NF_NET_EVENT e, const NFGUID& xClientID, const int nServerID)
    {
        std::cout << " fd: " << nSockIndex << " event_id: " << e << " thread_id: " << std::this_thread::get_id() << std::endl;
    }

    void Execute()
    {
        pNet->Execute();
    }

protected:
    NFINet* pNet;
    int nSendMsgCount;
    int nReciveMsgCount;
};

int main(int argc, char** argv)
{
    TestServerClass x;

    while (1)
    {
        x.Execute();
    }

    return 0;
}
