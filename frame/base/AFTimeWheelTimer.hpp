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
#include "AFDefine.hpp"
#include "AFSingleton.hpp"
#include "AFDateTime.hpp"

namespace ark
{	
	const uint32_t FREE_TIMER_SIZE = 1000;

	const uint32_t TIMER_PRECISION = 1;		//ms
	const uint32_t TV_BITS = 8;
	const uint32_t TV_SIZE = 1 << TV_BITS;	//256 slots each time_vec
	const uint32_t TV_MASK = TV_SIZE - 1;
	const uint32_t TV_NUM = 6;				//6 levels	

    class AFTimeWheelData
    {
    public:
		AFTimeWheelData()
        {
            
        }
        
        int32_t count = 0;		// -1 means forever
        uint32_t interval = 0;
        uint64_t expires = 0;
		uint64_t idx = 0;
        TIMER_FUNCTOR_PTR callback;

        //callback data
		char name[16];
        AFGUID entity_id = 0;
    };

	class AFTimeWheelVec
	{
	public:
		int index;
		std::vector<std::list<AFTimeWheelData*>> vec;
	};

    class AFTimeWheelManager : public AFSingleton<AFTimeWheelManager>
    {
    public:
		AFTimeWheelManager()
        {
            mnNowSlot = 0;
            mnLastUpdateTime = 0;
        }

        ~AFTimeWheelManager()
        {

        }

        void Init(uint64_t now_time)
        {
            mnNowSlot = 0;
			mnTimerIdx = 0;
            mnLastUpdateTime = now_time;

			for (int i = 0 ; i < FREE_TIMER_SIZE; ++i)
			{
				AFTimeWheelData* data = ARK_NEW AFTimeWheelData();
				memset(data, 0, sizeof(AFTimeWheelData));
				mxFreeTimers.push_back(data);
			}

			for (int i = 0 ; i < TV_NUM ; ++i)
			{
				AFTimeWheelVec* tv = ARK_NEW AFTimeWheelVec();
				InitTimerVec(tv, TV_SIZE);
				mxTVs[i] = tv;
			}
        }

        void Update(int64_t now_time)
        {
            UpdateTimer(now_time);
        }

        void Shut()
        {
            for (auto iter : mxTimers)
            {
				ARK_DELETE(iter.second);
            }

			for (auto iter : mxFreeTimers)
			{
				ARK_DELETE(iter);
			}

			for (auto iter : mxTVs)
			{
				ARK_DELETE(iter);
			}

            mxTimers.clear();
			mxFreeTimers.clear();
        }

        uint64_t AddTimer(const std::string& name, const AFGUID& entity_id, int32_t count, uint32_t interval, TIMER_FUNCTOR_PTR callback)
        {
            AFTimeWheelData* timer = GetFreeTimer();            
            ARK_STRNCPY(timer->name, name.c_str(), (name.length() > 16) ? 16 : name.length() + 1);
			timer->idx = ++mnTimerIdx;	
			timer->expires = (interval / TIMER_PRECISION) + mnNowSlot;
            timer->count = count;
            timer->interval = (interval / TIMER_PRECISION);
            timer->callback = callback;
            timer->entity_id = entity_id;

			AddTimerInternal(timer);
			mxTimers.insert(make_pair(timer->idx, timer));

            return timer->idx;
        }

        bool RemoveTimer(uint64_t idx)
        {
			auto iter = mxTimers.find(idx);
			if (iter == mxTimers.end())
			{
				return false;
			}
			AFTimeWheelData* timer = iter->second;
			mxTimers.erase(iter);

			timer->callback = nullptr;

			return true;
        }

    protected:
		void InitTimerVec(AFTimeWheelVec* tv, int32_t size)
		{
			tv->index = 0;
			tv->vec.resize(size);
			for (int i = 0 ; i < size ; ++i)
			{
				tv->vec[i] = std::list<AFTimeWheelData*>();
			}
		}

