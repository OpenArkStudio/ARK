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
#include "bredis/Connection.hpp"
#include "bredis/MarkerHelpers.hpp"
#include "bredis/Extract.hpp"
#include <boost/asio/spawn.hpp>
#include <boost/variant.hpp>
#include "bredis/Markers.hpp"
#include "boost/smart_ptr/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include "base/AFPluginManager.hpp"
#include "base/AFPlatform.hpp"
#include "base/container/AFMap.hpp"
#include "base/container/AFList.hpp"

namespace ark {

using socket_t = boost::asio::ip::tcp::socket;
using Buffer = boost::asio::streambuf;
using Iterator = typename bredis::to_iterator<Buffer>::iterator_t;
using Connection = bredis::Connection<socket_t>;
using Policy = bredis::parsing_policy::keep_result;
using result_t = bredis::parse_result_mapper_t<Iterator, Policy>;
class AFRedisConn
{
public:
    AFRedisConn(const std::string& ip, const uint16_t& port, const uint32_t& id) : ip_(ip), port_(port), id_(id)
    {
    }
 
    bool Connect ()
    {
        boost::asio::ip::tcp::endpoint end_point(boost::asio::ip::make_address(ip_), port_);
        socket_ = boost::make_shared<socket_t> (io_srv_, end_point.protocol());
        boost::asio::socket_base::linger option (true, 0);
        boost::asio::socket_base::reuse_address re_option(true);
        socket_->set_option(option);
        socket_->set_option(re_option);
        bool success = false;
        while(!success)
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
            
            catch([[maybe_unused]] boost::system::system_error& se)
            {
                if(se.code().value() != 10048)
                {
                   ARK_LOG_ERROR("Connect to redis server failed");
                   return false;
                }
                
            }
            
        }
        c_ = boost::make_shared<Connection>(std::move(*socket_));
        is_connected_ = true;
        return true;
    }

    ~AFRedisConn()
    {
        io_srv_.stop();
        socket_->close();
    }

    bool Close()
    {
        if(!is_connected_)
        {
            return false;
        }
        socket_->close();
        c_ = nullptr;
        is_connected_ = false;
        return true;
    }

    const boost::shared_ptr<Connection> GetRedisConn()
    {
        if (!is_connected_)
        {
            return nullptr;
        }
        return c_;
    }

    auto GetIo() 
    {
        return std::ref(io_srv_);
    }

    void Reconnect()
    {
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
			{
			}
		}
		c_ = boost::make_shared<Connection>(std::move(*socket_));
		is_connected_ = true;
    }

    uint32_t GetID()
    {
        return id_;
    }

private:
    std::string ip_;
	uint16_t port_;
	boost::asio::io_service io_srv_;
	boost::shared_ptr<Connection> c_;
	boost::shared_ptr<socket_t> socket_;
	bool is_connected_ = false;
    uint32_t id_;
};

class AFRedisPool
{
public:
    AFRedisPool() {}

    virtual void AppendConn(AFRedisConn* conn) = 0;

    virtual void CloseConn() = 0;

    virtual uint32_t Size()
    {
        return pool_size_.load();
    }

    virtual void Reconnect() = 0;
private:
    std::atomic_uint32_t pool_size_;
};

class AFWorkingPool : public AFRedisPool
{
public:
    AFWorkingPool() = default;
    ~AFWorkingPool()
    {
        for (auto iter : working_pool_)
        {
            iter.second->Close();
            delete iter.second;
        }
        working_pool_.clear();
    }

    bool ReleaseConn(AFRedisConn* conn)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto iter = working_pool_.find(conn->GetID());
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

    void AppendConn(AFRedisConn* conn) override 
    {
        std::lock_guard<std::mutex> lock(mutex_);
        working_pool_.insert(conn->GetID(), conn);
		pool_size_ += 1;
		condition_.notify_one();
    }


    void CloseConn() override
    {
        for (auto iter : working_pool_)
        {
            iter.second->Close();
        }
        pool_size_ = 0;
    }

    void Reconnect() override
    {
        for (auto iter : working_pool_)
        {
            std::unique_lock<std::mutex> lock(mutex_);
				if (pool_size_ > 0)
				{
					condition_.notify_all();
					lock.unlock();
					iter.second->Reconnect();
					pool_size_ += 1;
				}
				else
				{
                    iter.second->Reconnect();
					pool_size_ += 1;
					condition_.notify_all();
					lock.unlock();
				}
        }
    }
private:
    AFMap<uint32_t, AFRedisConn> working_pool_;
	std::atomic_uint32_t pool_size_{0};
	std::mutex mutex_;
	std::condition_variable condition_;
};

