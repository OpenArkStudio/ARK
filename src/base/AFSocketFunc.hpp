/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#pragma once

#include "base/AFPlatform.hpp"
#include "base/AFMacros.hpp"
#include "AFError.hpp"

#if ARK_PLATFORM == PLATFORM_WIN
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

namespace ark
{
#if ARK_PLATFORM == PLATFORM_WIN
static std::atomic_bool g_WinSockIsInit = false;
#endif

enum class proto_type
{
    unkown,
    tcp,
    udp,
    http,
    https,
    ws,
    wss,
    zk,
};

class AFSocket
{
public:
    static bool InitSocket()
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

    static void ShutSocket()
    {
#if ARK_PLATFORM == PLATFORM_WIN
        g_WinSockIsInit = false;
        WSACleanup();
#endif
    }

    static bool IsDomain(const std::string &host)
    {
        return (inet_addr(host.c_str()) != INADDR_NONE);
    }

    static std::string MakeUrl(const std::string &protocol, const std::string &hostname, const uint16_t &port)
    {
        std::string domain = protocol + "://" + hostname + ":" + ARK_TO_STRING(port);
        return domain;
    }

    // hton ntoh functions
    static int64_t HTONLL(const int64_t data)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        return htonll(data);
#else
        return htobe64(data);
#endif
    }

    static int64_t NTOHLL(const int64_t data)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        return ntohll(data);
#else
        return be64toh(data);
#endif
    }

    static int32_t HTONL(const int32_t data)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        return htonl(data);
#else
        return htobe32(data);
#endif
    }

    static int32_t NTOHL(const int32_t data)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        return ntohl(data);
#else
        return be32toh(data);
#endif
    }

    static int16_t HTONS(const int16_t data)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        return htons(data);
#else
        return htobe16(data);
#endif
    }

    static int16_t NTOHS(const int16_t data)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        return ntohs(data);
#else
        return be16toh(data);
#endif
    }

    static proto_type str2proto(std::string const &s)
    {
        static char const *proto_type_s[] = {
            "unkown",
            "tcp",
            "udp",
            "http",
            "https",
            "ws",
            "wss",
            "zk",
        };

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

    static std::string proto2str(proto_type proto)
    {
        static char const *proto_type_s[] = {
            "unkown",
            "tcp",
            "udp",
            "http",
            "https",
            "ws",
            "wss",
            "zk",
        };

        static int n = ARRAY_LENTGH(proto_type_s);
        if ((int)proto >= n)
        {
            return proto_type_s[0];
        }

        return proto_type_s[(int)proto];
    }

#if ARK_PLATFORM == PLATFORM_WIN
    static bool GetLocalIP(char *ip)
    {
        // 1.get host name
        char hostname[256] = {0};
        int ret = gethostname(hostname, sizeof(hostname));
        if (ret == SOCKET_ERROR)
        {
            return false;
        }
        // 2.get host ip
        HOSTENT *host = gethostbyname(hostname);
        if (host == nullptr)
        {
            return false;
        }
        // 3.transfer to char pointer
        strcpy(ip, inet_ntoa(*(in_addr *)*host->h_addr_list));
        return true;
    }
#endif

    static bool IsIPV4Address(const string &ip)
    {
        struct sockaddr_in sa;
        return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 0;
    }

    static bool IsIPV6Address(const string &ip)
    {
        struct sockaddr_in6 sa;
        return inet_pton(AF_INET6, ip.c_str(), &(sa.sin6_addr)) != 0;
    }

    static bool GetHost(const std::string &host, bool &is_ip_v6, std::string &ip)
    {
        bool ret = InitSocket();
        if (!ret)
        {
            return false;
        }

#if ARK_PLATFORM == PLATFORM_WIN
        static char local_ip[256] = {0};
        memset(local_ip, 0x00, sizeof(local_ip));
        ret = GetLocalIP(local_ip);
        if (ret)
        {
            ip = local_ip;
            if (IsIPV4Address(ip))
            {
                is_ip_v6 = false;
            }
            else
            {
                is_ip_v6 = true;
            }
        }
#else
        struct addrinfo hints;
        struct addrinfo *answer;
        struct addrinfo *curr;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_CANONNAME;
        hints.ai_protocol = 0; /* any protocol */

        char ip_v4[16] = {0};
        char ip_v6[128] = {0};
        int result = getaddrinfo(host.c_str(), NULL, &hints, &answer);
        if (result != 0)
        {
            return false;
        }

        for (curr = answer; curr != NULL; curr = curr->ai_next)
        {
            int pf_family = answer->ai_family;
            switch (pf_family)
            {
            case PF_INET:
            {
                is_ip_v6 = false;
                inet_ntop(AF_INET, &(((struct sockaddr_in *)(curr->ai_addr))->sin_addr), ip_v4, sizeof(ip_v4));
                ip = ip_v4;
            }
            break;
            case PF_INET6:
            {
                is_ip_v6 = true;
                inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)(curr->ai_addr))->sin6_addr), ip_v6, sizeof(ip_v6));
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
};

class AFEndpoint
{
public:
    AFEndpoint() = default;

    struct ext_t
    {
        proto_type proto_;
        std::string ip_;
        uint16_t port_;
        std::string path_;
    };

    proto_type proto() const
    {
        return ext_.proto_;
    }

    void SetProtocol(proto_type proto)
    {
        ext_.proto_ = proto;
    }

    std::string const &GetIP() const
    {
        return ext_.ip_;
    }

    void SetIP(std::string const &ip)
    {
        ext_.ip_ = ip;
    }

    std::string const &GetPath() const
    {
        return ext_.path_;
    }

    void SetPath(std::string const &path)
    {
        ext_.path_ = path;
    }

    uint16_t GetPort() const
    {
        return ext_.port_;
    }

    void SetPort(uint16_t const &port)
    {
        ext_.port_ = port;
    }

    bool IsV6() const
    {
        return is_ipv6_;
    }

    void SetIsV6(bool v6)
    {
        is_ipv6_ = v6;
    }

    static AFEndpoint FromString(std::string const &url, std::error_code &ec)
    {
        if (url.empty())
        {
            ec = AFErrorCategory::MakeErrorCode(-1);
            return AFEndpoint();
        }

        static std::regex re("((.*)://)?([^:/]+)(:(\\d+))?(/.*)?");
        std::smatch result;
        bool ok = std::regex_match(url, result, re);
        if (!ok)
        {
            ec = AFErrorCategory::MakeErrorCode(-2);
            return AFEndpoint();
        }

        std::string host(result[3].str());
        bool is_ipv6{false};
        std::string ip{};
        if (!AFSocket::GetHost(host, is_ipv6, ip))
        {
            ec = AFErrorCategory::MakeErrorCode(-3);
            return AFEndpoint();
        }

        AFEndpoint ep;
        ep.SetIP(ip);
        ep.SetPort(atoi(result[5].str().c_str()));
        ep.SetProtocol(AFSocket::str2proto(result[2].str()));
        ep.SetPath(result[6].str());
        ep.SetIsV6(is_ipv6);

        return ep;
    }

    std::string ToString() const
    {
        std::string url;
        if (proto() != proto_type::unkown)
        {
            url += AFSocket::proto2str(proto()) + "://";
        }

        url += GetIP() + ":" + ARK_TO_STRING(GetPort()) + GetPath();
        return url;
    }

private:
    ext_t ext_;
    bool is_ipv6_{false};
};

} // namespace ark
