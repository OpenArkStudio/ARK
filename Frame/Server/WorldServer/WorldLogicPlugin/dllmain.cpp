#include "SDK/Interface/AFPlatform.h"

#ifdef NF_DEBUG_MODE

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "Core_d.lib" )
#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
////#pragma comment( lib, "RakNet_LibStatic_Debug_x64.a" )
#pragma comment( lib, "AFNetEvpp_d.a" )
#pragma comment( lib, "Core_d.a" )
#pragma comment( lib, "Proto_d.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#else

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "AFNetEvpp.lib" )
#pragma comment( lib, "Core.lib" )
#pragma comment( lib, "Proto.lib" )
#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
//#pragma comment( lib, "RakNet_LibStatic_Release_x64.a" )
#pragma comment( lib, "AFNetEvpp.a" )
#pragma comment( lib, "Core.a" )
#pragma comment( lib, "Proto.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#endif
