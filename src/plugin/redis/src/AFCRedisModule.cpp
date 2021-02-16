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
#include "redis/include/AFCRedisModule.hpp"
#include "redis/include/AFRedisPlugin.hpp"
#include "base/AFEnum.hpp"
#include "base/AFDefine.hpp"
#include "base/AFXml.hpp"
namespace ark{

bool AFCRedisModule::Init()
{
    if (!LoadConfig())
    {
        return false;
    }
    AFRedisManager::instance()->CreatePool(pool_size_, ip_, port_);;
    return Connect();
}

bool AFCRedisModule::LoadConfig()
{
    ///load config
    AFXml xml(AFRedisPlugin::GetPluginConf());
    auto root = xml.GetRootNode();
    ARK_ASSERT_RET_VAL(root.IsValid(), false);

    std::unordered_map<std::string, std::string> key_value_list;
    for (auto set = root.FindNode("set"); set.IsValid(); set.NextNode())
    {
        auto name = set.GetString("name");
        auto value = set.GetString("value");
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
    port_ = AFMisc::FromString<uint16_t>(func("Port"));
    pool_size_ = AFMisc::FromString<uint32_t>(func("PoolSize"));
    auth_keys_ = func("Password");
    return true;
}

bool AFCRedisModule::Connect()
{
    auto all_conn = AFRedisManager::instance()->GetAllConn();
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

bool AFCRedisModule::AuthPwd(const std::string& password, AFRedisConn* c)
{
    if (password.empty())
	{
		return true;
	}

	auto conn = c->GetRedisConn();
	if (conn == nullptr)
	{
		return false;
	}
		
	Buffer rx_buff;
	try
	{
		conn->write(bredis::single_command_t("AUTH", password));
		auto result_markers = conn->read(rx_buff);
		auto err_code = IsOk(result_markers.result);
		rx_buff.consume(result_markers.consumed);
		if (err_code != RedisErrCode::ok)
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

bool AFCRedisModule::Reconnect()
{
	AFRedisManager::instance()->RemovePool();
	AFRedisManager::instance()->Reconnect();
	return true;
}

bool AFCRedisModule::Disconnect()
{
    AFRedisManager::instance()->RemovePool();
    return true;
}

//Synchronous Commands Implement

RedisErrCode AFCRedisModule::Get(const std::string& key, std::string& value)
{
    return OnRetStrCommand("GET", key, value);
}

RedisErrCode AFCRedisModule::Set(const std::string& key, const std::string& value, const uint64_t expire_time)
{
    if (expire_time > 0)
	{
		return OnRetNilCommand("SETEX", key, expire_time, value);
	}
	else
	{
		return OnRetNilCommand("SET", key, value);
	}
}

RedisErrCode AFCRedisModule::SetNX(const std::string& key, const std::string& value)
{
	return OnRetNilCommand("SETNX", key, value);
}

RedisErrCode AFCRedisModule::HSet(const std::string& key, const std::string& field, const std::string& value)
{
	return OnRetNilCommand("HSET", key, field, value);
}

RedisErrCode AFCRedisModule::GetSet(const std::string& key, const std::string& value, std::string& str)
{
	return OnRetStrCommand("GETSET", key, str, value);
}

RedisErrCode AFCRedisModule::HGet(const std::string& key, const std::string& field, std::string& value)
{
	return OnRetStrCommand("HGET", key, value, field);
}

RedisErrCode AFCRedisModule::MSet(const std::vector<std::string>& commands, const std::string& command, const std::string& key)
{
    return RetNilVecCommand(command, key, commands);
}

RedisErrCode AFCRedisModule::Incr(const std::string& key, int64_t& value)
{
	return OnRetIntCommand("INCR", key, value);
}

RedisErrCode AFCRedisModule::LLen(const std::string& key, int64_t& length)
{
	return OnRetIntCommand("LLEN", key, length);
}

RedisErrCode AFCRedisModule::LRange(const std::string& key, const int64_t& begin, const int64_t& end, AFVector<std::string>& res_list)
{
	return OnRetArrayCommand("LRANGE", key, res_list, begin, end);
}

RedisErrCode AFCRedisModule::LPop(const std::string& key, std::string& value)
{
	return OnRetStrCommand("LPOP", key, value);
}

RedisErrCode AFCRedisModule::RPop(const std::string& key, std::string& value)
{
	return OnRetStrCommand("RPOP", key, value);
}

RedisErrCode AFCRedisModule::SCard(const std::string& key, std::int64_t& value)
{
	return OnRetIntCommand("SCARD", key, value);
}

RedisErrCode AFCRedisModule::SMembers(const std::string& key, AFVector<std::string>& res_list)
{
	return OnRetArrayCommand("SMEMBERS", key, res_list);
}

RedisErrCode AFCRedisModule::SRandMember(const std::string& key, const uint32_t& count, AFVector<std::string>& res_list)
{
	return OnRetArrayCommand("SRANDMEMBER", key, res_list, count);
}

RedisErrCode AFCRedisModule::SPop(const std::string& key, const uint32_t& count, AFVector<std::string>& res_list)
{
	return OnRetArrayCommand("SPOP", key, res_list, count);
}

RedisErrCode AFCRedisModule::SPop(const std::string& key, std::string& value)
{
	return OnRetStrCommand("SPOP", key, value);
}

RedisErrCode AFCRedisModule::SIsMember(const std::string& key, const std::string& member, bool& res)
{
	int64_t tmp_value;
	auto err_code = OnRetIntCommand("SISMEMBER", key, tmp_value, member);
	if (err_code == RedisErrCode::ok)
	{
		res = (tmp_value == 1);
	}
	return err_code;
}

RedisErrCode AFCRedisModule::ZIncrBy(const std::string& key, const double& increment, const std::string& member, double& result_value)
{
	std::string tmp_str;
	auto err_code = OnRetStrCommand("ZINCRBY", key, tmp_str, increment, member);
	if (err_code == RedisErrCode::ok)
	{
		result_value = std::stod(tmp_str);
	}
	return err_code;
}

RedisErrCode AFCRedisModule::ZRange(const std::string& key, const int64_t& begin, const int64_t& end, AFVector<std::string>& res_list, bool with_score)
{
	if (with_score)
	{
		return OnRetArrayCommand("ZRANGE", key, res_list, begin, end, "WITHSCORES");
	}
	return OnRetArrayCommand("ZRANGE", key, res_list, begin, end);
}

RedisErrCode AFCRedisModule::ZRangeByScore(const std::string& key, const std::string& min, const std::string& max, AFVector<std::string>& res_list, bool with_score)
{
	if (with_score)
	{
		return OnRetArrayCommand("ZRANGEBYSCORE", key, res_list, min, max, "WITHSCORES");
	}
	return OnRetArrayCommand("ZRANGEBYSCORE", key, res_list, min, max);
}

RedisErrCode AFCRedisModule::ZRangeByScore(const std::string& key, const std::string& min, const std::string& max, const int64_t& offset, const uint32_t& count, AFVector<std::string>& res_list, bool with_score)
{
	if (with_score)
	{
		return OnRetArrayCommand("ZRANGEBYSCORE", key, res_list, min, max, "WITHSCORES", "LIMIT", offset, count);
	}
	return OnRetArrayCommand("ZRANGEBYSCORE", key, res_list, min, max, "LIMIT", offset, count);
}

RedisErrCode AFCRedisModule::ZRevRange(const std::string& key, const std::int64_t& begin, const int64_t& end, AFVector<std::string>& res_list, bool with_score)
{
	if (with_score)
	{
		return OnRetArrayCommand("ZREVRANGE", key, res_list, begin, end, "WITHSCORES");
	}
	return OnRetArrayCommand("ZREVRANGE", key, res_list, begin, end);
}

RedisErrCode AFCRedisModule::ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max, AFVector<std::string>& res_list, bool with_score)
{
	if (with_score)
	{
		return OnRetArrayCommand("ZREVRANGEBYSCORE", key, res_list, min, max, "WITHSCORES");
	}
	return OnRetArrayCommand("ZREVRANGEBYSCORE", key, res_list, min, max);
}

RedisErrCode AFCRedisModule::ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max, const int64_t& offset, const uint32_t& count, AFVector<std::string>& res_list, bool with_score)
{
	if (with_score)
	{
		return OnRetArrayCommand("ZREVRANGEBYSCORE", key, res_list, min, max, "WITHSCORES", "LIMIT", offset, count);
	}
	return OnRetArrayCommand("ZREVRANGEBYSCORE", key, res_list, min, max, "LIMIT", offset, count);
}

RedisErrCode AFCRedisModule::ZRank(const std::string& key, const std::string& member, int64_t& rank_res)
{
	return OnRetIntCommand("ZRANK", key, rank_res, member);
}

RedisErrCode AFCRedisModule::ZRevRank(const std::string& key, const std::string& member, int64_t& rank_res)
{
	return OnRetIntCommand("ZREVRANK", key, rank_res, member);
}

RedisErrCode AFCRedisModule::ZCard(const std::string& key, int64_t& count)
{
	return OnRetIntCommand("ZCARD", key, count);
}

RedisErrCode AFCRedisModule::ZCount(const std::string& key, const std::string& min, const std::string& max, int64_t& count)
{
	return OnRetIntCommand("ZCOUNT", key, count, min, max);
}

RedisErrCode AFCRedisModule::ZScore(const std::string& key, const std::string member, double& score)
{
	std::string tmp_str;
	auto err_code = OnRetStrCommand("ZSCORE", key, tmp_str, member);
	if (err_code == RedisErrCode::ok)
	{
		score = std::stod(tmp_str);
	}
	return err_code;
}

RedisErrCode AFCRedisModule::ZRemRangeByRank(const std::string& key, const int64_t& begin, const int64_t& end, int64_t& count)
{
	return OnRetIntCommand("ZREMRANGEBYRANK", key, count, begin, end);
}

RedisErrCode AFCRedisModule::ZRemRangeByScore(const std::string& key, const std::string& min, const std::string& max, int64_t& count)
{
	return OnRetIntCommand("ZREMRANGEBYSCORE", key, count, min, max);
}

RedisErrCode AFCRedisModule::HGetAll(const std::string& key, AFVector<std::string>& res_list)
{
	return OnRetArrayCommand("HGETALL", key, res_list);
}

RedisErrCode AFCRedisModule::HLen(const std::string& key, int64_t& length)
{
	return OnRetIntCommand("HLEN", key, length);
}

RedisErrCode AFCRedisModule::HSetNX(const std::string& key, const std::string& field, const std::string& value)
{
	return OnRetNilCommand("HSETNX", key, field, value);
}

RedisErrCode AFCRedisModule::HExists(const std::string& key, const std::string& field, bool& exist)
{
	int64_t tmp_value;
	auto err_code = OnRetIntCommand("HEXISTS", key, tmp_value, field);
	if (err_code == RedisErrCode::ok)
	{
		exist = (tmp_value == 1);
	}
	return err_code;
}

RedisErrCode AFCRedisModule::HKeys(const std::string& key, AFVector<std::string>& fields)
{
	return OnRetArrayCommand("HKEYS", key, fields);
}

RedisErrCode AFCRedisModule::HDel(const std::string& key, const std::string& field, bool& is_deleted)
{
	int64_t tmp_value;
	auto err_code = OnRetIntCommand("HDEL", key, tmp_value, field);
	if (err_code == RedisErrCode::ok)
	{
		is_deleted = (tmp_value == 1);
	}
	return err_code;
}

RedisErrCode AFCRedisModule::HIncrBy(const std::string& key, const std::string& field, const int64_t& increment, int64_t& result_value)
{
	return OnRetIntCommand("HINCRBY", key, result_value, field, increment);	
}

RedisErrCode AFCRedisModule::HIncrByFloat(const std::string& key, const std::string& field, const double& increment, double& result_value)
{
	std::string tmp_str;
	auto err_code = OnRetStrCommand("HINCRBYFLOAT", key, tmp_str, field, increment);
	if (err_code == RedisErrCode::ok)
	{
		result_value = std::stod(tmp_str);
	}
	return err_code;
}

RedisErrCode AFCRedisModule::HScan(const std::string& key, int64_t& ret_cursor, AFVector<std::string>& res_list, const int64_t& cursor, const std::string& pattern, const uint32_t& count)
{
	auto c = AFRedisManager::instance()->GetConn();
    if ( c == nullptr)
    {
        return RedisErrCode::err;
    }
	auto conn = c->GetRedisConn();

	if(conn == nullptr)
	{
		return RedisErrCode::err;
	}
	Buffer rx_buff;
	
	try
	{
		conn->write(bredis::single_command_t("HSCAN", key, std::to_string(cursor), "MATCH", pattern, "COUNT", std::to_string(count)));
		auto result_markers = conn->read(rx_buff);
		AFRedisManager::instance()->ReleaseConn(c);
		rx_buff.consume(result_markers.consumed);
		auto err_code = IsOk(result_markers.result);
		if (err_code != RedisErrCode::ok)
		{
			return err_code;
		}
		auto extract = boost::apply_visitor(bredis::extractor<Iterator>(), result_markers.result);

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
		return RedisErrCode::err;
	}
}

RedisErrCode AFCRedisModule::Exists(const std::string& key, bool& exist)
{
	int64_t tmp_value;
	auto err_code = OnRetIntCommand("EXISTS", key, tmp_value);
	if (err_code == RedisErrCode::ok)
	{
		exist = (tmp_value == 1);
	}
	return err_code;
}

RedisErrCode AFCRedisModule::Del(const std::string& key)
{
	return OnRetNilCommand("DEL", key);
}

RedisErrCode AFCRedisModule::IncrBy(const std::string& key, const int64_t& increment, int64_t& result)
{
	return OnRetIntCommand("INCRBY", key, result, std::to_string(increment));
}

RedisErrCode AFCRedisModule::DecrBy(const std::string& key, const int64_t& decrement, int64_t& result)
{
	return OnRetIntCommand("DECRBY", key, result, std::to_string(decrement));
}

RedisErrCode AFCRedisModule::Expire(const std::string& key, const int64_t& expire_time, bool& result)
{
	int64_t tmp_value;
	auto err_code = OnRetIntCommand("EXPIRE", key, tmp_value);
	if (err_code == RedisErrCode::ok)
	{
		result = (tmp_value == 1);
	}
	return err_code;
}

RedisErrCode AFCRedisModule::MGet(const std::vector<std::string>& commands, const std::string& command,
    const std::string& key, AFVector<std::string>& str_list)
{
    return RetArrayVecCommand(command, key, str_list, commands);
}

RedisErrCode AFCRedisModule::LPush(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length)
{
    return RetIntVecCommand(command, key, length, commands);
}

RedisErrCode AFCRedisModule::RPush(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length)
{
    return RetIntVecCommand(command, key, length, commands);
}

RedisErrCode AFCRedisModule::SRem(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length)
{
    return RetIntVecCommand(command, key, length, commands);
}

RedisErrCode AFCRedisModule::SAdd(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length)
{
    return RetIntVecCommand(command, key, length, commands);
}

RedisErrCode AFCRedisModule::ZAdd(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length)
{
    return RetIntVecCommand(command, key, length, commands);
}

RedisErrCode AFCRedisModule::ZRem(
    const std::vector<std::string>& commands, const std::string& command, const std::string& key, int64_t& length)
{
    return RetIntVecCommand(command, key, length, commands);
}

//Asynchronous Commands Implement
void AFCRedisModule::Get(const std::string& key, RedisStrFunc func)
{
    AsyncOnRetStrCommand("GET", key, func);
}
void AFCRedisModule::Set(
    const std::string& key, const std::string& value, RedisNilFunc func, const uint64_t expire_time)
{
    AsyncOnRetNilCommand("SET", key, func, value, expire_time);
}
void AFCRedisModule::Incr(const std::string& key, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("INCR", key, func);
}
void AFCRedisModule::HSet(
    const std::string& key, const std::string& field, const std::string& value, RedisNilFunc func)
{
    AsyncOnRetNilCommand("HSET", key, func, field, value);
}
void AFCRedisModule::HGet(const std::string& key, const std::string& field, RedisStrFunc func) 
{
    AsyncOnRetStrCommand("HGET", key, func, field);
}
void AFCRedisModule::SetNX(const std::string& key, const std::string& value, RedisNilFunc func)
{
    AsyncOnRetNilCommand("SETNX", key, func, value);
}
void AFCRedisModule::GetSet(const std::string& key, const std::string& value, RedisStrFunc func) 
{
    AsyncOnRetStrCommand("GETSET", key, func, value);
}
void AFCRedisModule::LLen(const std::string& key, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("LLEN", key, func);
}
void AFCRedisModule::LRange(
    const std::string& key, const int64_t& begin, const int64_t& end, RedisArrayFunc func) 
{
    AsyncOnRetArrayCommand("LRANGE", key, func, begin, end);
}
void AFCRedisModule::LPop(const std::string& key, RedisStrFunc func) 
{
    AsyncOnRetStrCommand("LPOP", key, func);
}
void AFCRedisModule::RPop(const std::string& key, RedisStrFunc func) 
{
    AsyncOnRetStrCommand("RPOP", key, func);
}
void AFCRedisModule::SCard(const std::string& key, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("SCARD", key, func);
}
void AFCRedisModule::SMembers(const std::string& key, RedisArrayFunc func) 
{
    AsyncOnRetArrayCommand("SMEMBERS", key, func);
}
void AFCRedisModule::SRandMember(const std::string& key, const uint32_t& count, RedisArrayFunc func) 
{
    AsyncOnRetArrayCommand("SRANDMEMBER", key, func, count);
}
void AFCRedisModule::SPop(const std::string& key, const uint32_t& count, RedisArrayFunc func) 
{
    AsyncOnRetArrayCommand("SPOP", key, func, count);
}
void AFCRedisModule::SPop(const std::string& key, RedisStrFunc func) 
{
    AsyncOnRetStrCommand("SPOP", key, func);
}
void AFCRedisModule::SIsMember(const std::string& key, const std::string& member, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("SISMEMBER", key, func, member);
}
void AFCRedisModule::ZIncrBy(
    const std::string& key, const double& increment, const std::string& member, RedisStrFunc func)
{
    AsyncOnRetStrCommand("ZINCRBY", key, func, increment, member);
}
void AFCRedisModule::ZRange(
    const std::string& key, const int64_t& begin, const int64_t& end, RedisArrayFunc func, bool with_score)
{
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZRANGE", key, func, begin, end, "WITHSCORE");
    }
    else
    {
        AsyncOnRetArrayCommand("ZRANGE", key, func, begin, end);
	}
}
void AFCRedisModule::ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
    RedisArrayFunc func,
    bool with_score)
{
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZRANGEBYSCORE", key, func, min, max, "WITHSCORE");
    }
    else
    {
        AsyncOnRetArrayCommand("ZRANGEBYSCORE", key, func, min, max);
    }
}
void AFCRedisModule::ZRangeByScore(const std::string& key, const std::string& min, const std::string& max,
    const int64_t& offset, const uint32_t& count, RedisArrayFunc func,
    bool with_score)
{
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZRANGEBYSCORE", key, func, min, max, "WITHSCORE", "LIMIT", offset, count);
    }
    else
    {
        AsyncOnRetArrayCommand("ZRANGEBYSCORE", key, func, min, max, "LIMIT", offset, count);
    }
}
void AFCRedisModule::ZRevRange(const std::string& key, const std::int64_t& begin, const int64_t& end,
    RedisArrayFunc func,
    bool with_score)
{
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZREVRAMGE", key, func, begin, end, "WITHSCORE");
    }
    else
    {
        AsyncOnRetArrayCommand("ZRANGEBYSCORE", key, func, begin, end);
    }
}
void AFCRedisModule::ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
    RedisArrayFunc func, bool with_score)
{
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZREVRANGEBYSCORE", key, func, min, max, "WITHSCORES");
	}
    else
    {
        AsyncOnRetArrayCommand("ZREVRANGEBYSCORE", key, func, min, max);
	}
}
void AFCRedisModule::ZRevRangeByScore(const std::string& key, const std::string& min, const std::string& max,
    const int64_t& offset, const uint32_t& count, RedisArrayFunc func, bool with_score)
{
    if (with_score)
    {
        AsyncOnRetArrayCommand("ZREVRANGEBYSCORE", key, func, min, max, "WITHSCORES", "LIMIT", offset, count);
	}
    else
    {
        AsyncOnRetArrayCommand("ZREVRANGEBYSCORE", key, func, min, max, "LIMIT", offset, count);
	}
} 
void AFCRedisModule::ZRank(const std::string& key, const std::string& member, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("ZRANK", key, func, member);
}
void AFCRedisModule::ZRevRank(const std::string& key, const std::string& member, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("ZREVRANK", key, func, member);
}
void AFCRedisModule::ZCard(const std::string& key, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("ZCARD", key, func);
}
void AFCRedisModule::ZCount(
    const std::string& key, const std::string& min, const std::string& max, RedisIntFunc func)
{
    AsyncOnRetIntCommand("ZCOUNT", key, func, min, max);
}
void AFCRedisModule::ZScore(const std::string& key, const std::string member, RedisStrFunc func) 
{
    AsyncOnRetStrCommand("ZSCORE", key, func, member);
}
void AFCRedisModule::ZRemRangeByRank(
    const std::string& key, const int64_t& begin, const int64_t& end, RedisIntFunc func)
{
    AsyncOnRetIntCommand("ZREMRANGEBYRANK", key, func, begin, end);
}
void AFCRedisModule::ZRemRangeByScore(
    const std::string& key, const std::string& min, const std::string& max, RedisIntFunc func)
{
    AsyncOnRetIntCommand("ZREMRANGEBYSCORE", key, func, min, max);
}
void AFCRedisModule::HGetAll(const std::string& key, RedisArrayFunc func) 
{
    AsyncOnRetArrayCommand("HGETALL", key, func);
}
void AFCRedisModule::HLen(const std::string& key, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("HLEN", key, func);
}
void AFCRedisModule::HSetNX(
    const std::string& key, const std::string& field, const std::string& value, RedisNilFunc func)
{
    AsyncOnRetNilCommand("HSETNX", key, func, field, value);
}
void AFCRedisModule::HExists(const std::string& key, const std::string& field, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("HEXISTS", key, func, field);
}
void AFCRedisModule::HKeys(const std::string& key, RedisArrayFunc func) 
{
    AsyncOnRetArrayCommand("HKEYS", key, func);
}
void AFCRedisModule::HDel(const std::string& key, const std::string& field, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("HDEL", key, func, field);
}
void AFCRedisModule::HIncrBy(
    const std::string& key, const std::string& field, const int64_t& increment, RedisIntFunc func)
{
    AsyncOnRetIntCommand("HINCRBY", key, func, field, increment);
}
void AFCRedisModule::HIncrByFloat(
    const std::string& key, const std::string& field, const double& increment, RedisStrFunc func)
{
    AsyncOnRetStrCommand("HINCRBYFLOAT", key, func, field, increment);
}
void AFCRedisModule::HScan(const std::string& key,
    void (*HscanFunc)(const RedisErrCode&, const int64_t&, const std::vector<std::string>&), const int64_t& cursor,
    const std::string& pattern, const uint32_t& count)
{
	std::vector<std::string> result_vector;
    auto c = AFRedisManager::instance()->GetConn();
    if (c == nullptr)
    {
        HscanFunc(RedisErrCode::disconnect, -1, result_vector);
	}
    auto conn = c->GetRedisConn();
    if (conn == nullptr)
    {
        HscanFunc(RedisErrCode::disconnect, -1, result_vector);
	}

    auto io = c->GetIo();

    Buffer rx_buff, tx_buff;
    
    auto commands = bredis::single_command_t("HSCAN", key, std::to_string(cursor), "MATCH", pattern, "COUNT", std::to_string(count)); 

    conn->async_write(tx_buff, commands, [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec)
        {
            AFRedisManager::instance()->ReleaseConn(c);
            HscanFunc(RedisErrCode::disconnect, -1, result_vector);
            ARK_LOG_ERROR("Redis Connection Failed with Command: HSCAN and key: {}", key);
            return;
        }
        tx_buff.consume(bytes_transferred);
        conn->async_read(rx_buff, [&](const boost::system::error_code& ec, result_t&& r) {
            if (ec)
            {
                AFRedisManager::instance()->ReleaseConn(c);
                HscanFunc(RedisErrCode::disconnect, -1, result_vector);
                ARK_LOG_ERROR("Redis Connection Failed with Command: HSCAN and key: {}", key);
                return;
            }
            AFRedisManager::instance()->ReleaseConn(c);
            rx_buff.consume(r.consumed);
            auto err_code = IsOk(r.result);
            if (err_code != RedisErrCode::ok)
            {
                HscanFunc(err_code, -1, result_vector);
                return;
            }
            auto extract = boost::apply_visitor(bredis::extractor<Iterator>(), r.result);
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
void AFCRedisModule::Exists(const std::string& key, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("EXISTS", key, func);
}
void AFCRedisModule::Del(const std::string& key, RedisNilFunc func) 
{
    AsyncOnRetNilCommand("DEL", key, func);
}
void AFCRedisModule::IncrBy(const std::string& key, const int64_t& increment, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("INCRBY", key, func, increment);
}
void AFCRedisModule::DecrBy(const std::string& key, const int64_t& decrement, RedisIntFunc func) 
{
    AsyncOnRetIntCommand("DECRBY", key, func, decrement);
}
void AFCRedisModule::Expire(const std::string& key, const int64_t& expire_time, RedisIntFunc func)
{
    AsyncOnRetIntCommand("EXPIRE", key, func, expire_time);
}

void AFCRedisModule::AsynMSet(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisNilFunc func)
{
	AsynRetNilVecCommand(command, key, func, commands);
}
void AFCRedisModule::AsynMGet(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisArrayFunc func)
{
	AsynRetArrayVecCommand(command, key, func, commands);
}
void AFCRedisModule::AsynLPush(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func)
{
	AsynRetIntVecCommand(command, key, func, commands);
}
void AFCRedisModule::AsynRPush(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func)
{
	AsynRetIntVecCommand(command, key, func, commands);
}
void AFCRedisModule::AsynSRem(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func)
{
	AsynRetIntVecCommand(command, key, func, commands);
}
void AFCRedisModule::AsynSAdd(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func)
{
	AsynRetIntVecCommand(command, key, func, commands);
}
void AFCRedisModule::AsynZAdd(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func)
{
	AsynRetIntVecCommand(command, key, func, commands);
}
void AFCRedisModule::AsynZRem(const std::vector<std::string>& commands, const std::string& command, const std::string& key, RedisIntFunc func)
{
	AsynRetIntVecCommand(command, key, func, commands);
}



RedisErrCode AFCRedisModule::RetNilVecCommand(const std::string& command, const std::string& key, const std::vector<std::string>& args)
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
		conn->write(bredis::single_command_t(args.begin(), args.end()));
		auto result_markers = conn->read(rx_buff);
		AFRedisManager::instance()->ReleaseConn(c);
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
        return RedisErrCode::disconnect;
	}
}



RedisErrCode AFCRedisModule::RetIntVecCommand(const std::string& command, const std::string& key, int64_t& value, const std::vector<std::string>& args)
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
		conn->write(bredis::single_command_t(args.begin(), args.end()));
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
        return RedisErrCode::disconnect;
	}
}