		AFTimeWheelData * GetFreeTimer()
		{
			AFTimeWheelData* timer = nullptr;
			if (mxFreeTimers.empty())
			{
				timer = ARK_NEW AFTimeWheelData;
			}
			else
			{
				timer = mxFreeTimers.front();
				mxFreeTimers.pop_front();
			}

			memset(timer, 0, sizeof(AFTimeWheelData));
			return timer;
		}

		void AddToFreeTimer(AFTimeWheelData * timer)
		{
			mxFreeTimers.push_back(timer);
		}

        void UpdateTimer(int64_t now_time)
        {
            uint64_t passedSlot = (now_time - mnLastUpdateTime) / TIMER_PRECISION;

            if (passedSlot == 0)
            {
                return;
            }

            mnLastUpdateTime += passedSlot * TIMER_PRECISION;

            for (uint64_t i = 0; i < passedSlot; ++i)
            {                
                UpdateSlotTimer();
            }
        }

		void AddTimerInternal(AFTimeWheelData* timer_data)
		{
			uint64_t expires = timer_data->expires;
			int64_t idx = expires - mnNowSlot;
			if (idx <= 0)
			{
				AFTimeWheelVec* tv = mxTVs[0];
				tv->vec[tv->index].push_back(timer_data);
				return;
			}

			uint32_t index = 0;
			uint64_t max_tv_num = 0;
			for (int32_t i = 0 ; i < TV_NUM ; ++i)
			{
				max_tv_num = ((uint64_t)1 << ((i + 1) * TV_BITS));
				if ((uint64_t)idx < max_tv_num)
				{
					index = ((expires >> (i * TV_BITS)) & TV_MASK);
					mxTVs[i]->vec[index].push_back(timer_data);
					return;
				}
			}

			ARK_ASSERT_NO_EFFECT(0);
		}
		
        void UpdateSlotTimer()
        {
			if (mxTVs[0]->index == 0)
			{
				int32_t n = 1;
				do 
				{
					CarryTimers(mxTVs[n]);
				} while (mxTVs[n]->index == 1 && ++n < TV_NUM);
			}
			
			int32_t index = mxTVs[0]->index;
			while (!mxTVs[0]->vec[index].empty())
			{
				AFTimeWheelData* timer = mxTVs[0]->vec[index].front();
				mxTVs[0]->vec[index].pop_front();

				if (timer->callback == nullptr)
				{
					AddToFreeTimer(timer);
					continue;
				}

				//timer handler
				(*(timer->callback))(timer->name, timer->entity_id);
				
				//forever timer
				if (timer->count < 0 && timer->interval > 0)
				{
					timer->expires = mnNowSlot + timer->interval;
					AddTimerInternal(timer);
					continue;
				}

				timer->count--;
				if (timer->count > 0 && timer->interval > 0)
				{					
					timer->expires = mnNowSlot + timer->interval;
					AddTimerInternal(timer);
				}
				else
				{
					mxTimers.erase(timer->idx);
					AddToFreeTimer(timer);
				}
			}

			mnNowSlot++;
			mxTVs[0]->index = (mxTVs[0]->index + 1) & TV_MASK;
        }

		void CarryTimers(AFTimeWheelVec* tv)
		{
			int32_t index = tv->index;
			while (!tv->vec[index].empty())
			{
				AFTimeWheelData* timer = tv->vec[index].front();
				AddTimerInternal(timer);
				tv->vec[index].pop_front();
			}

			tv->index = (tv->index + 1) & TV_MASK;
		}

    private:
		uint32_t mnNowSlot;
		AFTimeWheelVec* mxTVs[TV_NUM];
        uint64_t mnLastUpdateTime;
		uint64_t mnTimerIdx;
		std::unordered_map<uint64_t, AFTimeWheelData*> mxTimers;
		std::list<AFTimeWheelData*> mxFreeTimers;
    };

}