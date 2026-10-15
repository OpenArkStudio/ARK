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
#include "ark/define.hpp"
#include "ark/enum.hpp"
#include "ark/redis/redis_module_impl.hpp"
#include "ark/redis/redis_plugin.hpp"
#include "ark/xml.hpp"

namespace ark {

bool redis_module_impl::init() {
    if (!LoadConfig())
    {
        return false;
    }
    redis_manager::instance()->CreatePool(pool_size_, ip_, port_);
    ;
    return Connect();
}

bool redis_module_impl::LoadConfig() {
    ///load config
    xml xml(redis_plugin::get_plugin_conf());
    auto root = xml.get_root_node();
    ARK_ASSERT_RET_VAL(root.is_valid(), false);

    std::unordered_map<std::string, std::string> key_value_list;
    for (auto set = root.find_node("set"); set.is_valid(); set.next_node())
    {
        auto name = set.get_string("name");
        auto value = set.get_string("value");
        bool result = key_value_list.insert(std::make_pair(name, value)).second;
        ARK_ASSERT_RET_VAL(result, false);
    }

    auto func = [&key_value_list](const std::string& name) -> const std::string& {
        auto iter = key_value_list.find(name);
        if (iter == key_value_list.end())
        {
            return NULL_STR;
        }

        return iter->second;
    };

    ip_ = func("Ip");
    port_ = misc::from_string<uint16_t>(func("Port"));
    pool_size_ = misc::from_string<uint32_t>(func("PoolSize"));
    auth_keys_ = func("Password");
    return true;
}

bool redis_module_impl::Connect() {
    auto all_conn = redis_manager::instance()->GetAllConn();
    for (auto iter : all_conn)
    {
        auto ret = iter->Connect();
        if (!ret)
        {
            return false;
        }
        ret = AuthPwd(auth_keys_, iter);
        if (!ret)
        {
            return false;
        }
    }
    ARK_LOG_INFO("Connect to Redis successfully! ip = {}, port = {}", ip_, port_);
    return true;
}

bool redis_module_impl::AuthPwd(const std::string& password, redis_conn* c) {
    if (password.empty())
    {
        return true;
    }

    auto conn = c->GetRedisConn();
    if (conn == nullptr)
    {
        return false;
    }

    redis_buffer rx_buff;
    try
    {
        conn->write(bredis::single_command_t("AUTH", password));
        auto result_markers = conn->read(rx_buff);
        auto err_code = IsOk(result_markers.result);
        rx_buff.consume(result_markers.consumed);
        if (err_code != redis_err_code::ok)
        {
            return false;
        }
    }
    catch ([[maybe_unused]] boost::system::system_error& ec)
    {
        ARK_LOG_ERROR("Redis authentication Failed");
        return false;
    }
    return true;
}

bool redis_module_impl::Reconnect() {
    redis_manager::instance()->RemovePool();
    redis_manager::instance()->Reconnect();
    return true;
}

bool redis_module_impl::Disconnect() {
    redis_manager::instance()->RemovePool();
    return true;
}

//Synchronous Commands Implement

redis_err_code redis_module_impl::Get(const std::string& key, std::string& value) {
    return OnRetStrCommand("GET", key, value);
}

redis_err_code redis_module_impl::Set(const std::string& key, const std::string& value, const uint64_t expire_time) {
    if (expire_time > 0)
    {
        return OnRetNilCommand("SETEX", key, expire_time, value);
    }
    else
    {
        return OnRetNilCommand("SET", key, value);
    }
}

redis_err_code redis_module_impl::SetNX(const std::string& key, const std::string& value) {
    return OnRetNilCommand("SETNX", key, value);
}

redis_err_code redis_module_impl::HSet(const std::string& key, const std::string& field, const std::string& value) {
    return OnRetNilCommand("HSET", key, field, value);
}

redis_err_code redis_module_impl::GetSet(const std::string& key, const std::string& value, std::string& str) {
    return OnRetStrCommand("GETSET", key, str, value);
}

redis_err_code redis_module_impl::HGet(const std::string& key, const std::string& field, std::string& value) {
    return OnRetStrCommand("HGET", key, value, field);
}

redis_err_code redis_module_impl::MSet(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key) {
    return RetNilVecCommand(command, key, commands);
}

redis_err_code redis_module_impl::Incr(const std::string& key, int64_t& value) {
    return OnRetIntCommand("INCR", key, value);
}

redis_err_code redis_module_impl::LLen(const std::string& key, int64_t& length) {
    return OnRetIntCommand("LLEN", key, length);
}

redis_err_code redis_module_impl::LRange(
    const std::string& key, const int64_t& begin, const int64_t& end, value_vector<std::string>& res_list) {
    return OnRetArrayCommand("LRANGE", key, res_list, begin, end);
}

redis_err_code redis_module_impl::LPop(const std::string& key, std::string& value) {
    return OnRetStrCommand("LPOP", key, value);
}

redis_err_code redis_module_impl::RPop(const std::string& key, std::string& value) {
    return OnRetStrCommand("RPOP", key, value);
}

redis_err_code redis_module_impl::SCard(const std::string& key, std::int64_t& value) {
    return OnRetIntCommand("SCARD", key, value);
}

redis_err_code redis_module_impl::SMembers(const std::string& key, value_vector<std::string>& res_list) {
    return OnRetArrayCommand("SMEMBERS", key, res_list);
}

redis_err_code redis_module_impl::SRandMember(
    const std::string& key, const uint32_t& count, value_vector<std::string>& res_list) {
    return OnRetArrayCommand("SRANDMEMBER", key, res_list, count);
}

redis_err_code redis_module_impl::SPop(
    const std::string& key, const uint32_t& count, value_vector<std::string>& res_list) {
    return OnRetArrayCommand("SPOP", key, res_list, count);
}

redis_err_code redis_module_impl::SPop(const std::string& key, std::string& value) {
    return OnRetStrCommand("SPOP", key, value);
}

redis_err_code redis_module_impl::SIsMember(const std::string& key, const std::string& member, bool& res) {
    int64_t tmp_value;
    auto err_code = OnRetIntCommand("SISMEMBER", key, tmp_value, member);
    if (err_code == redis_err_code::ok)
    {
        res = (tmp_value == 1);
    }
    return err_code;
}

redis_err_code redis_module_impl::ZIncrBy(
    const std::string& key, const double& increment, const std::string& member, double& result_value) {
    std::string tmp_str;
    auto err_code = OnRetStrCommand("ZINCRBY", key, tmp_str, increment, member);
    if (err_code == redis_err_code::ok)
    {
        result_value = std::stod(tmp_str);
    }
    return err_code;
}

redis_err_code redis_module_impl::ZRange(const std::string& key, const int64_t& begin, const int64_t& end,
    value_vector<std::string>& res_list, bool with_score) {
    if (with_score)
    {
        return OnRetArrayCommand("ZRANGE", key, res_list, begin, end, "WITHSCORES");
    }
    return OnRetArrayCommand("ZRANGE", key, res_list, begin, end);
}

redis_err_code redis_module_impl::ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
    value_vector<std::string>& res_list, bool with_score) {
    if (with_score)
    {
        return OnRetArrayCommand("ZRANGEBYSCORE", key, res_list, min, max, "WITHSCORES");
    }
    return OnRetArrayCommand("ZRANGEBYSCORE", key, res_list, min, max);
}

