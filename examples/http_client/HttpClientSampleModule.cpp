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

#include <map>
#include "HttpClientSampleModule.h"

using namespace std;

namespace ark {

bool HttpClientSampleModule::Init()
{
    httpCli_ = FindModule<AFIHttpClientModule>();
    etcdCli_ = FindModule<AFIEtcdClientModule>()->CreateEtcdClient(vector<string>{ "172.20.3.74:2379" /*, "172.20.3.75:2379", "172.20.3.76:2379" */});
    return httpCli_ != nullptr && etcdCli_ != nullptr;
}

bool HttpClientSampleModule::PostInit()
{
    std::cout << "Start testing Http Client" << endl;

    string body = "{\"key\": \"Zm9v\", \"value\": \"YmFy\"}";
    auto future = httpCli_->Post("172.20.3.74", 2379, "/v3/kv/put", nullptr, &body);
    auto val = future.get();
    std::cout << this_thread::get_id() << "    " << val.first << "    " << val.second << endl;

    httpCli_->Post([](pair<bool, string> rsp) {
      cout << this_thread::get_id() << "    " << rsp.first << "    " << rsp.second << endl;
    }, "172.20.3.74", 2379, "/v3/kv/put", nullptr, &body);

    etcdCli_->Grant([](pair<int64_t, string> rsp) {
      cout << rsp.first << "  |  " << rsp.second << endl;
    }, 100);

    std::cout << "Finish testing Http Client" << endl;
    return true;
}

} // namespace ark
