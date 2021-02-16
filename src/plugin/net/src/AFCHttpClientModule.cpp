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

#include "net/include/AFCHttpClientModule.hpp"
#include <google/protobuf/util/json_util.h>

namespace ark {

using namespace std;

class HttpClientImpl : public enable_shared_from_this<HttpClientImpl>
{
public:
    explicit HttpClientImpl(boost::asio::io_context& ioc, int connectTimeout, int ioTimeout)
        : connectTimeout_(connectTimeout), ioTimeout_(ioTimeout), resolver_(ioc), stream_(ioc)
    {
    }

    // Start the asynchronous operation
    std::future<pair<bool, string>> AsyncRequest(zephyr::http::verb method, const string& host, const string& service,
        const string& target, const std::map<string, string>* params = nullptr, const vector<string>* cookies = nullptr,
        const string* http_body = nullptr, const string* content_type = nullptr)
    {
        initRequest(method, host, target, params, cookies, http_body, content_type);
        // Look up the domain name
        resolver_.async_resolve(
            host, service, boost::beast::bind_front_handler(&HttpClientImpl::onResolve, shared_from_this()));
        return promise_.get_future();
    }

    // Start the asynchronous operation
    void AsyncRequest(function<void(pair<bool, string>)> cb, zephyr::http::verb method, const string& host, const string service,
                      const string& target, const std::map<string, string>* params = nullptr, const vector<string>* cookies = nullptr,
                      const string* http_body = nullptr, const string* content_type = nullptr)
    {
        cb_ = move(cb);
        initRequest(method, host, target, params, cookies, http_body, content_type);
        // Look up the domain name
        resolver_.async_resolve(
            host, service, boost::beast::bind_front_handler(&HttpClientImpl::onResolve, shared_from_this()));
    }

private:
    void initRequest(zephyr::http::verb method, const string& host, const string& target,
        const std::map<string, string>* params = nullptr, const vector<string>* cookies = nullptr,
        const string* http_body = nullptr, const string* content_type = nullptr)
    {
        // Set up an HTTP GET request message
        req_.version(11);
        req_.method(method);
        req_.set(http::field::host, host);
        req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        string url;
        url.reserve(512);
        url.append(target);
        if (params)
        {
            url.append("?");
            for (const auto& kv : *params)
            {
                url.append(kv.first).append("=").append(kv.second).append("&");
            }
            url.resize(url.size() - 1);
        }
        req_.target(url);

        if (cookies)
        {
            string cookie;
            cookie.reserve(512);
            for (const auto& ck : *cookies)
            {
                cookie.append(ck).append("; ");
            }
            if (cookie.size() >= 2)
            {
                cookie.resize(cookie.size() - 2);
            }
            req_.set(zephyr::http::field::cookie, cookie);
        }

        if (http_body)
        {
            req_.body() = *http_body;
        }

        if (content_type)
        {
            req_.set(zephyr::http::field::content_type, *content_type);
        }
        req_.prepare_payload();
    }

    void onResolve(const boost::beast::error_code& ec, boost::asio::ip::tcp::resolver::results_type results)
    {
        if (ec)
        {
            if (cb_)
            {
                cb_(make_pair(false, ec.message()));
                cb_ = nullptr;
            }
            else
            {
                promise_.set_value(make_pair(false, ec.message()));
            }

            return;
        }

        // Set a timeout on the operation
        stream_.expires_after(connectTimeout_);
        // Make the connection on the IP address we get from a lookup
        stream_.async_connect(
            results, boost::beast::bind_front_handler(&HttpClientImpl::onConnect, shared_from_this()));
    }

    void onConnect(const boost::beast::error_code& ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type)
    {
        if (ec)
        {
            if (cb_)
            {
                cb_(make_pair(false, ec.message()));
                cb_ = nullptr;
            }
            else
            {
                promise_.set_value(make_pair(false, ec.message()));
            }

            return;
        }

        // Set a timeout on the operation
        stream_.expires_after(ioTimeout_);
        // Send the HTTP request to the remote host
        boost::beast::http::async_write(
            stream_, req_, boost::beast::bind_front_handler(&HttpClientImpl::onWrite, shared_from_this()));
    }

    void onWrite(const boost::beast::error_code& ec, size_t)
    {
        if (ec)
        {
            if (cb_)
            {
                cb_(make_pair(false, ec.message()));
                cb_ = nullptr;
            }
            else
            {
                promise_.set_value(make_pair(false, ec.message()));
            }

            return;
        }

        // Set a timeout on the operation
        stream_.expires_after(ioTimeout_);
        // Receive the HTTP response
        boost::beast::http::async_read(
            stream_, buffer_, res_, boost::beast::bind_front_handler(&HttpClientImpl::onRead, shared_from_this()));
    }

    void onRead(const boost::beast::error_code& ec, size_t)
    {
        if (ec)
        {
            if (cb_)
            {
                cb_(make_pair(false, ec.message()));
                cb_ = nullptr;
            }
            else
            {
                promise_.set_value(make_pair(false, ec.message()));
            }

            return;
        }

        if (res_.result() != boost::beast::http::status::ok)
        {
            if (cb_)
            {
                cb_(make_pair(false, res_.reason().to_string()));
                cb_ = nullptr;
            }
            else
            {
                promise_.set_value(make_pair(false, res_.reason().to_string()));
            }

            return;
        }

        if (cb_)
        {
            cb_(make_pair(true, res_.body()));
            cb_ = nullptr;
        }
        else
        {
            promise_.set_value(make_pair(true, res_.body()));
        }
    }

private:
    chrono::seconds connectTimeout_;
    chrono::seconds ioTimeout_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::tcp_stream stream_;
    boost::beast::http::request<boost::beast::http::string_body> req_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::response<boost::beast::http::string_body> res_;
    promise<pair<bool, string>> promise_;
    function<void(pair<bool, string>)> cb_;
};

AFCHttpClientModule::AFCHttpClientModule()
{
    thread t([this]() {
        boost::asio::io_context::work w(ioc_);
        ioc_.run();
    });
    t.detach();
}

future<pair<bool, string>> AFCHttpClientModule::asyncRequest(zephyr::http::verb method, const string& host,
    const string& service, const string& target, const std::map<string, string>* params, const vector<string>* cookies,
    const string* http_body, const string* content_type, int ioTimeout, int connectTimeout)
{
    return make_shared<HttpClientImpl>(ioc_, connectTimeout, ioTimeout)
        ->AsyncRequest(method, host, service, target, params, cookies, http_body, content_type);
}

void AFCHttpClientModule::asyncRequest(function<void(std::pair<bool, string>)> cb, zephyr::http::verb method, const string& host,
                  const string& service, const string& target, const std::map<string, string>* params,
                  const vector<string>* cookies, const string* http_body, const string* content_type, int ioTimeout, int connectTimeout)
{
    return make_shared<HttpClientImpl>(ioc_, connectTimeout, ioTimeout)->AsyncRequest([cb=move(cb), this](pair<bool, string> arg) {
      this->finishedRequests_.Push(make_shared<Callback>(move(cb), move(arg)));
    }, method, host, service, target, params, cookies, http_body, content_type);
}

} // namespace ark
