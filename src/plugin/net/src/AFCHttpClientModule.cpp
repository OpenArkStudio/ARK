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

#include <chrono>
#include <deque>
#include <mutex>
#include <unordered_map>

#include <arknet/http/request.hpp>

#include "net/include/AFCHttpClientModule.hpp"

namespace ark {

namespace {

using HttpResult = std::pair<bool, std::string>;
using HttpCallback = std::function<void(HttpResult)>;

std::string EncodeParameter(const std::string& text)
{
    constexpr char hex[] = "0123456789ABCDEF";
    std::string encoded;
    for (unsigned char character : text)
    {
        if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
            (character >= '0' && character <= '9') || character == '-' || character == '_' || character == '.' ||
            character == '~')
        {
            encoded += static_cast<char>(character);
        }
        else
        {
            encoded += '%';
            encoded += hex[character >> 4];
            encoded += hex[character & 15];
        }
    }
    return encoded;
}

arknet::http_request::request_type MakeRequest(arknet::http::verb method, const std::string& target,
    const std::map<std::string, std::string>* params, const std::vector<std::string>* cookies, const std::string* body,
    const std::string* content_type)
{
    std::string url = target.empty() ? "/" : target;
    if (params)
    {
        for (const auto& [key, value] : *params)
        {
            if (url.back() != '?' && url.back() != '&')
            {
                url += url.find('?') == std::string::npos ? '?' : '&';
            }
            url += EncodeParameter(key) + "=" + EncodeParameter(value);
        }
    }
    arknet::http_request::request_type request(method, url, 11);
    request.set(arknet::http::field::user_agent, "ARK");
    if (cookies)
    {
        std::string cookie;
        for (const auto& value : *cookies)
        {
            if (!cookie.empty())
            {
                cookie += "; ";
            }
            cookie += value;
        }
        request.set(arknet::http::field::cookie, cookie);
    }
    if (body)
    {
        request.body() = *body;
    }
    if (content_type)
    {
        request.set(arknet::http::field::content_type, *content_type);
    }
    return request;
}

HttpResult MakeResult(arknet::error_code error, arknet::http_request::response_type response)
{
    if (error)
    {
        return {false, error.message()};
    }
    if (response.result() != arknet::http::status::ok)
    {
        return {false, std::string(response.reason())};
    }
    return {true, std::move(response.body())};
}

} // namespace

struct AFCHttpClientModule::State : std::enable_shared_from_this<State>
{
    struct Callback
    {
        HttpCallback function;
        HttpResult result;
    };

    asio::io_context context;
    asio::executor_work_guard<asio::io_context::executor_type> work{asio::make_work_guard(context)};
    std::mutex mutex;
    std::unordered_map<std::uint64_t, std::shared_ptr<arknet::http_request>> requests;
    std::deque<Callback> finished;
    std::uint64_t next_request = 0;
    bool accepting = true;

    void Submit(arknet::http::verb method, const std::string& host, const std::string& service,
        const std::string& target, const std::map<std::string, std::string>* params,
        const std::vector<std::string>* cookies, const std::string* body, const std::string* content_type,
        int io_timeout, int connect_timeout, HttpCallback completion)
    {
        auto message = MakeRequest(method, target, params, cookies, body, content_type);
        arknet::http_request_options options;
        options.io_timeout = std::chrono::seconds(io_timeout);
        options.connect_timeout = std::chrono::seconds(connect_timeout);
        std::unique_lock<std::mutex> lock(mutex);
        if (!accepting)
        {
            lock.unlock();
            completion(MakeResult(asio::error::operation_aborted, {}));
            return;
        }
        auto operation = std::make_shared<arknet::http_request>(context);
        const auto id = ++next_request;
        requests.emplace(id, operation);
        operation->async_request(host, service, std::move(message), options,
            [self = shared_from_this(), id, completion = std::move(completion)](
                arknet::error_code error, arknet::http_request::response_type response) mutable {
                {
                    std::lock_guard<std::mutex> completed_lock(self->mutex);
                    self->requests.erase(id);
                }
                completion(MakeResult(error, std::move(response)));
            });
    }

    void Queue(HttpCallback callback, HttpResult result)
    {
        std::lock_guard<std::mutex> lock(mutex);
        finished.push_back({std::move(callback), std::move(result)});
    }

    void Stop()
    {
        std::lock_guard<std::mutex> lock(mutex);
        accepting = false;
        for (const auto& [id, operation] : requests)
        {
            operation->cancel();
        }
        work.reset();
    }
};

AFCHttpClientModule::AFCHttpClientModule()
    : state_(std::make_shared<State>())
{
    worker_ = std::thread([state = state_] { state->context.run(); });
}

AFCHttpClientModule::~AFCHttpClientModule()
{
    PreShut();
}

bool AFCHttpClientModule::PreShut()
{
    state_->Stop();
    if (worker_.joinable())
    {
        worker_.join();
    }
    return true;
}

bool AFCHttpClientModule::Update()
{
    std::deque<State::Callback> callbacks;
    {
        std::lock_guard<std::mutex> lock(state_->mutex);
        callbacks.swap(state_->finished);
    }
    for (auto& callback : callbacks)
    {
        if (callback.function)
        {
            callback.function(std::move(callback.result));
        }
    }
    return true;
}

std::future<HttpResult> AFCHttpClientModule::asyncRequest(arknet::http::verb method, const std::string& host,
    const std::string& service, const std::string& target, const std::map<std::string, std::string>* params,
    const std::vector<std::string>* cookies, const std::string* body, const std::string* content_type, int ioTimeout,
    int connectTimeout)
{
    auto promise = std::make_shared<std::promise<HttpResult>>();
    auto future = promise->get_future();
    state_->Submit(method, host, service, target, params, cookies, body, content_type, ioTimeout, connectTimeout,
        [promise = std::move(promise)](HttpResult result) { promise->set_value(std::move(result)); });
    return future;
}

void AFCHttpClientModule::asyncRequest(HttpCallback callback, arknet::http::verb method, const std::string& host,
    const std::string& service, const std::string& target, const std::map<std::string, std::string>* params,
    const std::vector<std::string>* cookies, const std::string* body, const std::string* content_type, int ioTimeout,
    int connectTimeout)
{
    state_->Submit(method, host, service, target, params, cookies, body, content_type, ioTimeout, connectTimeout,
        [state = state_, callback = std::move(callback)](
            HttpResult result) mutable { state->Queue(std::move(callback), std::move(result)); });
}

} // namespace ark
