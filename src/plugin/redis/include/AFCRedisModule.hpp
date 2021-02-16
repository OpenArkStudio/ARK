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
#include "redis/interface/AFIRedisModule.hpp"
#include "redis/include/AFRedisManager.hpp"

namespace ark{
class IsCorr : public boost::static_visitor<RedisErrCode>
{
public:
	RedisErrCode operator()(const bredis::markers::int_t<Iterator>& i) const
	{
		return RedisErrCode::ok;
	}
	RedisErrCode operator()(const bredis::markers::string_t<Iterator>& i) const
	{
		return RedisErrCode::ok;
	}
	RedisErrCode operator()(const bredis::markers::array_wrapper_t<Iterator>& i) const
	{
		return RedisErrCode::ok;
	}
	RedisErrCode operator()(const bredis::markers::nil_t<Iterator>& i) const
	{
		return RedisErrCode::miss;
	}
	RedisErrCode operator()(const bredis::markers::error_t<Iterator>& i) const
	{
		return RedisErrCode::err;
	}
};

class AFCRedisModule final : public AFIRedisModule
{
ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init();
    
    // Synchronous commands
	RedisErrCode Get(const std::string& key, std::string& value);
	RedisErrCode Set(const std::string& key, const std::string& value, const uint64_t expire_time = 0);
	RedisErrCode Incr(std::string const& key, int64_t& value);
	RedisErrCode HGet(const std::string& key, const std::string& field, std::string& value);
	RedisErrCode HSet(const std::string& key, const std::string& field, const std::string& value);
	RedisErrCode SetNX(const std::string& key, const std::string& value);
	RedisErrCode GetSet(const std::string& key, const std::string& value, std::string& str);
	RedisErrCode LLen(const std::string& key, OUT int64_t& length);
	RedisErrCode LRange(const std::string& key,const int64_t& begin, const int64_t& end, OUT AFVector<std::string>& res_list);
	RedisErrCode LPop(const std::string& key, OUT std::string& value);
	RedisErrCode RPop(const std::string& key, OUT std::string& value);
	RedisErrCode SCard(const std::string& key, OUT int64_t& value);
	RedisErrCode SMembers(const std::string& key, OUT AFVector<std::string>& res_list);
	RedisErrCode SRandMember(const std::string& key, const uint32_t& count, OUT AFVector<std::string>& res_list);
	RedisErrCode SPop(const std::string& key, const uint32_t& count, OUT AFVector<std::string>& res_list); // For redis 3.2+
	RedisErrCode SPop(const std::string& key, OUT std::string& value);
	RedisErrCode SIsMember(const std::string& key, const std::string& member, bool& res);
	RedisErrCode ZIncrBy(const std::string& key, const double& increment, const std::string& member, OUT double& result_value);
	RedisErrCode ZRange(const std::string& key, const int64_t& begin, const int64_t& end, OUT AFVector<std::string>& res_list, bool with_score = false);
	RedisErrCode ZRangeByScore(const std::string& key, const std::string& min, const std::string& max, OUT AFVector<std::string>& res_list, bool with_score = false);
	RedisErrCode ZRangeByScore(const std::string& key, const std::string& min, const std::string& max, const int64_t& offset, const uint32_t& count, OUT AFVector<std::string>& res_list, bool with_score = false); //with limit
	RedisErrCode ZRevRange(const std::string&key, const std::int64_t& begin, const int64_t& end, OUT AFVector<std::string>& res_list, bool with_score = false);
	RedisErrCode ZRevRangeByScore (const std::string& key, const std::string& min, const std::string& max, OUT AFVector<std::string>& res_list, bool with_score = false);
	RedisErrCode ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max, const int64_t& offset, const uint32_t& count, OUT AFVector<std::string>& res_list, bool with_score = false);// with limit
	RedisErrCode ZRank(const std::string& key, const std::string& member, OUT int64_t& rank_res);
	RedisErrCode ZRevRank(const std::string& key, const std::string& member, OUT int64_t& rank_res);
	RedisErrCode ZCard(const std::string& key, OUT int64_t& count);
	RedisErrCode ZCount(const std::string& key, const std::string& min, const std::string& max, OUT int64_t& count);
	RedisErrCode ZScore(const std::string& key, const std::string member, OUT double& score);
	RedisErrCode ZRemRangeByRank(const std::string& key, const int64_t& begin, const int64_t& end, OUT int64_t& count);
	RedisErrCode ZRemRangeByScore(const std::string& key, const std::string& min, const std::string& max, OUT int64_t& count);
	RedisErrCode HGetAll(const std::string& key, OUT AFVector<std::string>& res_list);
	RedisErrCode HLen(const std::string& key, OUT int64_t& length);
	RedisErrCode HSetNX(const std::string& key, const std::string& field, const std::string& value);
	RedisErrCode HExists(const std::string& key, const std::string& field, OUT bool& exist);
	RedisErrCode HKeys(const std::string& key, OUT AFVector<std::string>& fields);
	RedisErrCode HDel(const std::string& key, const std::string& field, OUT bool& is_deleted);
	RedisErrCode HIncrBy(const std::string& key, const std::string& field, const int64_t& increment, OUT int64_t& result_value);
	RedisErrCode HIncrByFloat(const std::string& key, const std::string& field, const double& increment, OUT double& result_value);
	RedisErrCode HScan(const std::string& key, OUT int64_t& ret_cursor, OUT AFVector<std::string>& res_list, const int64_t& cursor, const std::string& pattern, const uint32_t& count = 10);
	RedisErrCode Exists(const std::string& key, OUT bool& exist);
	RedisErrCode Del(const std::string& key);
	RedisErrCode IncrBy(const std::string& key, const int64_t& increment, OUT int64_t& result);
	RedisErrCode DecrBy(const std::string& key, const int64_t& decrement, OUT int64_t& result);
	RedisErrCode Expire(const std::string& key, const int64_t& expire_time, OUT bool& result);

