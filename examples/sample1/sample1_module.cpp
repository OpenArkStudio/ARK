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

#include "ark/kernel/data_impl.hpp"
#include "ark/macros.hpp"
#include "ark/time/date_time.hpp"
#include "ark/utility/scheduler.hpp"
#include "sample1_module.hpp"
#include "sample1_plugin.hpp"

namespace ark {

std::unique_ptr<sample1_module> make_sample1_module() {
    return std::make_unique<sample1_module>();
}

bool sample1_module::init() {
    timer_module_ = find_module<timer_module>();
    guid_module_ = find_module<guid_module>();

    ARK_LOG_INFO("{}, init", GET_CLASS_NAME(sample1_module));

    return true;
}

void test_basic_data() {
    // Test basic data
    guid_t test_id = guid_t(1);

    // test GUID type
    data_impl guid_data1(data_type::dt_int64, guid_t(1));
    data_impl guid_data2(data_type::dt_int64, guid_t(0));
    guid_data1 = guid_data2;
    guid_t guid = guid_data1.get_int64();

    // test string type
    data_impl data1(data_type::dt_string, "test1");
    data_impl data2(data_type::dt_string, "test2");
    data1 = data2;
    const char* str1 = data1.get_string();
}

void sample1_module::test_cron_scheduler() {
    static scheduler scheduler;
    date_time* now = ARK_NEW date_time();

    scheduler.cron("* * * * *", [now]() {
        date_time cur;
        date_time::time_diff diff = (cur - *now);

        std::cout << diff << std::endl;
        *now = cur;
    });
}

void sample1_module::timer_test(uint64_t timer_id, const guid_t& entity_id) {
    ARK_LOG_INFO("timer log, id = {} entity_id = {}", timer_id, entity_id);
}

bool sample1_module::post_init() {
    ARK_LOG_INFO("{}, PostInit", GET_CLASS_NAME(sample1_module));

    timer_module_->add_single_timer(guid_t(100), std::chrono::seconds(10), 5, this, &sample1_module::timer_test);
    //TestCronScheduler();

    return true;
}

bool sample1_module::update() {
    return true;
}

bool sample1_module::pre_shut() {
    ARK_LOG_INFO("{}, PreShut", GET_CLASS_NAME(sample1_module));
    return true;
}

bool sample1_module::shut() {
    ARK_LOG_INFO("{}, Shut", GET_CLASS_NAME(sample1_module));
    return true;
}

} // namespace ark
