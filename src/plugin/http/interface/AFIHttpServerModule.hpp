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

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

#include <arknet/http/router.hpp>

#include "interface/AFIModule.hpp"

namespace ark {

class AFIHttpServerModule : public AFIModule
{
public:
    virtual arknet::http_router& GetRouter() = 0;
    virtual bool StartServer() = 0;
    virtual bool StartServer(std::string const& ip = "0.0.0.0", uint16_t port = 8080, uint8_t thread_num = 1) = 0;

    template<typename ResponseBody, class RequestBody>
    arknet::http::response<ResponseBody> make_200(const arknet::http::request<RequestBody>& request,
        typename ResponseBody::value_type body, std::string_view content_type)
    {
        arknet::http::response<ResponseBody> response{arknet::http::status::ok, request.version()};
        response.set(arknet::http::field::server, "ARK");
        response.set(arknet::http::field::content_type, content_type);
        response.body() = std::move(body);
        response.prepare_payload();
        response.keep_alive(request.keep_alive());

        return response;
    }
};

} // namespace ark
