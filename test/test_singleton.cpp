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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "base/pattern/AFSingleton.hpp"

using namespace ark;

class test_singleton : public AFSingleton<test_singleton>
{
public:
    int value;
};

TEST_CASE("singleton", "[singleton]")
{
    test_singleton* pl = test_singleton::instance();
    test_singleton& pr = test_singleton::get_instance();

    pl->value = 1024;

    REQUIRE(pr.value == 1024);
    REQUIRE(&pr == pl);
}
