/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include "AFSocketFunc.hpp"
#include "AFMacros.hpp"

#if ARK_PLATFORM == PLATFORM_WIN
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma  comment(lib,"Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

namespace ark
{

    char const* proto_type_s[] =
    {
        "unkown",
        "tcp",
        "udp",
        "http",
        "https",
        "ws",
        "wss",
        "zk",
    };


    proto_type str2proto(std::string const& s)
    {
        static int n = ARRAY_LENTGH(proto_type_s);
        for (int i = 0; i < n; ++i)
        {
            if (ARK_STRICMP(s.c_str(), proto_type_s[i]) == 0)
            {
                return proto_type(i);
            }
        }

        return proto_type::unkown;
    }

    std::string proto2str(proto_type proto)
    {
        static int n = ARRAY_LENTGH(proto_type_s);
        if ((int)proto >= n)
        {
            return proto_type_s[0];
        }

        return proto_type_s[(int)proto];
    }
#if ARK_PLATFORM == PLATFORM_WIN
    bool g_WinSockIsInit = false;
#endif
    bool InitSocket()
    {
        bool ret = true;
#if ARK_PLATFORM == PLATFORM_WIN
        static WSADATA g_WSAData;
        if (g_WinSockIsInit)
        {
            return true;
        }

        if (WSAStartup(MAKEWORD(2, 2), &g_WSAData) == 0)
        {
            g_WinSockIsInit = true;
        }
        else
        {
            ret = false;
        }
#else
        signal(SIGPIPE, SIG_IGN);
#endif

        return ret;
    }

    void ShutSocket(void)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        g_WinSockIsInit = false;
        WSACleanup();
#endif
    }

    bool IsDomain(const std::string& host)
    {
        return (inet_addr(host.c_str()) != INADDR_NONE);
    }

    std::string MakeUrl(const std::string& protocol, const std::string& hostname, const uint16_t& port)
    {
        std::string domain = protocol + "://" + hostname + ":" + ARK_TO_STRING(port);
        return domain;
    }

#if ARK_PLATFORM == PLATFORM_WIN
    bool GetLocalIP(char* ip)
    {
        //1.获取主机名
        char hostname[256];
        int ret = gethostname(hostname, sizeof(hostname));
        if (ret == SOCKET_ERROR)
        {
            return false;
        }
        //2.获取主机ip
        HOSTENT* host = gethostbyname(hostname);
        if (host == nullptr)
        {
            return false;
        }
        //3.转化为char*并拷贝返回
        strcpy(ip, inet_ntoa(*(in_addr*)*host->h_addr_list));
        return true;
    }

#endif

    bool is_ipv4_address(const string& ip)
    {
        struct sockaddr_in sa;
        return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 0;
    }

    bool is_ipv6_address(const string& ip)
    {
        struct sockaddr_in6 sa;
        return inet_pton(AF_INET6, ip.c_str(), &(sa.sin6_addr)) != 0;
    }

    bool GetHost(const std::string& host, bool& is_ip_v6, std::string& ip)
    {
        bool ret = InitSocket();
        if (!ret)
        {
            return false;
        }

#if ARK_PLATFORM == PLATFORM_WIN
        static char local_ip[256] = { 0 };
        memset(local_ip, 0x00, sizeof(local_ip));
        ret = GetLocalIP(local_ip);
        if (ret)
        {
            ip = local_ip;
            if (is_ipv4_address(ip))
            {
                is_ip_v6 = false;
            }
            else
            {
                is_ip_v6 = true;
            }
        }
#else
        struct addrinfo hints, *answer, *curr;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_CANONNAME;
        hints.ai_protocol = 0;  /* any protocol */

        char ip_v4[16] = { 0 };
        char ip_v6[128] = { 0 };
        int result = getaddrinfo(host.c_str(), NULL, &hints, &answer);
        if (result != 0)
        {
            return false;
        }

        for (curr = answer; curr != NULL; curr = curr->ai_next)
        {
            int pf_family = answer->ai_family;
            switch (answer->ai_family)
            {
            case PF_INET:
                {
                    is_ip_v6 = false;
                    inet_ntop(AF_INET, &(((struct sockaddr_in*)(curr->ai_addr))->sin_addr), ip_v4, sizeof(ip_v4));
                    ip = ip_v4;
                }
                break;
            case PF_INET6:
                {
                    is_ip_v6 = true;
                    inet_ntop(AF_INET6, &(((struct sockaddr_in6*)(curr->ai_addr))->sin6_addr), ip_v6, sizeof(ip_v6));
                    ip = ip_v6;
                }
                break;
            default:
                break;
            }
        }

        freeaddrinfo(answer);
#endif

        ShutSocket();
        return ret;
    }

    AFEndpoint AFEndpoint::from_string(std::string const& url, std::error_code& ec)
    {
        if (url.empty())
        {
            ec = make_error_code(-1);
            return AFEndpoint();
        }

        static std::regex re("((.*)://)?([^:/]+)(:(\\d+))?(/.*)?");
        std::smatch result;
        bool ok = std::regex_match(url, result, re);
        if (!ok)
        {
            ec = make_error_code(-2);
            return AFEndpoint();
        }

        std::string host(result[3].str());
        bool is_ipv6{ false };
        std::string ip{};
        if (!GetHost(host, is_ipv6, ip))
        {
            ec = make_error_code(-3);
            return AFEndpoint();
        }

        AFEndpoint ep;
        ep.set_ip(ip);
        ep.set_port(atoi(result[5].str().c_str()));
        ep.set_proto(str2proto(result[2].str()));
        ep.set_path(result[6].str());
        ep.set_is_v6(is_ipv6);

        return ep;
    }

    std::string AFEndpoint::to_string() const
    {
        std::string url;
        if (proto() != proto_type::unkown)
        {
            url += proto2str(proto()) + "://";
        }

        url += ip() + ":" + ARK_TO_STRING(port()) + path();
        return url;
    }

}