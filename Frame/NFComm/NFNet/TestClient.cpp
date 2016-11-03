
#include "NFCNet.h"
#include <thread>
#include <string>
#include "NFComm/NFCore/NFTimer.h"
#include "NFCMulNet.h"
#pragma comment(lib,"ws2_32.lib")

#ifdef NF_DEBUG_MODE
#pragma comment(lib,"NFNet_d.lib")
#pragma comment(lib,"NFCore_d.lib")
#else
#pragma comment(lib,"NFNet.lib")
#pragma comment(lib,"NFCore.lib")
#endif
class TestClientClass
{
public:
    TestClientClass(const int nID)
    {
        //pNet = new NFCNet(this, &TestClientClass::ReciveHandler, &TestClientClass::EventHandler);
        pNet = new NFCMulNet(this, &TestClientClass::ReciveHandler, &TestClientClass::EventHandler);
        pNet->Initialization("127.0.0.1", 8088, 1);
		bConnected = false;
        nSendMsgCount = 0;
        nReciveMsgCount = 0;
        mnID = nID;
    }

    void ReciveHandler(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
    {
        std::string str;
        str.assign(msg, nLen);
        nReciveMsgCount++;
        std::cout << " mnID: " << mnID << " nSendMsgCount: " << nSendMsgCount << "nReciveMsgCount" << nReciveMsgCount << " fd: " << nSockIndex << " msg_id: " << nMsgID /*<<  " data: " << str */ << " thread_id: " << std::this_thread::get_id() << std::endl;
    };

    void EventHandler(const int nSockIndex, const NF_NET_EVENT e, const NFGUID& xClientID, const int nServerID)
    {
        std::cout << " fd: " << nSockIndex << " event_id: " << e << " thread_id: " << std::this_thread::get_id() << std::endl;
		if(e == NF_NET_EVENT_CONNECTED)
		{
			bConnected = true;
		}
    }

    void Execute()
    {
		if(bConnected)
		{
			pNet->SendMsgWithOutHead(1, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890", 100, 0);
            nSendMsgCount++;
		}

        pNet->Execute();
    }

protected:
    NFINet* pNet;
	bool bConnected;
    int nSendMsgCount;
    int nReciveMsgCount;
    int mnID;
private:
};

int main(int argc, char** argv)
{
    std::list<TestClientClass*> list;

    int nTime = NFTime::GetNowTime();
    for (int i = 0; i < 10; ++i)
    {
        TestClientClass* x = new TestClientClass(i);;
        list.push_back(x);
    }

    while (1)
    {
        std::list<TestClientClass*>::iterator it = list.begin();
        for (it; it != list.end(); ++it)
        {
            Sleep(1);

            (*it)->Execute();
        }

        int nNowTime = NFTime::GetNowTime();
        if (nNowTime - nTime > 300)
        {
            system("pause");
            break;
        }
    }



    return 0;
}