redis_err_code redis_module_impl::ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
    const int64_t& offset, const uint32_t& count, value_vector<std::string>& res_list, bool with_score) {
    if (with_score)
    {
        return OnRetArrayCommand("ZRANGEBYSCORE", key, res_list, min, max, "WITHSCORES", "LIMIT", offset, count);
    }
    return OnRetArrayCommand("ZRANGEBYSCORE", key, res_list, min, max, "LIMIT", offset, count);
}

redis_err_code redis_module_impl::ZRevRange(const std::string& key, const std::int64_t& begin, const int64_t& end,
    value_vector<std::string>& res_list, bool with_score) {
    if (with_score)
    {
        return OnRetArrayCommand("ZREVRANGE", key, res_list, begin, end, "WITHSCORES");
    }
    return OnRetArrayCommand("ZREVRANGE", key, res_list, begin, end);
}

redis_err_code redis_module_impl::ZRevRangeByScore(const std::string& key, const std::string& min,
    const std::string& max, value_vector<std::string>& res_list, bool with_score) {
    if (with_score)
    {
        return OnRetArrayCommand("ZREVRANGEBYSCORE", key, res_list, min, max, "WITHSCORES");
    }
    return OnRetArrayCommand("ZREVRANGEBYSCORE", key, res_list, min, max);
}

redis_err_code redis_module_impl::ZRevRangeByScore(const std::string& key, const std::string& min,
    const std::string& max, const int64_t& offset, const uint32_t& count, value_vector<std::string>& res_list,
    bool with_score) {
    if (with_score)
    {
        return OnRetArrayCommand("ZREVRANGEBYSCORE", key, res_list, min, max, "WITHSCORES", "LIMIT", offset, count);
    }
    return OnRetArrayCommand("ZREVRANGEBYSCORE", key, res_list, min, max, "LIMIT", offset, count);
}

