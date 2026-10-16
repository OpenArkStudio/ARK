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
#include <boost/asio/spawn.hpp>
#include <boost/make_shared.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/variant.hpp>
#include <bredis/Connection.hpp>
#include <bredis/Extract.hpp>
#include <bredis/MarkerHelpers.hpp>
#include <bredis/Markers.hpp>

#include "ark/container/list.hpp"
#include "ark/container/map.hpp"
#include "ark/platform.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

using socket_t = boost::asio::ip::tcp::socket;
using redis_buffer = boost::asio::streambuf;
using iterator = typename bredis::to_iterator<redis_buffer>::iterator_t;
using redis_connection = bredis::Connection<socket_t>;
using policy = bredis::parsing_policy::keep_result;
using result_t = bredis::parse_result_mapper_t<iterator, policy>;

class redis_conn
{
public:
    redis_conn(const std::string& ip, const uint16_t& port, const uint32_t& id)
        : ip_(ip)
        , port_(port)
        , id_(id) {
    }

    bool connect() {
        boost::asio::ip::tcp::endpoint end_point(boost::asio::ip::make_address(ip_), port_);
        socket_ = boost::make_shared<socket_t>(io_srv_, end_point.protocol());
        boost::asio::socket_base::linger option(true, 0);
        boost::asio::socket_base::reuse_address re_option(true);
        socket_->set_option(option);
        socket_->set_option(re_option);
        bool success = false;
        while (!success)
        {
            try
            {
                socket_->connect(end_point);
                success = true;
            }
            catch ([[maybe_unused]] boost::system::error_code& ec)
            {
                socket_->close();
                ARK_LOG_ERROR("Connect to redis server failed");
                return false;
            }

            catch ([[maybe_unused]] boost::system::system_error& se)
            {
                if (se.code().value() != 10048)
                {
                    ARK_LOG_ERROR("Connect to redis server failed");
                    return false;
                }
            }
        }
        c_ = boost::make_shared<redis_connection>(std::move(*socket_));
        is_connected_ = true;
        return true;
    }

    ~redis_conn() {
        io_srv_.stop();
        socket_->close();
    }

    bool close() {
        if (!is_connected_)
        {
            return false;
        }
        socket_->close();
        c_ = nullptr;
        is_connected_ = false;
        return true;
    }

    const boost::shared_ptr<redis_connection> get_redis_conn() {
        if (!is_connected_)
        {
            return nullptr;
        }
        return c_;
    }

    auto get_io() {
        return std::ref(io_srv_);
    }

    void reconnect() {
        bool success = false;
        boost::asio::ip::tcp::endpoint end_point(boost::asio::ip::make_address(ip_), port_);
        socket_.reset(new socket_t(io_srv_, end_point.protocol()));
        socket_->set_option(boost::asio::socket_base::reuse_address(true));
        while (!success)
        {
            try
            {
                socket_->connect(end_point);
                success = true;
            }
            catch ([[maybe_unused]] boost::system::error_code& ec)
            {
                ARK_LOG_ERROR("Reconnect to the redis server failed");
                socket_->close();
                return;
            }
            catch ([[maybe_unused]] boost::system::system_error& se)
            {}
        }
        c_ = boost::make_shared<redis_connection>(std::move(*socket_));
        is_connected_ = true;
    }

    uint32_t get_id() {
        return id_;
    }

private:
    std::string ip_;
    uint16_t port_;
    boost::asio::io_service io_srv_;
    boost::shared_ptr<redis_connection> c_;
    boost::shared_ptr<socket_t> socket_;
    bool is_connected_ = false;
    uint32_t id_;
};

class redis_pool
{
public:
    redis_pool() {
    }

    virtual void append_conn(redis_conn* conn) = 0;

    virtual void close_conn() = 0;

    virtual uint32_t size() {
        return pool_size_.load();
    }

    virtual void reconnect() = 0;

private:
    std::atomic_uint32_t pool_size_;
};

class working_pool : public redis_pool
{
public:
    working_pool() = default;

    ~working_pool() {
        for (auto iter : working_pool_)
        {
            iter.second->close();
            delete iter.second;
        }
        working_pool_.clear();
    }

    bool release_conn(redis_conn* conn) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto iter = working_pool_.find(conn->get_id());
        while (pool_size_ <= 0)
        {
            condition_.wait(lock);
        }
        if (iter == working_pool_.end())
        {
            return false;
        }
        working_pool_.erase(iter);
        pool_size_ -= 1;
        return true;
    }

    void append_conn(redis_conn* conn) override {
        std::lock_guard<std::mutex> lock(mutex_);
        working_pool_.insert(conn->get_id(), conn);
        pool_size_ += 1;
        condition_.notify_one();
    }

    void close_conn() override {
        for (auto iter : working_pool_)
        {
            iter.second->close();
        }
        pool_size_ = 0;
    }

    void reconnect() override {
        for (auto iter : working_pool_)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (pool_size_ > 0)
            {
                condition_.notify_all();
                lock.unlock();
                iter.second->reconnect();
                pool_size_ += 1;
            }
            else
            {
                iter.second->reconnect();
                pool_size_ += 1;
                condition_.notify_all();
                lock.unlock();
            }
        }
    }

