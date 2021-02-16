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

#include "base/AFPluginManager.hpp"
#include "base/container/AFLockFreeQueue.hpp"
#include "net/interface/AFIHttpClientModule.hpp"

namespace ark {

class AFCHttpClientModule final : public AFIHttpClientModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit AFCHttpClientModule();

    ~AFCHttpClientModule()
    {
        ioc_.stop();
    }

    bool Update() override
    {
        std::shared_ptr<Callback> cb;
        while (finishedRequests_.Pop(cb))
        {
            cb->Cb(cb->Arg);
        }
        return true;
    }

    std::future<std::pair<bool, std::string>> Get(const std::string& host, const uint16_t port, const std::string& url,
        const std::map<std::string, std::string>* url_params = nullptr,
        const std::vector<std::string>* cookies = nullptr, int ioTimeout = 20, int connectTimeout = 5) override
    {
        return asyncRequest(zephyr::http::verb::get, host, std::to_string(port),
                            url, url_params, cookies, nullptr, nullptr, ioTimeout, connectTimeout);
    }

    std::future<std::pair<bool, std::string>> Post(const std::string& host, const uint16_t port, const std::string& url,
        const std::map<std::string, std::string>* url_params = nullptr, const std::string* http_body = nullptr,
        const std::string* content_type = nullptr, const std::vector<std::string>* cookies = nullptr,
        int ioTimeout = 20, int connectTimeout = 5) override
    {
        return asyncRequest(zephyr::http::verb::post, host, std::to_string(port),
                            url, url_params, cookies, http_body, content_type, ioTimeout, connectTimeout);
    }

    void Post(std::function<void(std::pair<bool, std::string>)> cb, const std::string& host, const uint16_t port,
              const std::string& url, const std::map<std::string, std::string>* url_params = nullptr,
              const std::string* http_body = nullptr, const std::string* content_type = nullptr,
              const std::vector<std::string>* cookies = nullptr, int ioTimeout = 20, int connectTimeout = 5) override
    {
        asyncRequest(std::move(cb),zephyr::http::verb::post, host, std::to_string(port), url, url_params,
                     cookies, http_body, content_type, ioTimeout, connectTimeout);
    }

    void Post(std::function<void(std::pair<bool, std::string>)> cb, const std::string& host,
        const std::string& service, const std::string& url, const std::map<std::string, std::string>* url_params = nullptr,
        const std::string* http_body = nullptr, const std::string* content_type = nullptr,
        const std::vector<std::string>* cookies = nullptr, int ioTimeout = 20, int connectTimeout = 5) override
    {
        asyncRequest(std::move(cb),zephyr::http::verb::post, host, service, url, url_params,
                     cookies, http_body, content_type, ioTimeout, connectTimeout);
    }

private:
    std::future<std::pair<bool, std::string>> asyncRequest(zephyr::http::verb method, const std::string& host,
        const std::string& service, const std::string& target, const std::map<std::string, std::string>* params = nullptr,
        const std::vector<std::string>* cookies = nullptr, const std::string* http_body = nullptr,
        const std::string* content_type = nullptr, int ioTimeout = 20, int connectTimeout = 5);

    void asyncRequest(std::function<void(std::pair<bool, std::string>)> cb, zephyr::http::verb method, const std::string& host,
                      const std::string& service, const std::string& target, const std::map<std::string, std::string>* params = nullptr,
                      const std::vector<std::string>* cookies = nullptr, const std::string* http_body = nullptr,
                      const std::string* content_type = nullptr, int ioTimeout = 20, int connectTimeout = 5);

private:
    struct Callback {
        Callback(std::function<void(std::pair<bool, std::string>)> cb, std::pair<bool, std::string> arg)
            : Cb(std::move(cb)), Arg(move(arg))
        {
        }

        std::function<void(std::pair<bool, std::string>)> Cb;
        std::pair<bool, std::string> Arg;
    };

private:
    boost::asio::io_context ioc_;
    AFLockFreeQueue<std::shared_ptr<Callback>> finishedRequests_;
};

} // namespace ark