redis_err_code redis_module_impl::ZRank(const std::string& key, const std::string& member, int64_t& rank_res) {
    return OnRetIntCommand("ZRANK", key, rank_res, member);
}

redis_err_code redis_module_impl::ZRevRank(const std::string& key, const std::string& member, int64_t& rank_res) {
    return OnRetIntCommand("ZREVRANK", key, rank_res, member);
}

redis_err_code redis_module_impl::ZCard(const std::string& key, int64_t& count) {
    return OnRetIntCommand("ZCARD", key, count);
}

redis_err_code redis_module_impl::ZCount(
    const std::string& key, const std::string& min, const std::string& max, int64_t& count) {
    return OnRetIntCommand("ZCOUNT", key, count, min, max);
}

redis_err_code redis_module_impl::ZScore(const std::string& key, const std::string member, double& score) {
    std::string tmp_str;
    auto err_code = OnRetStrCommand("ZSCORE", key, tmp_str, member);
    if (err_code == redis_err_code::ok)
    {
        score = std::stod(tmp_str);
    }
    return err_code;
}

redis_err_code redis_module_impl::ZRemRangeByRank(
    const std::string& key, const int64_t& begin, const int64_t& end, int64_t& count) {
    return OnRetIntCommand("ZREMRANGEBYRANK", key, count, begin, end);
}

redis_err_code redis_module_impl::ZRemRangeByScore(
    const std::string& key, const std::string& min, const std::string& max, int64_t& count) {
    return OnRetIntCommand("ZREMRANGEBYSCORE", key, count, min, max);
}

redis_err_code redis_module_impl::HGetAll(const std::string& key, value_vector<std::string>& res_list) {
    return OnRetArrayCommand("HGETALL", key, res_list);
}

redis_err_code redis_module_impl::HLen(const std::string& key, int64_t& length) {
    return OnRetIntCommand("HLEN", key, length);
}

redis_err_code redis_module_impl::HSetNX(const std::string& key, const std::string& field, const std::string& value) {
    return OnRetNilCommand("HSETNX", key, field, value);
}

redis_err_code redis_module_impl::HExists(const std::string& key, const std::string& field, bool& exist) {
    int64_t tmp_value;
    auto err_code = OnRetIntCommand("HEXISTS", key, tmp_value, field);
    if (err_code == redis_err_code::ok)
    {
        exist = (tmp_value == 1);
    }
    return err_code;
}

redis_err_code redis_module_impl::HKeys(const std::string& key, value_vector<std::string>& fields) {
    return OnRetArrayCommand("HKEYS", key, fields);
}

redis_err_code redis_module_impl::HDel(const std::string& key, const std::string& field, bool& is_deleted) {
    int64_t tmp_value;
    auto err_code = OnRetIntCommand("HDEL", key, tmp_value, field);
    if (err_code == redis_err_code::ok)
    {
        is_deleted = (tmp_value == 1);
    }
    return err_code;
}

redis_err_code redis_module_impl::HIncrBy(
    const std::string& key, const std::string& field, const int64_t& increment, int64_t& result_value) {
    return OnRetIntCommand("HINCRBY", key, result_value, field, increment);
}

redis_err_code redis_module_impl::HIncrByFloat(
    const std::string& key, const std::string& field, const double& increment, double& result_value) {
    std::string tmp_str;
    auto err_code = OnRetStrCommand("HINCRBYFLOAT", key, tmp_str, field, increment);
    if (err_code == redis_err_code::ok)
    {
        result_value = std::stod(tmp_str);
    }
    return err_code;
}

