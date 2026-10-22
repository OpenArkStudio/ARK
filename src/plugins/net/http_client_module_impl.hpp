/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include <memory>

#include "ark/net/http_client_module.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

class http_client_module_impl final : public http_client_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    http_client_module_impl();
    ~http_client_module_impl() override;

    bool update() override;
    bool pre_shut() override;

    std::future<std::pair<bool, std::string>> async_request(arknet::http::verb method, const std::string& host,
        const uint16_t port, const std::string& url, const std::map<std::string, std::string>* url_params = nullptr,
        const std::vector<std::string>* cookies = nullptr, const std::string* http_body = nullptr,
        const std::string* content_type = nullptr, int io_timeout = 20, int connect_timeout = 5) override {
        return async_request(method, host, std::to_string(port), url, url_params, cookies, http_body, content_type,
            io_timeout, connect_timeout);
    }

    std::future<std::pair<bool, std::string>> get(const std::string& host, const uint16_t port, const std::string& url,
        const std::map<std::string, std::string>* url_params = nullptr,
        const std::vector<std::string>* cookies = nullptr, int io_timeout = 20, int connect_timeout = 5) override {
        return async_request(arknet::http::verb::get, host, std::to_string(port), url, url_params, cookies, nullptr,
            nullptr, io_timeout, connect_timeout);
    }

    std::future<std::pair<bool, std::string>> post(const std::string& host, const uint16_t port, const std::string& url,
        const std::map<std::string, std::string>* url_params = nullptr, const std::string* http_body = nullptr,
        const std::string* content_type = nullptr, const std::vector<std::string>* cookies = nullptr,
        int io_timeout = 20, int connect_timeout = 5) override {
        return async_request(arknet::http::verb::post, host, std::to_string(port), url, url_params, cookies, http_body,
            content_type, io_timeout, connect_timeout);
    }

    void post(std::function<void(std::pair<bool, std::string>)> cb, const std::string& host, const uint16_t port,
        const std::string& url, const std::map<std::string, std::string>* url_params = nullptr,
        const std::string* http_body = nullptr, const std::string* content_type = nullptr,
        const std::vector<std::string>* cookies = nullptr, int io_timeout = 20, int connect_timeout = 5) override {
        async_request(std::move(cb), arknet::http::verb::post, host, std::to_string(port), url, url_params, cookies,
            http_body, content_type, io_timeout, connect_timeout);
    }

    void post(std::function<void(std::pair<bool, std::string>)> cb, const std::string& host, const std::string& service,
        const std::string& url, const std::map<std::string, std::string>* url_params = nullptr,
        const std::string* http_body = nullptr, const std::string* content_type = nullptr,
        const std::vector<std::string>* cookies = nullptr, int io_timeout = 20, int connect_timeout = 5) override {
        async_request(std::move(cb), arknet::http::verb::post, host, service, url, url_params, cookies, http_body,
            content_type, io_timeout, connect_timeout);
    }

private:
    std::future<std::pair<bool, std::string>> async_request(arknet::http::verb method, const std::string& host,
        const std::string& service, const std::string& target,
        const std::map<std::string, std::string>* params = nullptr, const std::vector<std::string>* cookies = nullptr,
        const std::string* http_body = nullptr, const std::string* content_type = nullptr, int io_timeout = 20,
        int connect_timeout = 5);

    void async_request(std::function<void(std::pair<bool, std::string>)> cb, arknet::http::verb method,
        const std::string& host, const std::string& service, const std::string& target,
        const std::map<std::string, std::string>* params = nullptr, const std::vector<std::string>* cookies = nullptr,
        const std::string* http_body = nullptr, const std::string* content_type = nullptr, int io_timeout = 20,
        int connect_timeout = 5);

private:
    struct state;
    std::shared_ptr<state> state_;
};

} // namespace ark
