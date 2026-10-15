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

#include "ark/container/vector.hpp"
#include "ark/module.hpp"

namespace ark {

enum class redis_err_code
{
    ok = 0,
    err = 1,
    miss = 2,
    disconnect = 3
};
typedef void (*RedisIntFunc)(const redis_err_code&, const int64_t&);
typedef void (*RedisNilFunc)(const redis_err_code&);
typedef void (*RedisStrFunc)(const redis_err_code&, const std::string&);
typedef void (*RedisArrayFunc)(const redis_err_code&, const std::vector<std::string>&);

class redis_module : public module
{
public:
    // Synchronous commands
    virtual redis_err_code Get(const std::string& key, OUT std::string& value) = 0;
    virtual redis_err_code Set(const std::string& key, const std::string& value, const uint64_t expire_time = 0) = 0;
    virtual redis_err_code Incr(const std::string& key, OUT int64_t& value) = 0;
    virtual redis_err_code HSet(const std::string& key, const std::string& field, const std::string& value) = 0;
    virtual redis_err_code HGet(const std::string& key, const std::string& field, OUT std::string& value) = 0;
    virtual redis_err_code SetNX(const std::string& key, const std::string& value) = 0;
    virtual redis_err_code GetSet(const std::string& key, const std::string& value, OUT std::string& str) = 0;
    virtual redis_err_code LLen(const std::string& key, OUT int64_t& length) = 0;
    virtual redis_err_code LRange(
        const std::string& key, const int64_t& begin, const int64_t& end, OUT value_vector<std::string>& res_list) = 0;
    virtual redis_err_code LPop(const std::string& key, OUT std::string& value) = 0;
    virtual redis_err_code RPop(const std::string& key, OUT std::string& value) = 0;
    virtual redis_err_code SCard(const std::string& key, OUT int64_t& value) = 0;
    virtual redis_err_code SMembers(const std::string& key, OUT value_vector<std::string>& res_liste) = 0;
    virtual redis_err_code SRandMember(
        const std::string& key, const uint32_t& count, OUT value_vector<std::string>& res_list) = 0;
    virtual redis_err_code SPop(
        const std::string& key, const uint32_t& count, OUT value_vector<std::string>& res_list) = 0; // For redis 3.2+
    virtual redis_err_code SPop(const std::string& key, OUT std::string& value) = 0;
    virtual redis_err_code SIsMember(const std::string& key, const std::string& member, OUT bool& res) = 0;
    virtual redis_err_code ZIncrBy(
        const std::string& key, const double& increment, const std::string& member, OUT double& result_value) = 0;
    virtual redis_err_code ZRange(const std::string& key, const int64_t& begin, const int64_t& end,
        OUT value_vector<std::string>& res_list, bool with_score = false) = 0;
    virtual redis_err_code ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        OUT value_vector<std::string>& res_list, bool with_score = false) = 0;
    virtual redis_err_code ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, OUT value_vector<std::string>& res_list,
        bool with_score = false) = 0; //with limit
    virtual redis_err_code ZRevRange(const std::string& key, const std::int64_t& begin, const int64_t& end,
        OUT value_vector<std::string>& res_list, bool with_score = false) = 0;
    virtual redis_err_code ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        OUT value_vector<std::string>& res_list, bool with_score = false) = 0;
    virtual redis_err_code ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, OUT value_vector<std::string>& res_list,
        bool with_score = false) = 0; // with limit
    virtual redis_err_code ZRank(const std::string& key, const std::string& member, OUT int64_t& rank_res) = 0;
    virtual redis_err_code ZRevRank(const std::string& key, const std::string& member, OUT int64_t& rank_res) = 0;
    virtual redis_err_code ZCard(const std::string& key, OUT int64_t& count) = 0;
    virtual redis_err_code ZCount(
        const std::string& key, const std::string& min, const std::string& max, OUT int64_t& count) = 0;
    virtual redis_err_code ZScore(const std::string& key, const std::string member, OUT double& score) = 0;
    virtual redis_err_code ZRemRangeByRank(
        const std::string& key, const int64_t& begin, const int64_t& end, OUT int64_t& count) = 0;
    virtual redis_err_code ZRemRangeByScore(
        const std::string& key, const std::string& min, const std::string& max, OUT int64_t& count) = 0;
    virtual redis_err_code HGetAll(const std::string& key, OUT value_vector<std::string>& res_list) = 0;
    virtual redis_err_code HLen(const std::string& key, OUT int64_t& length) = 0;
    virtual redis_err_code HSetNX(const std::string& key, const std::string& field, const std::string& value) = 0;
    virtual redis_err_code HExists(const std::string& key, const std::string& field, OUT bool& exist) = 0;
    virtual redis_err_code HKeys(const std::string& key, OUT value_vector<std::string>& fields) = 0;
    virtual redis_err_code HDel(const std::string& key, const std::string& field, OUT bool& is_deleted) = 0;
    virtual redis_err_code HIncrBy(
        const std::string& key, const std::string& field, const int64_t& increment, OUT int64_t& result_valuee) = 0;
    virtual redis_err_code HIncrByFloat(
        const std::string& key, const std::string& field, const double& increment, double& result_value) = 0;
    virtual redis_err_code HScan(const std::string& key, OUT int64_t& ret_cursor,
        OUT value_vector<std::string>& res_list, const int64_t& cursor, const std::string& pattern,
        const uint32_t& count = 10) = 0;
    virtual redis_err_code Exists(const std::string& key, OUT bool& exist) = 0;
    virtual redis_err_code Del(const std::string& key) = 0;
    virtual redis_err_code IncrBy(const std::string& key, const int64_t& increment, OUT int64_t& result) = 0;
    virtual redis_err_code DecrBy(const std::string& key, const int64_t& decrement, OUT int64_t& result) = 0;
    virtual redis_err_code Expire(const std::string& key, const int64_t& expire_time, OUT bool& result) = 0;

    template<typename... Args>
    redis_err_code MSetArgs(const Args... args) {
        std::vector<std::string> params{"MSET", std::forward<std::string>(Convert<Args>(args))...};
        return MSet(params, "MSET", {});
    }

    virtual redis_err_code MSet(
        const std::vector<std::string>& commands, const std::string& command, const std::string& key) = 0;

    template<typename... Args>
    redis_err_code MGetArgs(value_vector<std::string>& str_list, const Args&... args) {
        std::vector<std::string> params{"MGET", std::forward<std::string>(Convert<Args>(args))...};
        return MGet(params, "MGET", {}, str_list);
    }

    virtual redis_err_code MGet(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, value_vector<std::string>& str_list) = 0;

    template<typename... Args>
    redis_err_code LPushArgs(const std::string& key, int64_t& length, const Args&... args) {
        std::vector<std::string> params{"LPUSH", key, std::forward<std::string>(Convert<Args>(args))...};
        return LPush(params, "LPUSH", key, length);
    }

    virtual redis_err_code LPush(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    template<typename... Args>
    redis_err_code RPushArgs(const std::string& key, int64_t& length, const Args&... args) {
        std::vector<std::string> params{"RPUSH", key, std::forward<std::string>(Convert<Args>(args))...};
        return RPush(params, "RPUSH", key, length);
    }

    virtual redis_err_code RPush(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    template<typename... Args>
    redis_err_code SRemArgs(const std::string& key, int64_t& length, const Args&... args) {
        std::vector<std::string> params{"SREM", key, std::forward<std::string>(Convert<Args>(args))...};
        return SRem(params, "SREM", key, length);
    }

    virtual redis_err_code SRem(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    template<typename... Args>
    redis_err_code SAddArgs(const std::string& key, int64_t& length, const Args&... args) {
        std::vector<std::string> params{"SADD", key, std::forward<std::string>(Convert<Args>(args))...};
        return SAdd(params, "SADD", key, length);
    }

    virtual redis_err_code SAdd(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    template<typename... Args>
    redis_err_code ZAddArgs(const std::string& key, int64_t& length, const Args&... args) {
        std::vector<std::string> params{"ZADD", key, std::forward<std::string>(Convert<Args>(args))...};
        return ZAdd(params, "ZADD", key, length);
    }

    virtual redis_err_code ZAdd(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    template<typename... Args>
    redis_err_code ZRemArgs(
        const std::string& key, int64_t& length, const Args&... args) // if Redis < 2.4, args number can only be 1
    {
        std::vector<std::string> params{"ZREM", key, std::forward<std::string>(Convert<Args>(args))...};
        return ZRem(params, "ZREM", key, length);
    }

    virtual redis_err_code ZRem(const std::vector<std::string>& commands, const std::string& command,
        const std::string& key, int64_t& length) = 0;

    //Asynchronous commands
    virtual void Get(const std::string& key, RedisStrFunc func) = 0;
    virtual void Set(
        const std::string& key, const std::string& value, RedisNilFunc func, const uint64_t expire_time = 0) = 0;
    virtual void Incr(const std::string& key, RedisIntFunc func) = 0;
    virtual void HSet(
        const std::string& key, const std::string& field, const std::string& value, RedisNilFunc func) = 0;
    virtual void HGet(const std::string& key, const std::string& field, RedisStrFunc func) = 0;
    virtual void SetNX(const std::string& key, const std::string& value, RedisNilFunc func) = 0;
    virtual void GetSet(const std::string& key, const std::string& value, RedisStrFunc func) = 0;
    virtual void LLen(const std::string& key, RedisIntFunc func) = 0;
    virtual void LRange(const std::string& key, const int64_t& begin, const int64_t& end, RedisArrayFunc func) = 0;
    virtual void LPop(const std::string& key, RedisStrFunc func) = 0;
    virtual void RPop(const std::string& key, RedisStrFunc func) = 0;
    virtual void SCard(const std::string& key, RedisIntFunc func) = 0;
    virtual void SMembers(const std::string& key, RedisArrayFunc func) = 0;
    virtual void SRandMember(const std::string& key, const uint32_t& count, RedisArrayFunc func) = 0;
    virtual void SPop(const std::string& key, const uint32_t& count, RedisArrayFunc func) = 0; // For redis 3.2+
    virtual void SPop(const std::string& key, RedisStrFunc func) = 0;
    virtual void SIsMember(const std::string& key, const std::string& member, RedisIntFunc func) = 0;
    virtual void ZIncrBy(
        const std::string& key, const double& increment, const std::string& member, RedisStrFunc func) = 0;
    virtual void ZRange(const std::string& key, const int64_t& begin, const int64_t& end, RedisArrayFunc func,
        bool with_score = false) = 0;
    virtual void ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        RedisArrayFunc func, bool with_score = false) = 0;
    virtual void ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, RedisArrayFunc func,
        bool with_score = false) = 0; //with limit
    virtual void ZRevRange(const std::string& key, const std::int64_t& begin, const int64_t& end, RedisArrayFunc func,
        bool with_score = false) = 0;
    virtual void ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        RedisArrayFunc func, bool with_score = false) = 0;
    virtual void ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
        const int64_t& offset, const uint32_t& count, RedisArrayFunc func,
        bool with_score = false) = 0; // with limit
    virtual void ZRank(const std::string& key, const std::string& member, RedisIntFunc func) = 0;
    virtual void ZRevRank(const std::string& key, const std::string& member, RedisIntFunc func) = 0;
    virtual void ZCard(const std::string& key, RedisIntFunc func) = 0;
    virtual void ZCount(const std::string& key, const std::string& min, const std::string& max, RedisIntFunc func) = 0;
    virtual void ZScore(const std::string& key, const std::string member, RedisStrFunc func) = 0;
    virtual void ZRemRangeByRank(
        const std::string& key, const int64_t& begin, const int64_t& end, RedisIntFunc func) = 0;
    virtual void ZRemRangeByScore(
        const std::string& key, const std::string& min, const std::string& max, RedisIntFunc func) = 0;
    virtual void HGetAll(const std::string& key, RedisArrayFunc func) = 0;
    virtual void HLen(const std::string& key, RedisIntFunc func) = 0;
    virtual void HSetNX(
        const std::string& key, const std::string& field, const std::string& value, RedisNilFunc func) = 0;
    virtual void HExists(const std::string& key, const std::string& field, RedisIntFunc func) = 0;
    virtual void HKeys(const std::string& key, RedisArrayFunc func) = 0;
    virtual void HDel(const std::string& key, const std::string& field, RedisIntFunc func) = 0;
    virtual void HIncrBy(
        const std::string& key, const std::string& field, const int64_t& increment, RedisIntFunc func) = 0;
    virtual void HIncrByFloat(
        const std::string& key, const std::string& field, const double& increment, RedisStrFunc func) = 0;
    virtual void HScan(const std::string& key,
        void (*HscanFunc)(const redis_err_code&, const int64_t&, const std::vector<std::string>&),
        const int64_t& cursor, const std::string& pattern, const uint32_t& count = 10) = 0;
    virtual void Exists(const std::string& key, RedisIntFunc func) = 0;
    virtual void Del(const std::string& key, RedisNilFunc func) = 0;
    virtual void IncrBy(const std::string& key, const int64_t& increment, RedisIntFunc func) = 0;
    virtual void DecrBy(const std::string& key, const int64_t& decrement, RedisIntFunc func) = 0;
    virtual void Expire(const std::string& key, const int64_t& expire_time, RedisIntFunc func) = 0;

    template<typename... Args, typename HandleFunc>
    void AsynMSetArgs(HandleFunc func, const Args&... args) {
        std::vector<std::string> params{"MSET", std::forward<std::string>(Convert<Args>(args))...};
        AsynMSet(params, "MSET", {}, func);
    }

    virtual void AsynMSet(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisNilFunc func) = 0;

    template<typename... Args, typename HandleFunc>
    void AsynMGetArgs(HandleFunc func, const Args&... args) {
        std::vector<std::string> params{"MGET", std::forward<std::string>(Convert<Args>(args))...};
        AsynMGet(params, "MGET", {}, func);
    }

    virtual void AsynMGet(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisArrayFunc func) = 0;

    template<typename... Args, typename HandleFunc>
    void AsynLPushArgs(const std::string& key, HandleFunc func, const Args&... args) {
        std::vector<std::string> params{"LPUSH", key, std::forward<std::string>(Convert<Args>(args))...};
        AsynLPush(params, "LPUSH", key, func);
    }

    virtual void AsynLPush(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func) = 0;

    template<typename... Args, typename HandleFunc>
    void AsynRPushArgs(const std::string& key, HandleFunc func, const Args&... args) {
        std::vector<std::string> params{"RPUSH", key, std::forward<std::string>(Convert<Args>(args))...};
        AsynRPush(params, "RPUSH", key, func);
    }

    virtual void AsynRPush(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func) = 0;

    template<typename... Args, typename HandleFunc>
    void AsynSRemArgs(const std::string& key, HandleFunc func, const Args&... args) {
        std::vector<std::string> params{"SREM", key, std::forward<std::string>(Convert<Args>(args))...};
        AsynSRem(params, "SREM", key, func);
    }

    virtual void AsynSRem(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func) = 0;

    template<typename... Args, typename HandleFunc>
    void AsynSAddArgs(const std::string& key, HandleFunc func, const Args&... args) {
        std::vector<std::string> params{"SADD", key, std::forward<std::string>(Convert<Args>(args))...};
        AsynSAdd(params, "SADD", key, func);
    }

    virtual void AsynSAdd(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func) = 0;

    template<typename... Args, typename HandleFunc>
    void AsynZAddArgs(const std::string& key, HandleFunc func, const Args&... args) {
        std::vector<std::string> params{"ZADD", key, std::forward<std::string>(Convert<Args>(args))...};
        AsynZAdd(params, "ZADD", key, func);
    }

    virtual void AsynZAdd(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func) = 0;

    template<typename... Args, typename HandleFunc>
    void AsynZRemArgs(
        const std::string& key, HandleFunc func, const Args&... args) // if Redis < 2.4, args number can only be 1
    {
        std::vector<std::string> params{"ZREM", key, std::forward<std::string>(Convert<Args>(args))...};
        AsynZRem(params, "ZREM", key, func);
    }

    virtual void AsynZRem(const std::vector<std::string>& commands, const std::string& command, const std::string& key,
        RedisIntFunc func) = 0;

    template<typename T>
    using true_type = std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, const T&>;

    template<typename T>
    using false_type = std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::string>, const T>;

    template<typename T>
    const std::string& Convert(true_type<T> arg) {
        return arg;
    }

    template<typename T>
    std::string Convert(std::enable_if_t<std::is_same_v<std::decay_t<false_type<T>>, const char*>, const char*> arg) {
        return std::string(arg);
    }

    template<typename T>
    std::string Convert(
        std::enable_if_t<!std::is_same_v<std::decay_t<false_type<T>>, const char*>, const std::decay_t<false_type<T>>>
            arg) {
        return std::to_string(arg);
    }

    virtual bool Connect() = 0;
    virtual bool Reconnect() = 0;
    virtual bool Disconnect() = 0;
};

} // namespace ark