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

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>

#include <http/reactor/listener.hxx>
#include <http/reactor/session.hxx>
#include <http/basic_router.hxx>
#include <http/out.hxx>

#include "interface/AFIModule.hpp"

namespace ark {

namespace beast = boost::beast;

using namespace _0xdead4ead;

class AFIHttpServerModule : public AFIModule
{
public:
    using http_session = http::reactor::_default::session_type;
    using http_listener = http::reactor::_default::listener_type;
    using http_context = typename http_session::context_type;
    using beast_http_request = typename http_session::request_type;
    using asio_socket = typename http_session::socket_type;

    virtual http::basic_router<http_session>& GetRouter() = 0;
    virtual bool StartServer() = 0;
    virtual bool StartServer(std::string const& ip = "0.0.0.0", uint16_t port = 8080, uint8_t thread_num = 1) = 0;

    template<typename ResponseBody, class RequestBody>
    beast::http::response<ResponseBody> make_200(const beast::http::request<RequestBody>& request,
        typename ResponseBody::value_type body, beast::string_view content_type)
    {
        beast::http::response<ResponseBody> response{beast::http::status::ok, request.version()};
        response.set(beast::http::field::server, "FreeBird/1.0");
        response.set(beast::http::field::content_type, content_type);
        response.body() = body;
        response.prepare_payload();
        response.keep_alive(request.keep_alive());

        return response;
    }
};

} // namespace ark