redis_err_code redis_module_impl::HScan(const std::string& key, int64_t& ret_cursor,
    value_vector<std::string>& res_list, const int64_t& cursor, const std::string& pattern, const uint32_t& count) {
    auto c = redis_manager::instance()->GetConn();
    if (c == nullptr)
    {
        return redis_err_code::err;
    }
    auto conn = c->GetRedisConn();

    if (conn == nullptr)
    {
        return redis_err_code::err;
    }
    redis_buffer rx_buff;

    try
    {
        conn->write(bredis::single_command_t(
            "HSCAN", key, std::to_string(cursor), "MATCH", pattern, "COUNT", std::to_string(count)));
        auto result_markers = conn->read(rx_buff);
        redis_manager::instance()->ReleaseConn(c);
        rx_buff.consume(result_markers.consumed);
        auto err_code = IsOk(result_markers.result);
        if (err_code != redis_err_code::ok)
        {
            return err_code;
        }
        auto extract = boost::apply_visitor(bredis::extractor<iterator>(), result_markers.result);

        auto tmp_v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
        res_list.clear();
        for (auto iter = tmp_v.begin(); iter != tmp_v.end(); ++iter)
        {
            if (iter == tmp_v.begin())
            {
                ret_cursor = std::stoi(boost::get<bredis::extracts::string_t>(*iter).str);
            }
            else
            {
                auto recur_tmp_v = boost::get<bredis::extracts::array_holder_t>(*iter).elements;
                for (auto recur_iter : recur_tmp_v)
                {
                    res_list.emplace_back(boost::get<bredis::extracts::string_t>(recur_iter).str);
                }
            }
        }
        return err_code;
    }
    catch ([[maybe_unused]] boost::system::system_error& ec)
    {
        ARK_LOG_ERROR("Redis Connection Failed with Command: HSCAN and key: {}", key);
        return redis_err_code::err;
    }
}

redis_err_code redis_module_impl::Exists(const std::string& key, bool& exist) {
    int64_t tmp_value;
    auto err_code = OnRetIntCommand("EXISTS", key, tmp_value);
    if (err_code == redis_err_code::ok)
    {
        exist = (tmp_value == 1);
    }
    return err_code;
}

redis_err_code redis_module_impl::Del(const std::string& key) {
    return OnRetNilCommand("DEL", key);
}

redis_err_code redis_module_impl::IncrBy(const std::string& key, const int64_t& increment, int64_t& result) {
    return OnRetIntCommand("INCRBY", key, result, std::to_string(increment));
}

redis_err_code redis_module_impl::DecrBy(const std::string& key, const int64_t& decrement, int64_t& result) {
    return OnRetIntCommand("DECRBY", key, result, std::to_string(decrement));
}

redis_err_code redis_module_impl::Expire(const std::string& key, const int64_t& expire_time, bool& result) {
    int64_t tmp_value;
    auto err_code = OnRetIntCommand("EXPIRE", key, tmp_value);
    if (err_code == redis_err_code::ok)
    {
        result = (tmp_value == 1);
    }
    return err_code;
}

redis_err_code redis_module_impl::MGet(const std::vector<std::string>& commands, const std::string& command,
    const std::string& key, value_vector<std::string>& str_list) {
    return RetArrayVecCommand(command, key, str_list, commands);
}

redis_err_code redis_module_impl::LPush(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return RetIntVecCommand(command, key, length, commands);
}

redis_err_code redis_module_impl::RPush(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return RetIntVecCommand(command, key, length, commands);
}

redis_err_code redis_module_impl::SRem(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return RetIntVecCommand(command, key, length, commands);
}

redis_err_code redis_module_impl::SAdd(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return RetIntVecCommand(command, key, length, commands);
}

redis_err_code redis_module_impl::ZAdd(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return RetIntVecCommand(command, key, length, commands);
}

redis_err_code redis_module_impl::ZRem(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length) {
    return RetIntVecCommand(command, key, length, commands);
}

//Asynchronous Commands Implement
void redis_module_impl::Get(const std::string& key, RedisStrFunc func) {
    AsyncOnRetStrCommand("GET", key, func);
}

void redis_module_impl::Set(
    const std::string& key, const std::string& value, RedisNilFunc func, const uint64_t expire_time) {
    AsyncOnRetNilCommand("SET", key, func, value, expire_time);
}

void redis_module_impl::Incr(const std::string& key, RedisIntFunc func) {
    AsyncOnRetIntCommand("INCR", key, func);
}

void redis_module_impl::HSet(
    const std::string& key, const std::string& field, const std::string& value, RedisNilFunc func) {
    AsyncOnRetNilCommand("HSET", key, func, field, value);
}

void redis_module_impl::HGet(const std::string& key, const std::string& field, RedisStrFunc func) {
    AsyncOnRetStrCommand("HGET", key, func, field);
}

void redis_module_impl::SetNX(const std::string& key, const std::string& value, RedisNilFunc func) {
    AsyncOnRetNilCommand("SETNX", key, func, value);
}

void redis_module_impl::GetSet(const std::string& key, const std::string& value, RedisStrFunc func) {
    AsyncOnRetStrCommand("GETSET", key, func, value);
}

