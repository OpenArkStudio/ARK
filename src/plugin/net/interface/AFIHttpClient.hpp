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

#include <ananas/future/Future.h>
#include <brynet/net/http/HttpFormat.hpp>

#include "base/AFPlatform.hpp"
// Protobuf header files below platform.hpp cuz the std::min and std::numberic_limit<T>::min;
#include <google/protobuf/message.h>

namespace ark {

// http client without DNS resolver
class AFIHttpClient
{
public:
    virtual ~AFIHttpClient() = default;
    virtual bool Update() = 0;

    // The final request function.
    virtual ananas::Future<std::pair<bool, std::string>> AsyncRequest(
        brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
        const std::string& url, std::map<std::string, std::string>& params, const std::vector<std::string>& cookies,
        const std::string& http_doby) = 0;
};

} // namespace ark