class AFIdlePool : public AFRedisPool
{
public:
    AFIdlePool(const uint32_t& pool_size, const std::string& ip, const uint16_t& port)
    {
        for (uint32_t id = 0; id  < pool_size; ++id)
        {
            idle_pool_.push_back(new AFRedisConn(ip, port, id));
        }
        pool_size_.store(pool_size);
    }

    ~AFIdlePool() 
    {
        for (uint32_t i = 0; i < pool_size_; ++i)
        {
            AFRedisConn* conn = nullptr;
            idle_pool_.get_value(i, conn);
            if (conn != nullptr)
            {
                conn->Close();
                delete conn;
            }
        }
        pool_size_ = 0;
    }

    AFRedisConn* GetConnection()
    {
       	std::unique_lock <std::mutex> lock(mutex_);
		while(pool_size_ <= 0)
		{
			condition_.wait(lock);
		}
		pool_size_ -= 1;
		AFRedisConn* conn = nullptr;
        idle_pool_.get_value(0, conn);
		idle_pool_.pop_front();
		lock.unlock();
		return conn; 
    }

    void AppendConn(AFRedisConn* conn)
    {
        std::lock_guard<std::mutex> lock(mutex_);
		idle_pool_.push_front(conn);
		pool_size_ += 1;
		condition_.notify_all();
    }

    void CloseConn() override
    {
        for(uint32_t i = 0; i < pool_size_; ++i)
        {
            AFRedisConn* conn = nullptr;
            idle_pool_.get_value(i, conn);
            conn->Close();
        }
        pool_size_ = 0;
    }

    void Reconnect() override
    {
        for (auto iter : idle_pool_)
        {
            
			std::unique_lock<std::mutex> lock(mutex_);
			if (pool_size_ > 0)
			{
				condition_.notify_all();
				lock.unlock();
				iter->Reconnect();
				pool_size_ += 1;
			}
			else
			{
				iter->Reconnect();
				pool_size_ += 1;
				condition_.notify_all();
			    lock.unlock();
			}
        }
    }

    AFList<AFRedisConn*> GetAllConn()
    {
        return idle_pool_;
    }
   
private:
    AFList<AFRedisConn*> idle_pool_;
	std::atomic_uint32_t pool_size_;
	std::mutex mutex_;
	std::condition_variable condition_;
};

class AFRedisManager final : public AFSingleton<AFRedisManager>
{
public:

    void CreatePool(const uint32_t& pool_size, const std::string& ip, const uint16_t& port)
    {
        if (is_closed)
		{
			if (working_pool_ != nullptr || idle_pool_ != nullptr)
			{
				working_pool_->CloseConn();
				idle_pool_->CloseConn();
			}
			is_closed.store(false);
			working_pool_ = std::make_shared<AFWorkingPool>();
			idle_pool_ = std::make_shared<AFIdlePool>(pool_size, ip, port);
		}
    }


    void RemovePool()
    {
        if (is_closed || is_reconnecting)
        {
            return;
        }
        std::lock_guard<std::mutex> recoon_lock(reconn_mutex_);
        std::lock_guard<std::mutex> close_lock(close_mutex_);
        is_closed.store(true);
        working_pool_->CloseConn();
        idle_pool_->CloseConn();
    }

    AFRedisConn* GetConn()
    {
        if (is_closed)
        {
            return nullptr;
        }
        std::lock_guard<std::mutex> lock(close_mutex_);
        auto conn = idle_pool_->GetConnection();
        working_pool_->AppendConn(conn);
        return conn;
    }

    AFList<AFRedisConn*> GetAllConn()
    {
        return idle_pool_->GetAllConn();
    }

    bool ReleaseConn(AFRedisConn* conn)
    {
        if(working_pool_->Size() <= 0)
        {
            return false;
        }
        working_pool_->ReleaseConn(conn);
        idle_pool_->AppendConn(conn);
        return true;
    }

    void DoReconnect()
    {
        idle_pool_->Reconnect();
		working_pool_->Reconnect();
		is_reconnecting.store(false);
    }

    void Reconnect()
    {
        if (!is_closed || is_reconnecting)
		{
			return;
		}
		std::unique_lock<std::mutex> lock(reconn_mutex_);
		is_reconnecting.store(true);
		is_closed = false;
		reconn_thread_ = std::thread ([this] {
			this->DoReconnect();
			});
		lock.unlock();
		return;
    }

private:
	std::shared_ptr<AFIdlePool> idle_pool_{nullptr};
	std::shared_ptr<AFWorkingPool> working_pool_{nullptr};
	std::atomic_bool is_closed{true};
	std::atomic_bool is_reconnecting{ false };
	std::thread reconn_thread_;
	std::mutex reconn_mutex_;
	std::mutex close_mutex_;
};
}  // namespace ark