RedisErrCode AFCRedisModule::RetStrVecCommand(const std::string& command, const std::string& key, std::string& value, const std::vector<std::string>& args)
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
		conn->write(bredis::single_command_t(args.begin(), args.end()));
			
		auto result_markers = conn->read(rx_buff);
		AFRedisManager::instance()->ReleaseConn(c);;
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
        return RedisErrCode::disconnect;
	}
}




RedisErrCode AFCRedisModule::RetArrayVecCommand(const std::string& command, const std::string& key, std::vector<std::string>& value, const std::vector<std::string>& args)
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
		conn->write(bredis::single_command_t(args.begin(), args.end()));
			
		auto result_markers = conn->read(rx_buff);
		AFRedisManager::instance()->ReleaseConn(c);
		rx_buff.consume(result_markers.consumed);
		auto err_code = IsOk(result_markers.result);
		if (err_code != RedisErrCode::ok)
		{
			return err_code;
		}
		auto extract = boost::apply_visitor(bredis::extractor<Iterator>(), result_markers.result);

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
        return RedisErrCode::disconnect;
	}
}


void AFCRedisModule::AsynRetNilVecCommand(const std::string& command, const std::string& key, RedisNilFunc func , const std::vector<std::string>& args)
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
	
	conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()), [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec)
        {
            AFRedisManager::instance()->ReleaseConn(c);
            func(RedisErrCode::disconnect);
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
                AFRedisManager::instance()->ReleaseConn(c);
                func(RedisErrCode::disconnect);
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
            AFRedisManager::instance()->ReleaseConn(c);
            rx_buff.consume(r.consumed);
            auto err_code = IsOk(r.result);
            func(err_code);
            return;
        });
    });
    io.get().poll();
}