void redis_module_impl::LLen(const std::string& key, RedisIntFunc func) {
    AsyncOnRetIntCommand("LLEN", key, func);
}

void redis_module_impl::LRange(const std::string& key, const int64_t& begin, const int64_t& end, RedisArrayFunc func) {
    AsyncOnRetArrayCommand("LRANGE", key, func, begin, end);
}

void redis_module_impl::LPop(const std::string& key, RedisStrFunc func) {
    AsyncOnRetStrCommand("LPOP", key, func);
}

void redis_module_impl::RPop(const std::string& key, RedisStrFunc func) {
    AsyncOnRetStrCommand("RPOP", key, func);
}

void redis_module_impl::SCard(const std::string& key, RedisIntFunc func) {
    AsyncOnRetIntCommand("SCARD", key, func);
}

void redis_module_impl::SMembers(const std::string& key, RedisArrayFunc func) {
    AsyncOnRetArrayCommand("SMEMBERS", key, func);
}

void redis_module_impl::SRandMember(const std::string& key, const uint32_t& count, RedisArrayFunc func) {
    AsyncOnRetArrayCommand("SRANDMEMBER", key, func, count);
}

void redis_module_impl::SPop(const std::string& key, const uint32_t& count, RedisArrayFunc func) {
    AsyncOnRetArrayCommand("SPOP", key, func, count);
}

void redis_module_impl::SPop(const std::string& key, RedisStrFunc func) {
    AsyncOnRetStrCommand("SPOP", key, func);
}

void redis_module_impl::SIsMember(const std::string& key, const std::string& member, RedisIntFunc func) {
    AsyncOnRetIntCommand("SISMEMBER", key, func, member);
}

void redis_module_impl::ZIncrBy(
    const std::string& key, const double& increment, const std::string& member, RedisStrFunc func) {
    AsyncOnRetStrCommand("ZINCRBY", key, func, increment, member);
}

void redis_module_impl::ZRange(
    const std::string& key, const int64_t& begin, const int64_t& end, RedisArrayFunc func, bool with_score) {
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZRANGE", key, func, begin, end, "WITHSCORE");
    }
    else
    {
        AsyncOnRetArrayCommand("ZRANGE", key, func, begin, end);
    }
}

void redis_module_impl::ZRangeByScore(
    const std::string& key, const std::string& min, const std::string& max, RedisArrayFunc func, bool with_score) {
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZRANGEBYSCORE", key, func, min, max, "WITHSCORE");
    }
    else
    {
        AsyncOnRetArrayCommand("ZRANGEBYSCORE", key, func, min, max);
    }
}

void redis_module_impl::ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
    const int64_t& offset, const uint32_t& count, RedisArrayFunc func, bool with_score) {
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZRANGEBYSCORE", key, func, min, max, "WITHSCORE", "LIMIT", offset, count);
    }
    else
    {
        AsyncOnRetArrayCommand("ZRANGEBYSCORE", key, func, min, max, "LIMIT", offset, count);
    }
}

void redis_module_impl::ZRevRange(
    const std::string& key, const std::int64_t& begin, const int64_t& end, RedisArrayFunc func, bool with_score) {
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZREVRAMGE", key, func, begin, end, "WITHSCORE");
    }
    else
    {
        AsyncOnRetArrayCommand("ZRANGEBYSCORE", key, func, begin, end);
    }
}

void redis_module_impl::ZRevRangeByScore(
    const std::string& key, const std::string& min, const std::string& max, RedisArrayFunc func, bool with_score) {
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZREVRANGEBYSCORE", key, func, min, max, "WITHSCORES");
    }
    else
    {
        AsyncOnRetArrayCommand("ZREVRANGEBYSCORE", key, func, min, max);
    }
}

void redis_module_impl::ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
    const int64_t& offset, const uint32_t& count, RedisArrayFunc func, bool with_score) {
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZREVRANGEBYSCORE", key, func, min, max, "WITHSCORES", "LIMIT", offset, count);
    }
    else
    {
        AsyncOnRetArrayCommand("ZREVRANGEBYSCORE", key, func, min, max, "LIMIT", offset, count);
    }
}

void redis_module_impl::ZRank(const std::string& key, const std::string& member, RedisIntFunc func) {
    AsyncOnRetIntCommand("ZRANK", key, func, member);
}

void redis_module_impl::ZRevRank(const std::string& key, const std::string& member, RedisIntFunc func) {
    AsyncOnRetIntCommand("ZREVRANK", key, func, member);
}