    RedisErrCode MSet(const std::vector<std::string>& commands, const std::string& command, const std::string& key);
    RedisErrCode MGet(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        AFVector<std::string>& str_list);
    RedisErrCode LPush(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    RedisErrCode RPush(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    RedisErrCode SRem(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    RedisErrCode SAdd(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    RedisErrCode ZAdd(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    RedisErrCode ZRem(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);

    //Asynchronous commands
    void Get(const std::string& key, RedisStrFunc func);
    void Set(const std::string& key, const std::string& value, RedisNilFunc func, const uint64_t expire_time = 0);
    void Incr(const std::string& key, RedisIntFunc func);
    void HSet(const std::string& key, const std::string& field, const std::string& value, RedisNilFunc func);
    void HGet(const std::string& key, const std::string& field, RedisStrFunc func);
    void SetNX(const std::string& key, const std::string& value, RedisNilFunc func);
    void GetSet(const std::string& key, const std::string& value, RedisStrFunc func);
    void LLen(const std::string& key, RedisIntFunc func);
    void LRange(
        const std::string& key, const int64_t& begin, const int64_t& end, RedisArrayFunc func);
    void LPop(const std::string& key, RedisStrFunc func);
    void RPop(const std::string& key, RedisStrFunc func);
    void SCard(const std::string& key, RedisIntFunc func);
    void SMembers(const std::string& key, RedisArrayFunc func);
    void SRandMember(const std::string& key, const uint32_t& count, RedisArrayFunc func);
    void SPop(const std::string& key, const uint32_t& count, RedisArrayFunc func); // For redis 3.2+
    void SPop(const std::string& key, RedisStrFunc func);
    void SIsMember(const std::string& key, const std::string& member, RedisIntFunc func);
    void ZIncrBy(
        const std::string& key, const double& increment, const std::string& member, RedisStrFunc func);
    void ZRange(const std::string& key, const int64_t& begin, const int64_t& end, RedisArrayFunc func,
        bool with_score = false) ;
    void ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        RedisArrayFunc func, bool with_score = false);
    void ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, RedisArrayFunc func,
        bool with_score = false); //with limit
    void ZRevRange(const std::string& key, const std::int64_t& begin, const int64_t& end,
        RedisArrayFunc func, bool with_score = false);
    void ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        RedisArrayFunc func, bool with_score = false);
    void ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, RedisArrayFunc func,
        bool with_score = false); // with limit
    void ZRank(const std::string& key, const std::string& member, RedisIntFunc func);
    void ZRevRank(const std::string& key, const std::string& member, RedisIntFunc func);
    void ZCard(const std::string& key, RedisIntFunc func);
    void ZCount(
        const std::string& key, const std::string& min, const std::string& max, RedisIntFunc func);
    void ZScore(const std::string& key, const std::string member, RedisStrFunc func);
    void ZRemRangeByRank(
        const std::string& key, const int64_t& begin, const int64_t& end, RedisIntFunc func);
    void ZRemRangeByScore(
        const std::string& key, const std::string& min, const std::string& max, RedisIntFunc func);
    void HGetAll(const std::string& key, RedisArrayFunc func);
    void HLen(const std::string& key, RedisIntFunc func);
    void HSetNX(
        const std::string& key, const std::string& field, const std::string& value, RedisNilFunc func);
    void HExists(const std::string& key, const std::string& field, RedisIntFunc func);
    void HKeys(const std::string& key, RedisArrayFunc func);
    void HDel(const std::string& key, const std::string& field, RedisIntFunc func);
    void HIncrBy(
        const std::string& key, const std::string& field, const int64_t& increment, RedisIntFunc func);
    void HIncrByFloat(
        const std::string& key, const std::string& field, const double& increment, RedisStrFunc func);
    void HScan(const std::string& key, void (*HscanFunc)(const RedisErrCode&, const int64_t&, const std::vector<std::string>&),
        const int64_t& cursor, const std::string& pattern, const uint32_t& count = 10);
    void Exists(const std::string& key, RedisIntFunc func);
    void Del(const std::string& key, RedisNilFunc func);
    void IncrBy(const std::string& key, const int64_t& increment, RedisIntFunc func);
    void DecrBy(const std::string& key, const int64_t& decrement, RedisIntFunc func);
    void Expire(const std::string& key, const int64_t& expire_time, RedisIntFunc func);

	void AsynMSet(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisNilFunc func);
	void AsynMGet(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisArrayFunc func);
	void AsynLPush(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func);
	void AsynRPush(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func);
	void AsynSRem(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func);
	void AsynSAdd(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func);
	void AsynZAdd(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func);
	void AsynZRem(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func);
	

	bool AuthPwd(const std::string& password, AFRedisConn* conn);
	bool Connect();
	bool Reconnect();
	bool Disconnect();
private:
    RedisErrCode IsOk(const bredis::markers::redis_result_t<Iterator>& res)
	{
		return boost::apply_visitor(IsCorr(), res);
	}
	
	template <typename... Args>
	RedisErrCode OnRetNilCommand(const std::string& command, const std::string& key, const Args&... args)
	{
		auto ret = std::make_tuple(Convert<Args>(args)...);
		RedisErrCode err_code;
		std::apply([this, &key, &command, &err_code](auto&& ... params) {err_code = RetNilCommand(command, key, params...); }, ret);
		return err_code;
	}

	template <typename... Args>
	RedisErrCode RetNilCommand(const std::string& command, const std::string& key, const Args&... args)
	{
		auto c = AFRedisManager::instance()->GetConn();
		if (c == nullptr)
		{
			return RedisErrCode::disconnect;
		}
		auto conn = c->GetRedisConn();
		if (conn == nullptr)
		{
			return RedisErrCode::disconnect;
		}
		Buffer rx_buff;
      
        try
        {
			if (!key.empty())
			{
				conn->write(bredis::single_command_t(command, key, args...));
			}
			else
			{
				conn->write(bredis::single_command_t(command, args...));
			}
			
            auto result_markers = conn->read(rx_buff);
            AFRedisManager::instance()->ReleaseConn(c);
            auto err_code = IsOk(result_markers.result);
            rx_buff.consume(result_markers.consumed);
            return err_code;
        }
        catch ([[maybe_unused]] boost::system::system_error& ec)
        {
            AFRedisManager::instance()->ReleaseConn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return RedisErrCode::disconnect;
        }
	}

	template <typename... Args>
	RedisErrCode OnRetIntCommand(const std::string& command, const std::string& key, int64_t& value, const Args&... args)
	{
		auto ret = std::make_tuple(Convert<Args>(args)...);
		RedisErrCode err_code;
		std::apply([this, &key, &value, &command, &err_code](auto&& ... params) {err_code = RetIntCommand(command, key, value, params...); }, ret);
		return err_code;
	}

	template <typename... Args>
	RedisErrCode RetIntCommand(const std::string& command, const std::string& key, int64_t& value, const Args&... args)
	{
		auto c = AFRedisManager::instance()->GetConn();
		if (c == nullptr)
		{
			return RedisErrCode::disconnect;
		}
		auto conn = c->GetRedisConn();
		if (conn == nullptr)
		{
			return RedisErrCode::disconnect;
		}

		Buffer rx_buff;
       
        try
        {
            if (!key.empty())
			{
				conn->write(bredis::single_command_t(command, key, args...));
			}
			else
			{
				conn->write(bredis::single_command_t(command, args...));
			}
            auto result_markers = conn->read(rx_buff);
            AFRedisManager::instance()->ReleaseConn(c);
            rx_buff.consume(result_markers.consumed);
            auto err_code = IsOk(result_markers.result);
            if (err_code != RedisErrCode::ok)
            {
                return err_code;
            }
            auto extract = boost::apply_visitor(bredis::extractor<Iterator>(), result_markers.result);
            value = boost::get<bredis::extracts::int_t>(extract);
            return err_code;
        }
        catch ([[maybe_unused]] boost::system::system_error& ec)
        {
            AFRedisManager::instance()->ReleaseConn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return RedisErrCode::disconnect;
        }
        
	}

	template <typename... Args>
	RedisErrCode OnRetStrCommand(const std::string& command, const std::string& key, std::string& value, const Args&... args)
	{
		auto ret = std::make_tuple(Convert<Args>(args)...);
		RedisErrCode err_code;
		std::apply([this, &key, &value, &command, &err_code](auto&& ... params) {err_code = RetStrCommand(command, key, value, params...); }, ret);
		return err_code;
	}

	template <typename... Args>
	RedisErrCode RetStrCommand(const std::string& command, const std::string& key, std::string & value, const Args&... args)
	{
		auto c = AFRedisManager::instance()->GetConn();
		if (c == nullptr)
		{
			return RedisErrCode::disconnect;
		}
		auto conn = c->GetRedisConn();
		if (conn == nullptr)
		{
			return RedisErrCode::disconnect;
		}

		Buffer rx_buff;
     
        try
        {
           if (!key.empty())
			{
				conn->write(bredis::single_command_t(command, key, args...));
			}
			else
			{
				conn->write(bredis::single_command_t(command, args...));
			}
            auto result_markers = conn->read(rx_buff);
            AFRedisManager::instance()->ReleaseConn(c);
            rx_buff.consume(result_markers.consumed);
            auto err_code = IsOk(result_markers.result);
            if (err_code != RedisErrCode::ok)
            {
                return err_code;
            }
            auto extract = boost::apply_visitor(bredis::extractor<Iterator>(), result_markers.result);
            value = boost::get<bredis::extracts::string_t>(extract).str;
            return err_code;
        }
        catch ([[maybe_unused]] boost::system::system_error& ec)
        {
            AFRedisManager::instance()->ReleaseConn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return RedisErrCode::disconnect;
        }
        
	}

	template <typename... Args>
	RedisErrCode OnRetArrayCommand(const std::string& command, const std::string& key, std::vector<std::string>& value, const Args&... args)
	{
		auto ret = std::make_tuple(Convert<Args>(args)...);
		RedisErrCode err_code;
		std::apply([this, &key, &value, &command, &err_code](auto&& ... params) {err_code = RetArrayCommand(command, key, value, params...); }, ret);
		return err_code;
	}

	template <typename... Args>
	RedisErrCode RetArrayCommand(const std::string& command, const std::string& key, std::vector<std::string>& value, const Args&... args)
	{
		auto c = AFRedisManager::instance()->GetConn();
		if (c == nullptr)
		{
			return RedisErrCode::disconnect;
		}
		auto conn = c->GetRedisConn();
		if (conn == nullptr)
		{
			return RedisErrCode::disconnect;
		}

		Buffer rx_buff;
        try
        {
            if (!key.empty())
			{
				conn->write(bredis::single_command_t(command, key, args...));
			}
			else
			{
				conn->write(bredis::single_command_t(command, args...));
			}
            auto result_markers = conn->read(rx_buff);
            AFRedisManager::instance()->ReleaseConn(c);
            rx_buff.consume(result_markers.consumed);
            auto err_code = IsOk(result_markers.result);
            if (err_code != RedisErrCode::ok)
            {
                return err_code;
            }
            auto extract = boost::apply_visitor(bredis::extractor<Iterator>(), result_markers.result);

            auto &tmp_v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
            value.clear();
            for (auto iter : tmp_v)
            {
                value.emplace_back(boost::get<bredis::extracts::string_t>(iter).str);
            }
            return err_code;
        }
        catch ([[maybe_unused]] boost::system::system_error& ec)
        {
            AFRedisManager::instance()->ReleaseConn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return RedisErrCode::disconnect;
        }
    }

    // For Asynchronous Command


    template<typename... Args>
    void AsyncOnRetNilCommand(
        const std::string& command, const std::string& key, RedisNilFunc func, const Args&... args)
    {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        std::apply([this, &key, &func, &command](
                       auto&&... params) {AsyncRetNilCommand(command, key, func, params...);},
            ret);
        return;
    }

    template<typename... Args>
    void AsyncRetNilCommand(const std::string& command, const std::string& key, RedisNilFunc func, const Args&... args)
    {
        auto c = AFRedisManager::instance()->GetConn();
        if (c == nullptr)
        {
            func(RedisErrCode::disconnect);
            return;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            func(RedisErrCode::disconnect);
            return;
        }

        auto io = c->GetIo();

        Buffer rx_buff, tx_buff;
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                AFRedisManager::instance()->ReleaseConn(c);
                func(RedisErrCode::disconnect);
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    AFRedisManager::instance()->ReleaseConn(c);
                    func(RedisErrCode::disconnect);
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    return;
                }
                AFRedisManager::instance()->ReleaseConn(c);
                std::vector<std::string> result_vector;
                rx_buff.consume(r.consumed);
                auto err_code = IsOk(r.result);
                func(err_code);
            });
        });
        io.get().poll();
    }


    template<typename... Args>
    void AsyncOnRetIntCommand(
        const std::string& command, const std::string& key, RedisIntFunc func, const Args&... args)
    {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        std::apply([this, &key, &func, &command](
                       auto&&... params) {AsyncRetIntCommand(command, key, func, params...); },
            ret);
        return;
    }

    template<typename... Args>
    void AsyncRetIntCommand(const std::string& command, const std::string& key, RedisIntFunc func, const Args&... args)
    {
        auto c = AFRedisManager::instance()->GetConn();
        if (c == nullptr)
        {
            func(RedisErrCode::disconnect, -1);
            return;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            func(RedisErrCode::disconnect, -1);
            return;
        }

        auto io = c->GetIo();

        Buffer rx_buff, tx_buff;
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                AFRedisManager::instance()->ReleaseConn(c);
                func(RedisErrCode::disconnect, -1);
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    AFRedisManager::instance()->ReleaseConn(c);
                    func(RedisErrCode::disconnect, -1);
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    return;
                }
                AFRedisManager::instance()->ReleaseConn(c);
                rx_buff.consume(r.consumed);
                auto err_code = IsOk(r.result);
                if (err_code != RedisErrCode::ok)
                {
                    func(err_code, -1);
                    return;
                }
                auto extract = boost::apply_visitor(bredis::extractor<Iterator>(), r.result);
                auto result_value = boost::get<bredis::extracts::int_t>(extract);
                func(err_code, result_value);
            });
        });
        io.get().poll();
    }

    template<typename... Args>
    void AsyncOnRetStrCommand(const std::string& command, const std::string& key, RedisStrFunc func, const Args&... args)
    {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        std::apply([this, &key, &func, &command](
                       auto&&... params) {AsyncRetStrCommand(command, key, func, params...); },
            ret);
        return;
    }

    template<typename... Args>
    void AsyncRetStrCommand(const std::string& command, const std::string& key, RedisStrFunc func, const Args&... args)
    {
        auto c = AFRedisManager::instance()->GetConn();
        if (c == nullptr)
        {
            func(RedisErrCode::disconnect, std::string());
            return;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            func(RedisErrCode::disconnect, std::string());
            return;
        }

        auto io = c->GetIo();

        Buffer rx_buff, tx_buff;
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                AFRedisManager::instance()->ReleaseConn(c);
                func(RedisErrCode::disconnect, std::string());
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    AFRedisManager::instance()->ReleaseConn(c);
                    func(RedisErrCode::disconnect, std::string());
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    return;
                }
                AFRedisManager::instance()->ReleaseConn(c);
                rx_buff.consume(r.consumed);
                auto err_code = IsOk(r.result);
                if (err_code != RedisErrCode::ok)
                {
                    func(err_code, std::string());
                    return;
                }
                auto extract = boost::apply_visitor(bredis::extractor<Iterator>(), r.result);
                auto result_value = boost::get<bredis::extracts::string_t>(extract).str;
                func(err_code, result_value);
            });
        });
        io.get().poll();
    }


	template<typename... Args>
    void AsyncOnRetArrayCommand(
        const std::string& command, const std::string& key, RedisArrayFunc func, const Args&... args)
    {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        std::apply([this, &key, &func, &command](
                       auto&&... params) {AsyncRetArrayCommand(command, key, func, params...); },
            ret);
        return;
    }

	template<typename... Args>
    void AsyncRetArrayCommand(
        const std::string& command, const std::string& key, RedisArrayFunc func, const Args&... args)
    {
		std::vector<std::string> result_vector;
        auto c = AFRedisManager::instance()->GetConn();
        if (c == nullptr)
        {
            func(RedisErrCode::disconnect, result_vector);
            return;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            func(RedisErrCode::disconnect, result_vector);
            return;
        }

        auto io = c->GetIo();

        Buffer rx_buff, tx_buff;
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

      
        conn->async_write(
            tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                if (ec)
                {
                    AFRedisManager::instance()->ReleaseConn(c);
                    func(RedisErrCode::disconnect, result_vector);
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    return;
                        
                }
                tx_buff.consume(bytes_transferred);
                conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {  
                    if (ec)
                    {
                        AFRedisManager::instance()->ReleaseConn(c);
                        func(RedisErrCode::disconnect, result_vector);
                        ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                        return;
                    }
                    AFRedisManager::instance()->ReleaseConn(c);
                    rx_buff.consume(r.consumed);
                    auto err_code = IsOk(r.result);
                    if (err_code != RedisErrCode::ok)
                    {
                        func(err_code, result_vector);
                        return;
                    }
                    
                    auto extract = boost::apply_visitor(bredis::extractor<Iterator>(), r.result);
                    auto &v =  boost::get<bredis::extracts::array_holder_t>(extract).elements;
                    for (auto iter : v)
                    {
                        result_vector.emplace_back(boost::get<bredis::extracts::string_t>(iter).str);
                    }
                    func(err_code, result_vector);
                });
            });
        io.get().poll();
    }



    // Use vector instead of variadic template 

    // Synchronous
	RedisErrCode RetNilVecCommand(const std::string& command, const std::string& key, const std::vector<std::string>& args);
	RedisErrCode RetIntVecCommand(const std::string& command, const std::string& key, int64_t& value, const std::vector<std::string>& args);
	RedisErrCode RetStrVecCommand(const std::string& command, const std::string& key, std::string& value, const std::vector<std::string>& args);
	RedisErrCode RetArrayVecCommand(const std::string& command, const std::string& key, std::vector<std::string>& value, const std::vector<std::string>& args);

    // Asynchronous
	void AsynRetNilVecCommand(const std::string& command, const std::string& key, RedisNilFunc func, const std::vector<std::string>& args);
	void AsynRetIntVecCommand(const std::string& command, const std::string& key, RedisIntFunc func, const std::vector<std::string>& args);
	void AsynRetStrVecCommand(const std::string& command, const std::string& key, RedisStrFunc func, const std::vector<std::string>& args);
	void AsynRetArrayVecCommand(const std::string& command, const std::string& key, RedisArrayFunc func, const std::vector<std::string>& args);

    bool LoadConfig();
	std::string ip_;
	uint16_t port_;
    uint32_t pool_size_;
	std::string auth_keys_;
};
}  // namespace ark