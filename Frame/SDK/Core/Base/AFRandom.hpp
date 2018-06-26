/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

#include "AFPlatform.hpp"

class AFRandom
{
public:
    AFRandom() : _seed(0), _get_count(0)
    {
    }

    ~AFRandom() {}

    //--------------------------------------------------------------------
    // 设置随机数种子
    void SetSeed(uint32_t seed)
    {
        _seed = seed;
        _get_count = 0;
    }

    uint32_t GetSeed()
    {
        return _seed;
    }

    //--------------------------------------------------------------------
    // 生成一个随机数
    inline uint32_t Random()
    {
        unsigned int next = _seed;
        unsigned int result = 0;

        next *= 1103515245;
        next += 12345;
        result = (unsigned int)(next >> 16) % 2048;

        next *= 1103515245;
        next += 12345;
        result <<= 10;
        result ^= (unsigned int)(next >> 16) % 1024;

        next *= 1103515245;
        next += 12345;
        result <<= 10;
        result ^= (unsigned int)(next >> 16) % 1024;

        _seed = next;

        _get_count++;

        //reset
        if (_get_count >= (std::numeric_limits<int>::max() - 1))
        {
            _get_count = 0;
        }

        return result;
    }

    //--------------------------------------------------------------------
    //生成一个[0, max_range - 1]之间的随机数
    uint32_t Random(uint32_t max_range)
    {
        if (max_range == 0)
        {
            return 0;
        }

        return Random() % max_range;
    }

    //生成一个[min_range, max_range - 1]之间的随机数
    unsigned int Random(uint32_t min_range, uint32_t max_range)
    {
        uint32_t min = 0;
        uint32_t max = 0;

        if (min_range <= max_range)
        {
            min = min_range;
            max = max_range;
        }
        else
        {
            min = max_range;
            max = min_range;
        }

        if (min == max)
        {
            return max;
        }

        return Random(max - min) + min;
    }

    // 生成一个[min_range, max_range - 1]之间的随机数
    int Random(int min_range, int max_range)
    {
        if (min_range == max_range)
        {
            return min_range;
        }
        else if (min_range > max_range)
        {
            std::swap(min_range, max_range);
            //int iMinSwap = min_range;
            //min_range = max_range;
            //max_range = iMinSwap;
        }

        int range = max_range - min_range;
        int rand_result = Random((uint32_t)range);

        return rand_result + min_range;
    }

    int GetRandomCount()
    {
        return _get_count;
    }

private:
    uint32_t _seed;
    int _get_count;
};
