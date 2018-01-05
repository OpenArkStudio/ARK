/****************************************************************************

Git <https://github.com/sniper00/MoonNetLua>
E-Mail <hanyongtao@live.com>
Copyright (c) 2015-2017 moon
Licensed under the MIT License <http://opensource.org/licenses/MIT>.

****************************************************************************/
#pragma once
#include <cstdint>
#include <memory>
#include <functional>
#include <cassert>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <list>

namespace moon
{
	using timerid_t = uint32_t;
	//using timer_handler_t = std::function<void(timerid_t)>;

	namespace detail
	{
		template<typename TimerHandler>
		class timer_context
		{
		public:
			using timer_handler_t = TimerHandler;

			timer_context(const timer_handler_t& callBack, int32_t duration, bool forever = false)
				:removed_(false)
				, forever_(forever)
				, id_(0)
				, repeattimes_(0)
				, duration_(duration)
				, endtime_(0)
				, handler_(callBack)
			{
			}

			timer_context(timer_handler_t&& callBack, int32_t duration, bool forever = false)
				:removed_(false)
				, forever_(forever)
				, id_(0)
				, repeattimes_(0)
				, duration_(duration)
				, endtime_(0)
				, handler_(std::forward<timer_handler_t>(callBack))
			{
			}

			void init(timer_handler_t&& callBack, int32_t duration, bool forever = false)
			{
				removed_ = false;
				forever_ = forever;
				id_ = 0;
				repeattimes_ = 0;
				duration_ = duration;
				endtime_ = 0;
				handler_ = std::forward<timer_handler_t>(callBack);
			}

			void id(timerid_t value)
			{
				id_ = value;
			}

			timerid_t id()
			{
				return id_;
			}

			void endtime(int64_t value)
			{
				endtime_ = value;
			}

			int64_t endtime()
			{
				return endtime_;
			}

			int64_t duration()
			{
				return duration_;
			}

			void repeattimes(int32_t value)
			{
				repeattimes_ = value;
			}

			int32_t repeattimes()
			{
				return repeattimes_;
			}

			void  expired(timerid_t id)
			{
				assert(nullptr != handler_);
				handler_(id);
			}

			void removed(bool value)
			{
				removed_ = value;
			}

			bool removed()
			{
				return removed_;
			}

			bool forever()
			{
				return forever_;
			}
		private:
			bool removed_;
			bool	forever_;
			timerid_t id_;
			int32_t	repeattimes_;//重复次数，-1循环
			int32_t	duration_;
			int64_t	endtime_;
			timer_handler_t handler_;//调度器回掉
		};

		template<typename TContainer, uint8_t TSize>
		class timer_wheel
		{
		public:
			timer_wheel()
				:m_head(0)
			{
			}

			TContainer & operator[](uint8_t pos)
			{
				assert(pos < TSize);
				return m_array[pos];
			}

			TContainer & front()
			{
				assert(m_head < TSize);
				return m_array[m_head];
			}

			void pop_front()
			{
				auto tmp = ++m_head;
				m_head = tmp % TSize;
			}

			bool round()
			{
				return m_head == 0;
			}

			uint8_t size()
			{
				return TSize;
			}

			size_t next_slot() { return m_head; }
		private:
			TContainer 	m_array[TSize];
			uint32_t	m_head;
		};

