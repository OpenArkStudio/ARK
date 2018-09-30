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

#pragma once

#include "AFPlatform.hpp"
#include "AFMacros.hpp"
#include "AFError.hpp"

namespace ark
{

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

    bool InitSocket();
    void ShutSocket();

    bool IsDomain(const std::string& host);

    std::string MakeUrl(const std::string& protocol, const std::string& hostname, const uint16_t& port);

#if ARK_PLATFORM == PLATFORM_WIN
    bool GetLocalIP(char* ip);
#endif

    bool is_ipv4_address(const string& str);
    bool is_ipv6_address(const string& str);
    bool GetHost(const std::string& host, bool& is_ip_v6, std::string& ip);


    struct AFEndpoint
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

        void set_proto(proto_type proto)
        {
            ext_.proto_ = proto;
        }

        std::string const& ip() const
        {
            return ext_.ip_;
        }

        void set_ip(std::string const& ip)
        {
            ext_.ip_ = ip;
        }

        std::string const& path() const
        {
            return ext_.path_;
        }

        void set_path(std::string const& path)
        {
            ext_.path_ = path;
        }

        uint16_t port() const
        {
            return ext_.port_;
        }

        void set_port(uint16_t const& port)
        {
            ext_.port_ = port;
        }

        bool is_v6() const
        {
            return is_ipv6_;
        }

        void set_is_v6(bool v6)
        {
            is_ipv6_ = v6;
        }

        static AFEndpoint from_string(std::string const& url, std::error_code& ec);
        std::string to_string() const;

    private:
        ext_t ext_;
        bool is_ipv6_{ false };
    };

}