void AFCRedisModule::AsynRetIntVecCommand(const std::string& command, const std::string& key, RedisIntFunc func, const std::vector<std::string>& args)
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
	
	conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()), [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec)
        {
            AFRedisManager::instance()->ReleaseConn(c);
            func(RedisErrCode::disconnect, -1);
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
                AFRedisManager::instance()->ReleaseConn(c);
                func(RedisErrCode::disconnect, -1);
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
            return;
        });
    });
    io.get().poll();
}




void AFCRedisModule::AsynRetStrVecCommand(const std::string& command, const std::string& key, RedisStrFunc func, const std::vector<std::string>& args)
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
	conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()), [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec)
        {
            AFRedisManager::instance()->ReleaseConn(c);
            func(RedisErrCode::disconnect, std::string());
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
                AFRedisManager::instance()->ReleaseConn(c);
                func(RedisErrCode::disconnect, std::string());
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
            return;
        });
    });	
    io.get().poll();
}




void AFCRedisModule::AsynRetArrayVecCommand(const std::string& command, const std::string& key, RedisArrayFunc func, const std::vector<std::string>& args)
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
		
	conn->async_write(tx_buff, bredis::single_command_t(args.begin(), args.end()), [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec)
        {
            AFRedisManager::instance()->ReleaseConn(c);
            func(RedisErrCode::disconnect, result_vector);
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
                AFRedisManager::instance()->ReleaseConn(c);
                func(RedisErrCode::disconnect, result_vector);
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
            return;
        });
    });	
    io.get().poll();
}



} // namespace ark