void redis_module_impl::ZCard(const std::string& key, RedisIntFunc func) {
    AsyncOnRetIntCommand("ZCARD", key, func);
}

void redis_module_impl::ZCount(
    const std::string& key, const std::string& min, const std::string& max, RedisIntFunc func) {
    AsyncOnRetIntCommand("ZCOUNT", key, func, min, max);
}

void redis_module_impl::ZScore(const std::string& key, const std::string member, RedisStrFunc func) {
    AsyncOnRetStrCommand("ZSCORE", key, func, member);
}

void redis_module_impl::ZRemRangeByRank(
    const std::string& key, const int64_t& begin, const int64_t& end, RedisIntFunc func) {
    AsyncOnRetIntCommand("ZREMRANGEBYRANK", key, func, begin, end);
}

void redis_module_impl::ZRemRangeByScore(
    const std::string& key, const std::string& min, const std::string& max, RedisIntFunc func) {
    AsyncOnRetIntCommand("ZREMRANGEBYSCORE", key, func, min, max);
}

void redis_module_impl::HGetAll(const std::string& key, RedisArrayFunc func) {
    AsyncOnRetArrayCommand("HGETALL", key, func);
}

void redis_module_impl::HLen(const std::string& key, RedisIntFunc func) {
    AsyncOnRetIntCommand("HLEN", key, func);
}

void redis_module_impl::HSetNX(
    const std::string& key, const std::string& field, const std::string& value, RedisNilFunc func) {
    AsyncOnRetNilCommand("HSETNX", key, func, field, value);
}

void redis_module_impl::HExists(const std::string& key, const std::string& field, RedisIntFunc func) {
    AsyncOnRetIntCommand("HEXISTS", key, func, field);
}

void redis_module_impl::HKeys(const std::string& key, RedisArrayFunc func) {
    AsyncOnRetArrayCommand("HKEYS", key, func);
}

void redis_module_impl::HDel(const std::string& key, const std::string& field, RedisIntFunc func) {
    AsyncOnRetIntCommand("HDEL", key, func, field);
}

void redis_module_impl::HIncrBy(
    const std::string& key, const std::string& field, const int64_t& increment, RedisIntFunc func) {
    AsyncOnRetIntCommand("HINCRBY", key, func, field, increment);
}

void redis_module_impl::HIncrByFloat(
    const std::string& key, const std::string& field, const double& increment, RedisStrFunc func) {
    AsyncOnRetStrCommand("HINCRBYFLOAT", key, func, field, increment);
}

void redis_module_impl::HScan(const std::string& key,
    void (*HscanFunc)(const redis_err_code&, const int64_t&, const std::vector<std::string>&), const int64_t& cursor,
    const std::string& pattern, const uint32_t& count) {
    std::vector<std::string> result_vector;
    auto c = redis_manager::instance()->GetConn();
    if (c == nullptr)
    {
        HscanFunc(redis_err_code::disconnect, -1, result_vector);
    }
    auto conn = c->GetRedisConn();
    if (conn == nullptr)
    {
        HscanFunc(redis_err_code::disconnect, -1, result_vector);
    }

    auto io = c->GetIo();

    redis_buffer rx_buff, tx_buff;

    auto commands = bredis::single_command_t(
        "HSCAN", key, std::to_string(cursor), "MATCH", pattern, "COUNT", std::to_string(count));

    conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec)
        {
            redis_manager::instance()->ReleaseConn(c);
            HscanFunc(redis_err_code::disconnect, -1, result_vector);
            ARK_LOG_ERROR("Redis Connection Failed with Command: HSCAN and key: {}", key);
            return;
        }
        tx_buff.consume(bytes_transferred);
        conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
            if (ec)
            {
                redis_manager::instance()->ReleaseConn(c);
                HscanFunc(redis_err_code::disconnect, -1, result_vector);
                ARK_LOG_ERROR("Redis Connection Failed with Command: HSCAN and key: {}", key);
                return;
            }
            redis_manager::instance()->ReleaseConn(c);
            rx_buff.consume(r.consumed);
            auto err_code = IsOk(r.result);
            if (err_code != redis_err_code::ok)
            {
                HscanFunc(err_code, -1, result_vector);
                return;
            }
            auto extract = boost::apply_visitor(bredis::extractor<iterator>(), r.result);
            auto& v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
            int64_t new_cursor;
            for (auto iter = v.begin(); iter != v.end(); ++iter)
            {
                if (iter == v.begin())
                {
                    new_cursor = std::stoi(boost::get<bredis::extracts::string_t>(*iter).str);
                }
                else
                {
                    auto& tmp_v = boost::get<bredis::extracts::array_holder_t>(*iter).elements;
                    for (auto& tmp_iter : tmp_v)
                    {
                        result_vector.emplace_back(boost::get<bredis::extracts::string_t>(tmp_iter).str);
                    }
                }
            }
            HscanFunc(err_code, new_cursor, result_vector);
        });
    });
    io.get().poll();
}

