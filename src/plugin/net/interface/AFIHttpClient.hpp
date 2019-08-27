/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
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
#include "proto/AFProtoCPP.hpp"

namespace ark {

// http client without DNS resolver
class AFIHttpClient
{
public:
    using HTTP_CALLBACK = std::function<void(const std::string&)>;
    using HTTP_CALLBACK = std::function<void(const std::string&)>;

    template<typename BaseType>
    void AsyncPost(const std::string& ip, const uint16_t port, const std::string& url,
        std::map<std::string, std::string>& params, const std::string& post_data, BaseType* base_ptr,
        void (BaseType::*handler)(const std::string&))
    {
        auto functor = std::bind(handler, base_ptr, std::placeholders::_1);
        AsyncPost(ip, port, url, params, post_data, std::move(functor));
    }

    //// BaseType - the this pointer of function's class
    //// Response - response protobuf message
    //template<typename BaseType, typename Response>
    //void AsyncPost(const std::string& ip, const uint16_t port, const std::string& url,
    //    std::map<std::string, std::string>& params, const google::protobuf::Message& post_msg, BaseType* base_ptr,
    //    void (BaseType::*handler)(const Response& msg))
    //{
    //    auto functor = std::bind(handler, base_ptr, std::placeholders::_1);
    //    AsyncPost(ip, port, url, params, post_msg, std::move(functor));
    //}

    template<typename BaseType>
    void AsyncGet(const std::string& ip, const uint16_t port, const std::string& url,
        std::map<std::string, std::string>& params, BaseType* base_ptr, void (BaseType::*handler)(const std::string&))
    {
        auto functor = std::bind(handler, base_ptr, std::placeholders::_1);
        AsyncGet(ip, port, url, params, post_data, std::move(functor));
    }

    //template<typename BaseType, typename Response>
    //void AsyncGet(const std::string& ip, const uint16_t port, const std::string& url,
    //    std::map<std::string, std::string>& params, BaseType* base_ptr, void (BaseType::*handler)(const Response&))
    //{
    //    auto functor = std::bind(handler, base_ptr, std::placeholders::_1);
    //    AsyncGet(ip, port, url, params, post_data, std::move(functor));
    //}

protected:
    virtual void AsyncPost(const std::string& ip, const uint16_t port, const std::string& url,
        std::map<std::string, std::string>& params, const std::string& post_data, HTTP_CALLBACK&& callback) = 0;
    virtual void AsyncGet(const std::string& ip, const uint16_t port, const std::string& url,
        std::map<std::string, std::string>& params, HTTP_CALLBACK&& callback) = 0;
};

} // namespace ark
