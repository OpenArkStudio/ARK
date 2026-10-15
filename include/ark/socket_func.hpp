/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/dep.hpp"
#include "ark/error.hpp"
#include "ark/macros.hpp"
#include "ark/platform.hpp"

#ifdef ARK_PLATFORM_WIN
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

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

class endpoint final
{
public:
    class socket final
    {
    public:
        static std::string make_url(const std::string& protocol, const std::string& hostname, const uint16_t& port) {
            std::string domain = protocol + "://" + hostname + ":" + ARK_TO_STRING(port);
            return domain;
        }

        static proto_type str2proto(std::string const& s) {
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

        static std::string proto2str(proto_type proto) {
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
    };

    struct ext_t
    {
        proto_type proto_;
        std::string ip_;
        uint16_t port_;
        std::string path_;
    };

    proto_type proto() const {
        return ext_.proto_;
    }

    void set_protocol(proto_type proto) {
        ext_.proto_ = proto;
    }

    std::string const& get_ip() const {
        return ext_.ip_;
    }

    void set_ip(std::string const& ip) {
        ext_.ip_ = ip;
    }

    std::string const& get_path() const {
        return ext_.path_;
    }

    void set_path(std::string const& path) {
        ext_.path_ = path;
    }

    uint16_t get_port() const {
        return ext_.port_;
    }

    void set_port(uint16_t const& port) {
        ext_.port_ = port;
    }

    static endpoint from_string(std::string const& url, std::error_code& ec) {
        if (url.empty())
        {
            ec = error_category::make_error_code(-1);
            return endpoint();
        }

        static std::regex re("((.*)://)?([^:/]+)(:(\\d+))?(/.*)?");
        std::smatch result;
        bool ok = std::regex_match(url, result, re);
        if (!ok)
        {
            ec = error_category::make_error_code(-2);
            return endpoint();
        }

        endpoint ep;
        ep.set_ip(result[3].str());
        ep.set_port(ARK_LEXICAL_CAST<uint16_t>(result[5].str()));
        ep.set_protocol(socket::str2proto(result[2].str()));
        ep.set_path(result[6].str());

        return ep;
    }

    std::string to_string() const {
        std::string url;
        if (proto() != proto_type::unknown)
        {
            url += socket::proto2str(proto()) + "://";
        }

        url += get_ip() + ":" + ARK_TO_STRING(get_port()) + get_path();
        return url;
    }

private:
    ext_t ext_;
};

} // namespace ark