void redis_module_impl::Exists(const std::string& key, RedisIntFunc func) {
    AsyncOnRetIntCommand("EXISTS", key, func);
}

void redis_module_impl::Del(const std::string& key, RedisNilFunc func) {
    AsyncOnRetNilCommand("DEL", key, func);
}

void redis_module_impl::IncrBy(const std::string& key, const int64_t& increment, RedisIntFunc func) {
    AsyncOnRetIntCommand("INCRBY", key, func, increment);
}

void redis_module_impl::DecrBy(const std::string& key, const int64_t& decrement, RedisIntFunc func) {
    AsyncOnRetIntCommand("DECRBY", key, func, decrement);
}

void redis_module_impl::Expire(const std::string& key, const int64_t& expire_time, RedisIntFunc func) {
    AsyncOnRetIntCommand("EXPIRE", key, func, expire_time);
}

void redis_module_impl::AsynMSet(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisNilFunc func) {
    AsynRetNilVecCommand(command, key, func, commands);
}

void redis_module_impl::AsynMGet(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisArrayFunc func) {
    AsynRetArrayVecCommand(command, key, func, commands);
}

void redis_module_impl::AsynLPush(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func) {
    AsynRetIntVecCommand(command, key, func, commands);
}

void redis_module_impl::AsynRPush(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func) {
    AsynRetIntVecCommand(command, key, func, commands);
}

void redis_module_impl::AsynSRem(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func) {
    AsynRetIntVecCommand(command, key, func, commands);
}

void redis_module_impl::AsynSAdd(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func) {
    AsynRetIntVecCommand(command, key, func, commands);
}

void redis_module_impl::AsynZAdd(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func) {
    AsynRetIntVecCommand(command, key, func, commands);
}

void redis_module_impl::AsynZRem(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func) {
    AsynRetIntVecCommand(command, key, func, commands);
}

redis_err_code redis_module_impl::RetNilVecCommand(
    const std::string& command, const std::string& key, const std::vector<std::string>& args) {
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
        conn->write(bredis::single_command_t(args.begin(), args.end()));
        auto result_markers = conn->read(rx_buff);
        redis_manager::instance()->ReleaseConn(c);
        auto err_code = IsOk(result_markers.result);
        rx_buff.consume(result_markers.consumed);
        return err_code;
    }
    catch ([[maybe_unused]] boost::system::system_error& ec)
    {
        if (!key.empty())
        {
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
        }
        else
        {
            std::stringstream tmp;
            for (auto iter : args)
            {
                tmp << iter << " ";
            }
            ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
        }
        return redis_err_code::disconnect;
    }
}

redis_err_code redis_module_impl::RetIntVecCommand(
    const std::string& command, const std::string& key, int64_t& value, const std::vector<std::string>& args) {
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
        conn->write(bredis::single_command_t(args.begin(), args.end()));
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
        if (!key.empty())
        {
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
        }
        else
        {
            std::stringstream tmp;
            for (auto iter : args)
            {
                tmp << iter << " ";
            }
            ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
        }
        return redis_err_code::disconnect;
    }
}

redis_err_code redis_module_impl::RetStrVecCommand(
    const std::string& command, const std::string& key, std::string& value, const std::vector<std::string>& args) {
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
        conn->write(bredis::single_command_t(args.begin(), args.end()));

        auto result_markers = conn->read(rx_buff);
        redis_manager::instance()->ReleaseConn(c);
        ;
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
        if (!key.empty())
        {
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
        }
        else
        {
            std::stringstream tmp;
            for (auto iter : args)
            {
                tmp << iter << " ";
            }
            ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
        }
        return redis_err_code::disconnect;
    }
}

