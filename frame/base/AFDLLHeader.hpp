/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "AFPlatform.hpp"

#if ARK_PLATFORM == PLATFORM_WIN

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "ws2_32")

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG

#pragma comment(lib, "brynetd.lib")
#pragma comment(lib, "libprotobuf-lited.lib")
#pragma comment(lib, "libprotobufd.lib")
#pragma comment(lib, "AFCore_d.lib")
#pragma comment(lib, "AFProto_d.lib")

#else

#pragma comment(lib, "brynet.lib")
#pragma comment(lib, "libprotobuf-lite.lib")
#pragma comment(lib, "libprotobuf.lib")
#pragma comment(lib, "AFCore.lib")
#pragma comment(lib, "AFProto.lib")

#endif

#endif