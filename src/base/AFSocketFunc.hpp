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
#include "base/AFError.hpp"

#ifdef ARK_PLATFORM_WIN
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

namespace ark {
#ifdef ARK_PLATFORM_WIN
static std::atomic_bool g_WinSockIsInit = false;
#endif

enum class proto_type
{
    unknown,
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
#ifdef ARK_PLATFORM_WIN
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
#ifdef ARK_PLATFORM_WIN
        g_WinSockIsInit = false;
        WSACleanup();
#endif
    }

    static bool IsDomain(const std::string& host)
    {
        return (inet_addr(host.c_str()) != INADDR_NONE);
    }

    static std::string MakeUrl(const std::string& protocol, const std::string& hostname, const uint16_t& port)
    {
        std::string domain = protocol + "://" + hostname + ":" + ARK_TO_STRING(port);
        return domain;
    }

    // hton ntoh functions
    static int64_t ARK_HTONLL(const int64_t data)
    {
#if defined(ARK_PLATFORM_WIN) || defined(ARK_PLATFORM_DARWIN)
        return htonll(data);
#else
        return htobe64(data);
#endif
    }

    static int64_t ARK_NTOHLL(const int64_t data)
    {
#if defined(ARK_PLATFORM_WIN) || defined(ARK_PLATFORM_DARWIN)
        return ntohll(data);
#else
        return be64toh(data);
#endif
    }

    static int32_t ARK_HTONL(const int32_t data)
    {
#if defined(ARK_PLATFORM_WIN) || defined(ARK_PLATFORM_DARWIN)
        return htonl(data);
#else
        return htobe32(data);
#endif
    }

    static int32_t ARK_NTOHL(const int32_t data)
    {
#if defined(ARK_PLATFORM_WIN) || defined(ARK_PLATFORM_DARWIN)
        return ntohl(data);
#else
        return be32toh(data);
#endif
    }

    static int16_t ARK_HTONS(const int16_t data)
    {
#if defined(ARK_PLATFORM_WIN) || defined(ARK_PLATFORM_DARWIN)
        return htons(data);
#else
        return htobe16(data);
#endif
    }

    static int16_t ARK_NTOHS(const int16_t data)
    {
#if defined(ARK_PLATFORM_WIN) || defined(ARK_PLATFORM_DARWIN)
        return ntohs(data);
#else
        return be16toh(data);
#endif
    }

    static proto_type str2proto(std::string const& s)
    {
        static char const* proto_type_s[] = {
            "unknown",
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

        return proto_type::unknown;
    }

    static std::string proto2str(proto_type proto)
    {
        static char const* proto_type_s[] = {
            "unknown",
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

#ifdef ARK_PLATFORM_WIN
    static bool GetIntranetIP(char* ip)
    {
        // 1.get host name
        char hostname[256] = {0};
        int ret = gethostname(hostname, sizeof(hostname));
        if (ret == SOCKET_ERROR)
        {
            return false;
        }
        // 2.get host ip
        HOSTENT* host = gethostbyname(hostname);
        if (host == nullptr)
        {
            return false;
        }
        // 3.transfer to char pointer
        strcpy(ip, inet_ntoa(*(in_addr*)*host->h_addr_list));
        return true;
    }
#endif

    static bool IsIPV4Address(const string& ip)
    {
        struct sockaddr_in sa;
        return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 0;
    }

    static bool IsIPV6Address(const string& ip)
    {
        struct sockaddr_in6 sa;
        return inet_pton(AF_INET6, ip.c_str(), &(sa.sin6_addr)) != 0;
    }

    static bool GetHost(bool need_intranet_ip, const std::string& host, bool& is_ip_v6, std::string& ip)
    {
        bool ret = InitSocket();
        if (!ret)
        {
            return false;
        }

#ifdef ARK_PLATFORM_WIN

        if (need_intranet_ip)
        {
            static char intranet_ip[256] = {0};
            memset(intranet_ip, 0x00, sizeof(intranet_ip));
            ret = GetIntranetIP(intranet_ip);
            if (ret)
            {
                ip = intranet_ip;
                if (IsIPV4Address(ip))
                {
                    is_ip_v6 = false;
                }
                else
                {
                    is_ip_v6 = true;
                }
            }
        }
        else
        {
            ip = host;
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
        struct addrinfo* answer;
        struct addrinfo* curr;

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

    std::string const& GetIP() const
    {
        return ext_.ip_;
    }

    void SetIP(std::string const& ip)
    {
        ext_.ip_ = ip;
    }

    std::string const& GetPath() const
    {
        return ext_.path_;
    }

    void SetPath(std::string const& path)
    {
        ext_.path_ = path;
    }

    uint16_t GetPort() const
    {
        return ext_.port_;
    }

    void SetPort(uint16_t const& port)
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

    static AFEndpoint FromString(std::string const& url, std::error_code& ec)
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

        //std::string host(result[3].str());
        //bool need_intranet_ip = (host == "0.0.0.0" || host == "[::]") ? false : true; // check if we need intranet ip

        //bool is_ipv6{false};
        //std::string ip{};
        //if (!AFSocket::GetHost(need_intranet_ip, host, is_ipv6, ip))
        //{
        //    ec = AFErrorCategory::MakeErrorCode(-3);
        //    return AFEndpoint();
        //}

        AFEndpoint ep;
        ep.SetIP(result[3].str());
        ep.SetPort(ARK_LEXICAL_CAST<uint16_t>(result[5].str()));
        ep.SetProtocol(AFSocket::str2proto(result[2].str()));
        ep.SetPath(result[6].str());
        ep.SetIsV6(false);

        return ep;
    }

    std::string ToString() const
    {
        std::string url;
        if (proto() != proto_type::unknown)
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