private:
    object_map<uint32_t, redis_conn> working_pool_;
    std::atomic_uint32_t pool_size_{0};
    std::mutex mutex_;
    std::condition_variable condition_;
};

class idle_pool : public redis_pool
{
public:
    idle_pool(const uint32_t& pool_size, const std::string& ip, const uint16_t& port) {
        for (uint32_t id = 0; id < pool_size; ++id)
        {
            idle_pool_.push_back(new redis_conn(ip, port, id));
        }
        pool_size_.store(pool_size);
    }

    ~idle_pool() {
        for (uint32_t i = 0; i < pool_size_; ++i)
        {
            redis_conn* conn = nullptr;
            idle_pool_.get_value(i, conn);
            if (conn != nullptr)
            {
                conn->close();
                delete conn;
            }
        }
        pool_size_ = 0;
    }

    redis_conn* get_connection() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (pool_size_ <= 0)
        {
            condition_.wait(lock);
        }
        pool_size_ -= 1;
        redis_conn* conn = nullptr;
        idle_pool_.get_value(0, conn);
        idle_pool_.pop_front();
        lock.unlock();
        return conn;
    }

    void append_conn(redis_conn* conn) {
        std::lock_guard<std::mutex> lock(mutex_);
        idle_pool_.push_front(conn);
        pool_size_ += 1;
        condition_.notify_all();
    }

    void close_conn() override {
        for (uint32_t i = 0; i < pool_size_; ++i)
        {
            redis_conn* conn = nullptr;
            idle_pool_.get_value(i, conn);
            conn->close();
        }
        pool_size_ = 0;
    }

    void reconnect() override {
        for (auto iter : idle_pool_)
        {

            std::unique_lock<std::mutex> lock(mutex_);
            if (pool_size_ > 0)
            {
                condition_.notify_all();
                lock.unlock();
                iter->reconnect();
                pool_size_ += 1;
            }
            else
            {
                iter->reconnect();
                pool_size_ += 1;
                condition_.notify_all();
                lock.unlock();
            }
        }
    }

    list<redis_conn*> get_all_conn() {
        return idle_pool_;
    }

private:
    list<redis_conn*> idle_pool_;
    std::atomic_uint32_t pool_size_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

class redis_manager final : public singleton<redis_manager>
{
public:
    void create_pool(const uint32_t& pool_size, const std::string& ip, const uint16_t& port) {
        if (is_closed_)
        {
            if (working_pool_ != nullptr || idle_pool_ != nullptr)
            {
                working_pool_->close_conn();
                idle_pool_->close_conn();
            }
            is_closed_.store(false);
            working_pool_ = std::make_shared<working_pool>();
            idle_pool_ = std::make_shared<idle_pool>(pool_size, ip, port);
        }
    }

    void remove_pool() {
        if (is_closed_ || is_reconnecting_)
        {
            return;
        }
        std::lock_guard<std::mutex> recoon_lock(reconn_mutex_);
        std::lock_guard<std::mutex> close_lock(close_mutex_);
        is_closed_.store(true);
        working_pool_->close_conn();
        idle_pool_->close_conn();
    }

    redis_conn* get_conn() {
        if (is_closed_)
        {
            return nullptr;
        }
        std::lock_guard<std::mutex> lock(close_mutex_);
        auto conn = idle_pool_->get_connection();
        working_pool_->append_conn(conn);
        return conn;
    }

    list<redis_conn*> get_all_conn() {
        return idle_pool_->get_all_conn();
    }

    bool release_conn(redis_conn* conn) {
        if (working_pool_->size() <= 0)
        {
            return false;
        }
        working_pool_->release_conn(conn);
        idle_pool_->append_conn(conn);
        return true;
    }

    void do_reconnect() {
        idle_pool_->reconnect();
        working_pool_->reconnect();
        is_reconnecting_.store(false);
    }

    void reconnect() {
        if (!is_closed_ || is_reconnecting_)
        {
            return;
        }
        std::unique_lock<std::mutex> lock(reconn_mutex_);
        is_reconnecting_.store(true);
        is_closed_ = false;
        reconn_thread_ = std::thread([this] { this->do_reconnect(); });
        lock.unlock();
        return;
    }

private:
    std::shared_ptr<idle_pool> idle_pool_{nullptr};
    std::shared_ptr<working_pool> working_pool_{nullptr};
    std::atomic_bool is_closed_{true};
    std::atomic_bool is_reconnecting_{false};
    std::thread reconn_thread_;
    std::mutex reconn_mutex_;
    std::mutex close_mutex_;
};
} // namespace ark