redis_err_code redis_module_impl::RetArrayVecCommand(const std::string& command, const std::string& key,
    std::vector<std::string>& value, const std::vector<std::string>& args) {
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
        conn->write(bredis::single_command_t(args.begin(), args.end()));

        auto result_markers = conn->read(rx_buff);
        redis_manager::instance()->ReleaseConn(c);
        rx_buff.consume(result_markers.consumed);
        auto err_code = IsOk(result_markers.result);
        if (err_code != redis_err_code::ok)
        {
            return err_code;
        }
        auto extract = boost::apply_visitor(bredis::extractor<iterator>(), result_markers.result);

        auto tmp_v = boost::get<bredis::extracts::array_holder_t>(extract).elements;
        value.clear();
        for (auto iter : tmp_v)
        {
            value.emplace_back(boost::get<bredis::extracts::string_t>(iter).str);
        }
        return err_code;
    }
    catch ([[maybe_unused]] boost::system::system_error& ec)
    {
        if (!key.empty())
        {
            ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
        }
        else
        {
            std::stringstream tmp;
            for (auto iter : args)
            {
                tmp << iter << " ";
            }
            ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
        }
        return redis_err_code::disconnect;
    }
}

void redis_module_impl::AsynRetNilVecCommand(
    const std::string& command, const std::string& key, RedisNilFunc func, const std::vector<std::string>& args) {
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

    conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()),
        [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->ReleaseConn(c);
                func(redis_err_code::disconnect);
                if (!key.empty())
                {
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                }
                else
                {
                    std::stringstream tmp;
                    for (auto iter : args)
                    {
                        tmp << iter << " ";
                    }
                    ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                }
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->ReleaseConn(c);
                    func(redis_err_code::disconnect);
                    if (!key.empty())
                    {
                        ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    }
                    else
                    {
                        std::stringstream tmp;
                        for (auto iter : args)
                        {
                            tmp << iter << " ";
                        }
                        ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                    }
                    return;
                }
                redis_manager::instance()->ReleaseConn(c);
                rx_buff.consume(r.consumed);
                auto err_code = IsOk(r.result);
                func(err_code);
                return;
            });
        });
    io.get().poll();
}

void redis_module_impl::AsynRetIntVecCommand(
    const std::string& command, const std::string& key, RedisIntFunc func, const std::vector<std::string>& args) {
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

    conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()),
        [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->ReleaseConn(c);
                func(redis_err_code::disconnect, -1);
                if (!key.empty())
                {
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                }
                else
                {
                    std::stringstream tmp;
                    for (auto iter : args)
                    {
                        tmp << iter << " ";
                    }
                    ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                }
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->ReleaseConn(c);
                    func(redis_err_code::disconnect, -1);
                    if (!key.empty())
                    {
                        ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    }
                    else
                    {
                        std::stringstream tmp;
                        for (auto iter : args)
                        {
                            tmp << iter << " ";
                        }
                        ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                    }
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
                return;
            });
        });
    io.get().poll();
}

void redis_module_impl::AsynRetStrVecCommand(
    const std::string& command, const std::string& key, RedisStrFunc func, const std::vector<std::string>& args) {
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
    conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()),
        [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->ReleaseConn(c);
                func(redis_err_code::disconnect, std::string());
                if (!key.empty())
                {
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                }
                else
                {
                    std::stringstream tmp;
                    for (auto iter : args)
                    {
                        tmp << iter << " ";
                    }
                    ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                }
                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->ReleaseConn(c);
                    func(redis_err_code::disconnect, std::string());
                    if (!key.empty())
                    {
                        ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    }
                    else
                    {
                        std::stringstream tmp;
                        for (auto iter : args)
                        {
                            tmp << iter << " ";
                        }
                        ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                    }
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
                return;
            });
        });
    io.get().poll();
}

void redis_module_impl::AsynRetArrayVecCommand(
    const std::string& command, const std::string& key, RedisArrayFunc func, const std::vector<std::string>& args) {
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

    conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()),
        [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec)
            {
                redis_manager::instance()->ReleaseConn(c);
                func(redis_err_code::disconnect, result_vector);
                if (!key.empty())
                {
                    ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                }
                else
                {
                    std::stringstream tmp;
                    for (auto iter : args)
                    {
                        tmp << iter << " ";
                    }
                    ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                }

                return;
            }
            tx_buff.consume(bytes_transferred);
            conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
                if (ec)
                {
                    redis_manager::instance()->ReleaseConn(c);
                    func(redis_err_code::disconnect, result_vector);
                    if (!key.empty())
                    {
                        ARK_LOG_ERROR("Redis Connection Failed with Command: {} and key: {}", command, key);
                    }
                    else
                    {
                        std::stringstream tmp;
                        for (auto iter : args)
                        {
                            tmp << iter << " ";
                        }
                        ARK_LOG_ERROR("Redis Connection Failed with Commands: {}", tmp.str());
                    }
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
                return;
            });
        });
    io.get().poll();
}

} // namespace ark
