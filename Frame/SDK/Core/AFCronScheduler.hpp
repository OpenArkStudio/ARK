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
#include "SDK/Core/Common/cronexpr.h"

namespace ark
{

    class AFCronSheduler;

    class AFCronData
    {
        friend class AFCronSheduler;
    public:
        class AFCronSorter
        {
        public:
            bool operator()(const AFCronData* lhs, const AFCronData* rhs)
            {
                return lhs->next_time < rhs->next_time;
            }
        };

        AFCronData()
        {
        }

        ~AFCronData()
        {
            Reset();
        }

        bool Parse(const char* cron_expression)
        {
            Reset();
            const char* err_msg = nullptr;
            cron_parse_expr(cron_expression, &cron_parser, &err_msg);
            return (err_msg == nullptr);
        }

        void Reset()
        {
            cron_id = 0;
            user_data = 0;
            next_time = 0;
            delete_flag = false;
        }

        bool IsTriggerable(int64_t now)
        {
            if (delete_flag)
            {
                return false;
            }

            now = now / 1000; //convert ms to s

            if (now > next_time)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool Trigger()
        {
            if (callback == nullptr)
            {
                return false;
            }

            if (delete_flag)
            {
                return false;
            }
            else
            {
                //Do not return callback result, because callback is your own logic
                //maybe return false
                (*callback)(cron_id, user_data);
                return true;
            }
        }

        void GetNext(int64_t now)
        {
            if (delete_flag)
            {
                return;
            }

            now = now / 1000; //convert ms to s

            if (now >= next_time)
            {
                next_time = cron_next(&cron_parser, now);
            }
        }

    private:
        int cron_id{ 0 };
        int user_data{ 0 };//Will be extended by specific logic
        SCHEDULER_FUNCTOR_PTR callback{ nullptr };
        time_t next_time{ 0 };
        cron_expr cron_parser;
        bool delete_flag{ false };
    };

    class AFCronSheduler
    {
    public:
        AFCronSheduler()
        {

        }

        ~AFCronSheduler()
        {
            Clear();
        }

        void Update(int64_t now)
        {
            if (mbCronWaitDelete)
            {
                for (auto iter = mxCronList.begin(); iter != mxCronList.end();)
                {
                    AFCronData* pCron = *iter;

                    if (pCron->delete_flag)
                    {
                        ARK_DELETE(pCron);
                        iter = mxCronList.erase(iter);
                    }
                    else
                    {
                        ++iter;
                    }
                }

                mbCronWaitDelete = false;
            }

            time_t tn;
            time(&tn);
            bool bTriggered = false;

            for (auto it : mxCronList)
            {
                if (it->IsTriggerable(now))
                {
                    it->Trigger();
                    it->GetNext(now);
                    bTriggered = true;
                }
                else
                {
                    break;
                }
            }

            if (bTriggered)
            {
                SortCrons();
            }
        }

        bool AddCron(int cron_id, int user_arg, const char* cron_expression, int64_t now, SCHEDULER_FUNCTOR_PTR cb)
        {
            AFCronData* pCron = ARK_NEW AFCronData();
            memset(pCron, 0, sizeof(*pCron));

            if (!pCron->Parse(cron_expression))
            {
                ARK_DELETE(pCron);
                return false;
            }

            pCron->cron_id = cron_id;
            pCron->user_data = user_arg;
            pCron->callback = cb;
            pCron->GetNext(now);
            mxCronList.push_back(pCron);

            //Sort Crons by next_time
            SortCrons();
            return true;
        }

        int RemoveCron(int cron_id)
        {
            int count = 0;

            for (auto it : mxCronList)
            {
                if (it->cron_id == cron_id)
                {
                    it->delete_flag = true;
                    mbCronWaitDelete = true;
                    ++count;
                }
            }

            return count;
        }

        void Clear()
        {
            for (auto it : mxCronList)
            {
                ARK_DELETE(it);
            }

            mxCronList.clear();
        }

    protected:
        void SortCrons()
        {
            mxCronList.sort(AFCronData::AFCronSorter());
        }

    private:
        using CronDataList = std::list<AFCronData*>;
        CronDataList mxCronList;
        bool mbCronWaitDelete = false;
    };

}