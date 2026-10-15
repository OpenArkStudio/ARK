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

#include "ark/plugin_manager.hpp"
#include "ark/redis/redis_manager.hpp"
#include "ark/redis/redis_module.hpp"

namespace ark {
class is_corr : public boost::static_visitor<redis_err_code>
{
public:
    redis_err_code operator()(const bredis::markers::int_t<iterator>& i) const {
        return redis_err_code::ok;
    }

    redis_err_code operator()(const bredis::markers::string_t<iterator>& i) const {
        return redis_err_code::ok;
    }

    redis_err_code operator()(const bredis::markers::array_wrapper_t<iterator>& i) const {
        return redis_err_code::ok;
    }

    redis_err_code operator()(const bredis::markers::nil_t<iterator>& i) const {
        return redis_err_code::miss;
    }

    redis_err_code operator()(const bredis::markers::error_t<iterator>& i) const {
        return redis_err_code::err;
    }
};

class redis_module_impl final : public redis_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init();

    // Synchronous commands
    redis_err_code Get(const std::string& key, std::string& value);
    redis_err_code Set(const std::string& key, const std::string& value, const uint64_t expire_time = 0);
    redis_err_code Incr(std::string const& key, int64_t& value);
    redis_err_code HGet(const std::string& key, const std::string& field, std::string& value);
    redis_err_code HSet(const std::string& key, const std::string& field, const std::string& value);
    redis_err_code SetNX(const std::string& key, const std::string& value);
    redis_err_code GetSet(const std::string& key, const std::string& value, std::string& str);
    redis_err_code LLen(const std::string& key, OUT int64_t& length);
    redis_err_code LRange(
        const std::string& key, const int64_t& begin, const int64_t& end, OUT value_vector<std::string>& res_list);
    redis_err_code LPop(const std::string& key, OUT std::string& value);
    redis_err_code RPop(const std::string& key, OUT std::string& value);
    redis_err_code SCard(const std::string& key, OUT int64_t& value);
    redis_err_code SMembers(const std::string& key, OUT value_vector<std::string>& res_list);
    redis_err_code SRandMember(const std::string& key, const uint32_t& count, OUT value_vector<std::string>& res_list);
    redis_err_code SPop(
        const std::string& key, const uint32_t& count, OUT value_vector<std::string>& res_list); // For redis 3.2+
    redis_err_code SPop(const std::string& key, OUT std::string& value);
    redis_err_code SIsMember(const std::string& key, const std::string& member, bool& res);
    redis_err_code ZIncrBy(
        const std::string& key, const double& increment, const std::string& member, OUT double& result_value);
    redis_err_code ZRange(const std::string& key, const int64_t& begin, const int64_t& end,
        OUT value_vector<std::string>& res_list, bool with_score = false);
    redis_err_code ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        OUT value_vector<std::string>& res_list, bool with_score = false);
    redis_err_code ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, OUT value_vector<std::string>& res_list,
        bool with_score = false); //with limit
    redis_err_code ZRevRange(const std::string& key, const std::int64_t& begin, const int64_t& end,
        OUT value_vector<std::string>& res_list, bool with_score = false);
    redis_err_code ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        OUT value_vector<std::string>& res_list, bool with_score = false);
    redis_err_code ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, OUT value_vector<std::string>& res_list,
        bool with_score = false); // with limit
    redis_err_code ZRank(const std::string& key, const std::string& member, OUT int64_t& rank_res);
    redis_err_code ZRevRank(const std::string& key, const std::string& member, OUT int64_t& rank_res);
    redis_err_code ZCard(const std::string& key, OUT int64_t& count);
    redis_err_code ZCount(const std::string& key, const std::string& min, const std::string& max, OUT int64_t& count);
    redis_err_code ZScore(const std::string& key, const std::string member, OUT double& score);
    redis_err_code ZRemRangeByRank(
        const std::string& key, const int64_t& begin, const int64_t& end, OUT int64_t& count);
    redis_err_code ZRemRangeByScore(
        const std::string& key, const std::string& min, const std::string& max, OUT int64_t& count);
    redis_err_code HGetAll(const std::string& key, OUT value_vector<std::string>& res_list);
    redis_err_code HLen(const std::string& key, OUT int64_t& length);
    redis_err_code HSetNX(const std::string& key, const std::string& field, const std::string& value);
    redis_err_code HExists(const std::string& key, const std::string& field, OUT bool& exist);
    redis_err_code HKeys(const std::string& key, OUT value_vector<std::string>& fields);
    redis_err_code HDel(const std::string& key, const std::string& field, OUT bool& is_deleted);
    redis_err_code HIncrBy(
        const std::string& key, const std::string& field, const int64_t& increment, OUT int64_t& result_value);
    redis_err_code HIncrByFloat(
        const std::string& key, const std::string& field, const double& increment, OUT double& result_value);
    redis_err_code HScan(const std::string& key, OUT int64_t& ret_cursor, OUT value_vector<std::string>& res_list,
        const int64_t& cursor, const std::string& pattern, const uint32_t& count = 10);
    redis_err_code Exists(const std::string& key, OUT bool& exist);
    redis_err_code Del(const std::string& key);
    redis_err_code IncrBy(const std::string& key, const int64_t& increment, OUT int64_t& result);
    redis_err_code DecrBy(const std::string& key, const int64_t& decrement, OUT int64_t& result);
    redis_err_code Expire(const std::string& key, const int64_t& expire_time, OUT bool& result);

    redis_err_code MSet(const std::vector<std::string>& commands, const std::string& command, const std::string& key);
    redis_err_code MGet(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        value_vector<std::string>& str_list);
    redis_err_code LPush(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    redis_err_code RPush(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    redis_err_code SRem(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    redis_err_code SAdd(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    redis_err_code ZAdd(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length);
    redis_err_code ZRem(
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
    void LRange(const std::string& key, const int64_t& begin, const int64_t& end, RedisArrayFunc func);
    void LPop(const std::string& key, RedisStrFunc func);
    void RPop(const std::string& key, RedisStrFunc func);
    void SCard(const std::string& key, RedisIntFunc func);
    void SMembers(const std::string& key, RedisArrayFunc func);
    void SRandMember(const std::string& key, const uint32_t& count, RedisArrayFunc func);
    void SPop(const std::string& key, const uint32_t& count, RedisArrayFunc func); // For redis 3.2+
    void SPop(const std::string& key, RedisStrFunc func);
    void SIsMember(const std::string& key, const std::string& member, RedisIntFunc func);
    void ZIncrBy(const std::string& key, const double& increment, const std::string& member, RedisStrFunc func);
    void ZRange(
        const std::string& key, const int64_t& begin, const int64_t& end, RedisArrayFunc func, bool with_score = false);
    void ZRangeByScore(const std::string& key, const std::string& min, const std::string& max, RedisArrayFunc func,
        bool with_score = false);
    void ZRangeByScore(const std::string& key, const std::string& min, const std::string& max, const int64_t& offset,
        const uint32_t& count, RedisArrayFunc func,
        bool with_score = false); //with limit
    void ZRevRange(const std::string& key, const std::int64_t& begin, const int64_t& end, RedisArrayFunc func,
        bool with_score = false);
    void ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max, RedisArrayFunc func,
        bool with_score = false);
    void ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max, const int64_t& offset,
        const uint32_t& count, RedisArrayFunc func,
        bool with_score = false); // with limit
    void ZRank(const std::string& key, const std::string& member, RedisIntFunc func);
    void ZRevRank(const std::string& key, const std::string& member, RedisIntFunc func);
    void ZCard(const std::string& key, RedisIntFunc func);
    void ZCount(const std::string& key, const std::string& min, const std::string& max, RedisIntFunc func);
    void ZScore(const std::string& key, const std::string member, RedisStrFunc func);
    void ZRemRangeByRank(const std::string& key, const int64_t& begin, const int64_t& end, RedisIntFunc func);
    void ZRemRangeByScore(const std::string& key, const std::string& min, const std::string& max, RedisIntFunc func);
    void HGetAll(const std::string& key, RedisArrayFunc func);
    void HLen(const std::string& key, RedisIntFunc func);
    void HSetNX(const std::string& key, const std::string& field, const std::string& value, RedisNilFunc func);
    void HExists(const std::string& key, const std::string& field, RedisIntFunc func);
    void HKeys(const std::string& key, RedisArrayFunc func);
    void HDel(const std::string& key, const std::string& field, RedisIntFunc func);
    void HIncrBy(const std::string& key, const std::string& field, const int64_t& increment, RedisIntFunc func);
    void HIncrByFloat(const std::string& key, const std::string& field, const double& increment, RedisStrFunc func);
    void HScan(const std::string& key,
        void (*HscanFunc)(const redis_err_code&, const int64_t&, const std::vector<std::string>&),
        const int64_t& cursor, const std::string& pattern, const uint32_t& count = 10);
    void Exists(const std::string& key, RedisIntFunc func);
    void Del(const std::string& key, RedisNilFunc func);
    void IncrBy(const std::string& key, const int64_t& increment, RedisIntFunc func);
    void DecrBy(const std::string& key, const int64_t& decrement, RedisIntFunc func);
    void Expire(const std::string& key, const int64_t& expire_time, RedisIntFunc func);

    void AsynMSet(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisNilFunc func);
    void AsynMGet(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisArrayFunc func);
    void AsynLPush(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func);
    void AsynRPush(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func);
    void AsynSRem(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func);
    void AsynSAdd(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func);
    void AsynZAdd(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func);
    void AsynZRem(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func);

    bool AuthPwd(const std::string& password, redis_conn* conn);
    bool Connect();
    bool Reconnect();
    bool Disconnect();

private:
    redis_err_code IsOk(const bredis::markers::redis_result_t<iterator>& res) {
        return boost::apply_visitor(is_corr(), res);
    }

    template<typename... Args>
    redis_err_code OnRetNilCommand(const std::string& command, const std::string& key, const Args&... args) {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        redis_err_code err_code;
        std::apply(
            [this, &key, &command, &err_code](auto&&... params) { err_code = RetNilCommand(command, key, params...); },
            ret);
        return err_code;
    }

    template<typename... Args>
    redis_err_code RetNilCommand(const std::string& command, const std::string& key, const Args&... args) {
        auto c = redis_manager::instance()->GetConn();
        if (c == nullptr)
        {
            return redis_err_code::disconnect;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            return redis_err_code::disconnect;
        }
        redis_buffer rx_buff;

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
            redis_manager::instance()->ReleaseConn(c);
            auto err_code = IsOk(result_markers.result);
            rx_buff.consume(result_markers.consumed);
            return err_code;
        }
        catch ([[maybe_unused]] boost::system::system_error& ec)
        {
            redis_manager::instance()->ReleaseConn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return redis_err_code::disconnect;
        }
    }

    template<typename... Args>
    redis_err_code OnRetIntCommand(
        const std::string& command, const std::string& key, int64_t& value, const Args&... args) {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        redis_err_code err_code;
        std::apply([this, &key, &value, &command, &err_code](
                       auto&&... params) { err_code = RetIntCommand(command, key, value, params...); },
            ret);
        return err_code;
    }

    template<typename... Args>
    redis_err_code RetIntCommand(
        const std::string& command, const std::string& key, int64_t& value, const Args&... args) {
        auto c = redis_manager::instance()->GetConn();
        if (c == nullptr)
        {
            return redis_err_code::disconnect;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            return redis_err_code::disconnect;
        }

        redis_buffer rx_buff;

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
            redis_manager::instance()->ReleaseConn(c);
            rx_buff.consume(result_markers.consumed);
            auto err_code = IsOk(result_markers.result);
            if (err_code != redis_err_code::ok)
            {
                return err_code;
            }
            auto extract = boost::apply_visitor(bredis::extractor<iterator>(), result_markers.result);
            value = boost::get<bredis::extracts::int_t>(extract);
            return err_code;
        }
        catch ([[maybe_unused]] boost::system::system_error& ec)
        {
            redis_manager::instance()->ReleaseConn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return redis_err_code::disconnect;
        }
    }

    template<typename... Args>
    redis_err_code OnRetStrCommand(
        const std::string& command, const std::string& key, std::string& value, const Args&... args) {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        redis_err_code err_code;
        std::apply([this, &key, &value, &command, &err_code](
                       auto&&... params) { err_code = RetStrCommand(command, key, value, params...); },
            ret);
        return err_code;
    }

    template<typename... Args>
    redis_err_code RetStrCommand(
        const std::string& command, const std::string& key, std::string& value, const Args&... args) {
        auto c = redis_manager::instance()->GetConn();
        if (c == nullptr)
        {
            return redis_err_code::disconnect;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            return redis_err_code::disconnect;
        }

        redis_buffer rx_buff;

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
            redis_manager::instance()->ReleaseConn(c);
            rx_buff.consume(result_markers.consumed);
            auto err_code = IsOk(result_markers.result);
            if (err_code != redis_err_code::ok)
            {
                return err_code;
            }
            auto extract = boost::apply_visitor(bredis::extractor<iterator>(), result_markers.result);
            value = boost::get<bredis::extracts::string_t>(extract).str;
            return err_code;
        }
        catch ([[maybe_unused]] boost::system::system_error& ec)
        {
            redis_manager::instance()->ReleaseConn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return redis_err_code::disconnect;
        }
    }

    template<typename... Args>
    redis_err_code OnRetArrayCommand(
        const std::string& command, const std::string& key, std::vector<std::string>& value, const Args&... args) {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        redis_err_code err_code;
        std::apply([this, &key, &value, &command, &err_code](
                       auto&&... params) { err_code = RetArrayCommand(command, key, value, params...); },
            ret);
        return err_code;
    }

    template<typename... Args>
    redis_err_code RetArrayCommand(
        const std::string& command, const std::string& key, std::vector<std::string>& value, const Args&... args) {
        auto c = redis_manager::instance()->GetConn();
        if (c == nullptr)
        {
            return redis_err_code::disconnect;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            return redis_err_code::disconnect;
        }

        redis_buffer rx_buff;
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
            redis_manager::instance()->ReleaseConn(c);
            rx_buff.consume(result_markers.consumed);
            auto err_code = IsOk(result_markers.result);
            if (err_code != redis_err_code::ok)
            {
                return err_code;
            }
            auto extract = boost::apply_visitor(bredis::extractor<iterator>(), result_markers.result);

            auto& tmp_v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
            value.clear();
            for (auto iter : tmp_v)
            {
                value.emplace_back(boost::get<bredis::extracts::string_t>(iter).str);
            }
            return err_code;
        }
        catch ([[maybe_unused]] boost::system::system_error& ec)
        {
            redis_manager::instance()->ReleaseConn(c);
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
            return redis_err_code::disconnect;
        }
    }

    // For Asynchronous Command

    template<typename... Args>
    void AsyncOnRetNilCommand(
        const std::string& command, const std::string& key, RedisNilFunc func, const Args&... args) {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        std::apply(
            [this, &key, &func, &command](auto&&... params) { AsyncRetNilCommand(command, key, func, params...); },
            ret);
        return;
    }

    template<typename... Args>
    void AsyncRetNilCommand(
        const std::string& command, const std::string& key, RedisNilFunc func, const Args&... args) {
        auto c = redis_manager::instance()->GetConn();
        if (c == nullptr)
        {
            func(redis_err_code::disconnect);
            return;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            func(redis_err_code::disconnect);
            return;
        }

        auto io = c->GetIo();

        redis_buffer rx_buff, tx_buff;
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->ReleaseConn(c);
                func(redis_err_code::disconnect);
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->ReleaseConn(c);
                    func(redis_err_code::disconnect);
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    return;
                }
                redis_manager::instance()->ReleaseConn(c);
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
        const std::string& command, const std::string& key, RedisIntFunc func, const Args&... args) {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        std::apply(
            [this, &key, &func, &command](auto&&... params) { AsyncRetIntCommand(command, key, func, params...); },
            ret);
        return;
    }

    template<typename... Args>
    void AsyncRetIntCommand(
        const std::string& command, const std::string& key, RedisIntFunc func, const Args&... args) {
        auto c = redis_manager::instance()->GetConn();
        if (c == nullptr)
        {
            func(redis_err_code::disconnect, -1);
            return;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            func(redis_err_code::disconnect, -1);
            return;
        }

        auto io = c->GetIo();

        redis_buffer rx_buff, tx_buff;
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->ReleaseConn(c);
                func(redis_err_code::disconnect, -1);
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->ReleaseConn(c);
                    func(redis_err_code::disconnect, -1);
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    return;
                }
                redis_manager::instance()->ReleaseConn(c);
                rx_buff.consume(r.consumed);
                auto err_code = IsOk(r.result);
                if (err_code != redis_err_code::ok)
                {
                    func(err_code, -1);
                    return;
                }
                auto extract = boost::apply_visitor(bredis::extractor<iterator>(), r.result);
                auto result_value = boost::get<bredis::extracts::int_t>(extract);
                func(err_code, result_value);
            });
        });
        io.get().poll();
    }

    template<typename... Args>
    void AsyncOnRetStrCommand(
        const std::string& command, const std::string& key, RedisStrFunc func, const Args&... args) {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        std::apply(
            [this, &key, &func, &command](auto&&... params) { AsyncRetStrCommand(command, key, func, params...); },
            ret);
        return;
    }

    template<typename... Args>
    void AsyncRetStrCommand(
        const std::string& command, const std::string& key, RedisStrFunc func, const Args&... args) {
        auto c = redis_manager::instance()->GetConn();
        if (c == nullptr)
        {
            func(redis_err_code::disconnect, std::string());
            return;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            func(redis_err_code::disconnect, std::string());
            return;
        }

        auto io = c->GetIo();

        redis_buffer rx_buff, tx_buff;
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->ReleaseConn(c);
                func(redis_err_code::disconnect, std::string());
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->ReleaseConn(c);
                    func(redis_err_code::disconnect, std::string());
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    return;
                }
                redis_manager::instance()->ReleaseConn(c);
                rx_buff.consume(r.consumed);
                auto err_code = IsOk(r.result);
                if (err_code != redis_err_code::ok)
                {
                    func(err_code, std::string());
                    return;
                }
                auto extract = boost::apply_visitor(bredis::extractor<iterator>(), r.result);
                auto result_value = boost::get<bredis::extracts::string_t>(extract).str;
                func(err_code, result_value);
            });
        });
        io.get().poll();
    }

    template<typename... Args>
    void AsyncOnRetArrayCommand(
        const std::string& command, const std::string& key, RedisArrayFunc func, const Args&... args) {
        auto ret = std::make_tuple(Convert<Args>(args)...);
        std::apply(
            [this, &key, &func, &command](auto&&... params) { AsyncRetArrayCommand(command, key, func, params...); },
            ret);
        return;
    }

    template<typename... Args>
    void AsyncRetArrayCommand(
        const std::string& command, const std::string& key, RedisArrayFunc func, const Args&... args) {
        std::vector<std::string> result_vector;
        auto c = redis_manager::instance()->GetConn();
        if (c == nullptr)
        {
            func(redis_err_code::disconnect, result_vector);
            return;
        }
        auto conn = c->GetRedisConn();
        if (conn == nullptr)
        {
            func(redis_err_code::disconnect, result_vector);
            return;
        }

        auto io = c->GetIo();

        redis_buffer rx_buff, tx_buff;
        auto commands = bredis::single_command_t(command, args...);
        if (!key.empty())
        {
            commands = bredis::single_command_t(command, key, args...);
        }

        conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->ReleaseConn(c);
                func(redis_err_code::disconnect, result_vector);
                ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->ReleaseConn(c);
                    func(redis_err_code::disconnect, result_vector);
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    return;
                }
                redis_manager::instance()->ReleaseConn(c);
                rx_buff.consume(r.consumed);
                auto err_code = IsOk(r.result);
                if (err_code != redis_err_code::ok)
                {
                    func(err_code, result_vector);
                    return;
                }

                auto extract = boost::apply_visitor(bredis::extractor<iterator>(), r.result);
                auto& v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
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
    redis_err_code RetNilVecCommand(
        const std::string& command, const std::string& key, const std::vector<std::string>& args);
    redis_err_code RetIntVecCommand(
        const std::string& command, const std::string& key, int64_t& value, const std::vector<std::string>& args);
    redis_err_code RetStrVecCommand(
        const std::string& command, const std::string& key, std::string& value, const std::vector<std::string>& args);
    redis_err_code RetArrayVecCommand(const std::string& command, const std::string& key,
        std::vector<std::string>& value, const std::vector<std::string>& args);

    // Asynchronous
    void AsynRetNilVecCommand(
        const std::string& command, const std::string& key, RedisNilFunc func, const std::vector<std::string>& args);
    void AsynRetIntVecCommand(
        const std::string& command, const std::string& key, RedisIntFunc func, const std::vector<std::string>& args);
    void AsynRetStrVecCommand(
        const std::string& command, const std::string& key, RedisStrFunc func, const std::vector<std::string>& args);
    void AsynRetArrayVecCommand(
        const std::string& command, const std::string& key, RedisArrayFunc func, const std::vector<std::string>& args);

    bool LoadConfig();
    std::string ip_;
    uint16_t port_;
    uint32_t pool_size_;
    std::string auth_keys_;
};
} // namespace ark