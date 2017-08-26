/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file  	dllmain.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     dllmain                                                  *
*****************************************************************************/
#include "SDK/Base/AFPlatform.hpp"

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
//#pragma comment( lib, "RakNet_LibStatic_Debug_x64.lib" )
#pragma comment( lib, "AFNet_d.lib" )
#pragma comment( lib, "AFCore_d.lib" )
#pragma comment( lib, "AFProto_d.lib" )
#pragma comment( lib, "libprotobuf_d.lib" )
#pragma comment( lib, "event_core.lib" )

#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == AF_PLATFORM_ANDROID
//#pragma comment( lib, "RakNet_LibStatic_Debug_x64.a" )
#pragma comment( lib, "AFNet_d.a" )
#pragma comment( lib, "AFCore_d.a" )
#pragma comment( lib, "AFProto_d.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == AF_PLATFORM_APPLE_IOS
#endif

#else

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "AFNet.lib" )
#pragma comment( lib, "AFCore.lib" )
#pragma comment( lib, "AFProto.lib" )
#pragma comment( lib, "libprotobuf.lib" )
#pragma comment( lib, "event_core.lib" )

#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == AF_PLATFORM_ANDROID
#pragma comment( lib, "AFNet.a" )
#pragma comment( lib, "AFCore.a" )
#pragma comment( lib, "AFProto.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == AF_PLATFORM_APPLE_IOS
#endif

#endif