		inline int64_t millseconds()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}
	}


	template<typename TimerHandler>
	class timer
	{
		using timer_handler_t = TimerHandler;
		using  timer_context_ptr = std::shared_ptr<detail::timer_context<timer_handler_t>>;

		//every wheel size，max 255
		static const uint8_t  WHEEL_SIZE = 255;
		//precision ms
		static const int32_t PRECISION = 10;

		static const int TIMERID_SHIT = 32;

		static const int MAX_TIMER_NUM = (1<<16)-1;

		using timer_wheel_t = detail::timer_wheel<std::list<uint64_t>, WHEEL_SIZE>;
	public:
		timer()
			: tick_(0)
			, prew_tick_(0)
			, uuid_(1)
			, stop_(false)
		{
			wheels_.emplace_back();
			wheels_.emplace_back();
			wheels_.emplace_back();
			wheels_.emplace_back();
		}

		~timer()
		{

		}
		/**
		* 添加一个只执行一次的计时器
		*
		* @duration 计时器间隔 ms
		* @callBack  回掉函数
		*  typedef   std::function<void()> timer_handler_t;
		* 返回计时器ID
		*/
		timerid_t  expired_once(int32_t duration, const timer_handler_t& callBack)
		{
			if (duration < PRECISION)
				duration = PRECISION;
			auto pt = std::make_shared<detail::timer_context<timer_handler_t>>(callBack, duration, false);
			pt->repeattimes(1);
			return add_new_timer(pt);
		}

		timerid_t  expired_once(int32_t duration,timer_handler_t&& callBack)
		{
			if (duration < PRECISION)
				duration = PRECISION;
			auto pt = std::make_shared<detail::timer_context<timer_handler_t>>(std::forward<timer_handler_t>(callBack), duration, false);
			pt->repeattimes(1);
			return add_new_timer(pt);
		}

		/**
		* 添加重复执行的计时器
		*
		* @duration 计时器间隔 ms
		* @times 重复次数，(-1 无限循环)
		* @callBack 回掉函数
		*  typedef   std::function<void()> timer_handler_t;
		* 返回计时器ID
		*/
		timerid_t  repeat(int32_t duration, int32_t times, const timer_handler_t& callBack)
		{
			if (duration < PRECISION)
			{
				duration = PRECISION;
			}

			auto pt = std::make_shared<detail::timer_context<timer_handler_t>>(callBack, duration, times < 0);
			pt->repeattimes(times);
			return add_new_timer(pt);
		}

		timerid_t  repeat(int32_t duration, int32_t times,timer_handler_t&& callBack)
		{
			if (duration < PRECISION)
			{
				duration = PRECISION;
			}

			auto pt = std::make_shared<detail::timer_context<timer_handler_t>>(std::forward<timer_handler_t>(callBack), duration, times < 0);
			pt->repeattimes(times);
			return add_new_timer(pt);
		}

		/**
		* 移除一个计时器
		*
		* @timerid 计时器 ID
		*/
		void			remove(timerid_t timerid)
		{
			auto iter = timers_.find(timerid);
			if (iter != timers_.end())
			{
                iter->second->removed(true);
				return;
			}

			for (auto& nit : new_timers_)
			{
				if (nit->id() == timerid)
				{
					nit->removed(true);
					break;
				}
			}
		}

		//逻辑线程需要调用这个函数，驱动计时器
		void			update()
		{
			auto nowTick = detail::millseconds();
			for (auto& it : new_timers_)
			{
				if (it->removed())
					continue;
				add_timer(nowTick,it);
			}
			new_timers_.clear();

			if (prew_tick_ == 0)
			{
				prew_tick_ = nowTick;
			}

			tick_ += (nowTick - prew_tick_);
			prew_tick_ = nowTick;

			auto& wheels = wheels_;
			while (tick_ >= PRECISION)
			{
				tick_ -= PRECISION;
				if (stop_)
					continue;

				{
					auto& timers = wheels[0].front();
					if (!timers.empty())
					{
						expired(timers);
					}
					wheels[0].pop_front();
				}

				int i = 0;
				for (auto wheel = wheels.begin(); wheel != wheels.end(); ++wheel, ++i)
				{
					auto next_wheel = wheel;
					if (wheels.end() == (++next_wheel))
						break;

					if (wheel->round())
					{
						auto& timers = next_wheel->front();
						while (!timers.empty())
						{
							auto key = timers.front();
							timers.pop_front();
							auto slot = get_slot(key, i);
							(*wheel)[slot].push_front(key);
							//printf("update timer id %u add to wheel [%d] slot [%d]\r\n", (key>>32), i+1, slot);
						}
						next_wheel->pop_front();
					}
					else
					{
						break;
					}
				}
			}
		}

		void			stop_all_timer()
		{
			stop_ = true;
		}

		void			start_all_timer()
		{
			stop_ = false;
		}

	private:
		// slots:      8bit(notuse) 8bit(wheel3_slot)  8bit(wheel2_slot)  8bit(wheel1_slot)  
		uint64_t make_key(timerid_t id, uint32_t slots)
		{
			return ((static_cast<uint64_t>(id) << TIMERID_SHIT) | slots);
		}

		inline uint8_t get_slot(uint64_t  key, int which_queue)
		{
			return (key >> (which_queue * 8)) & 0xFF;
		}

		timerid_t 	add_new_timer(const timer_context_ptr& t)
		{
			t->endtime(t->duration() + detail::millseconds());
			if (t->id() == 0)
			{
				if (uuid_ == 0 || uuid_ == MAX_TIMER_NUM)
					uuid_ = 1;
				auto& timers = timers_;
				while (timers.find(uuid_) != timers.end())
				{
					++uuid_;
				}
				t->id(uuid_);
				timers_.emplace(t->id(), t);
			}
			new_timers_.push_back(t);
			return t->id();
		}

		void	add_timer(int64_t now, const timer_context_ptr& t)
		{
			if (t->endtime() > now)
			{
				auto diff = t->duration();
				auto offset = diff%PRECISION;
				//校正
				if (offset > 0)
				{
					diff += PRECISION;
				}
				auto slot_count = diff / PRECISION;
				slot_count = (slot_count > 0) ? slot_count : 1;
				uint64_t key = 0;
				do
				{
					int i = 0;
					uint32_t slots = 0;
					for (auto it = wheels_.begin(); it != wheels_.end(); it++, i++)
					{
						auto& wheel = *it;
						slot_count += wheel.next_slot();
						uint8_t slot = (slot_count - 1) % (wheel.size());
						slot_count -= slot;
						slots |= ( static_cast<uint32_t>(slot) << (i * 8));
						key = make_key(t->id(), slots);
						//printf("process timer id %u wheel[%d] slot[%d]\r\n", t->id(), i+1, slot);
						if (slot_count < wheel.size())
						{
							//printf("timer id %u add to wheel [%d] slot [%d]\r\n",t->id(),  i + 1, slot);
							wheel[slot].push_back(key);
							break;
						}
						slot_count /= wheel.size();
					}
					break;
				} while (0);
			}
			else
			{
				wheels_[0].front().push_back(make_key(t->id(), 0));
				//printf("time out.\r\n");
			}
		}

		void	 expired(std::list<uint64_t>& expires)
		{
			//auto& timers = timers_;
			//{
			//	auto key = expires.front();
			//	timerid_t id = (key >> TIMERID_SHIT);
			//	auto iter = timers.find(id);
			//	if (iter != timers.end())
			//	{
			//		if (iter->second->endtime() - detail::millseconds() >= PRECISION)
			//			return false;
			//	}
			//}

			while (!expires.empty())
			{
				auto key = expires.front();
				expires.pop_front();
				timerid_t id = static_cast<timerid_t>(key >> TIMERID_SHIT);
				auto iter = timers_.find(id);
				if (iter == timers_.end())
					continue;

				auto&ctx = iter->second;

				if (!ctx->removed())
				{
					ctx->expired(id);
					ctx->repeattimes(ctx->repeattimes() - 1);

					if (ctx->repeattimes() > 0 || ctx->forever())
					{
						add_new_timer(ctx);
						continue;
					}
				}
				timers_.erase(iter);
			}
			//return true;
		}
	private:
		int64_t tick_;
		int64_t prew_tick_;
		uint32_t uuid_;
		bool stop_;
		std::vector < timer_wheel_t> wheels_;
		std::unordered_map<uint32_t, timer_context_ptr> timers_;
		std::vector<timer_context_ptr> new_timers_;
	};
}

