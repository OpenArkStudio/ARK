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

#ifndef ARK_HAS_PROTOBUF
#error "Enable ARK_BUILD_PROTO and consume ark::proto to use protocol messages"
#endif

#include "ark/proto/AFCSMsg.pb.h"
#include "ark/proto/AFCommonMsgNew.pb.h"
#include "ark/proto/AFConsul.pb.h"
#include "ark/proto/AFDBMsg.pb.h"
#include "ark/proto/AFMsgMysql.pb.h"
#include "ark/proto/AFMsgURl.pb.h"
#include "ark/proto/AFSSMsg.pb.h"
#include "google/protobuf/util/json_util.h"
