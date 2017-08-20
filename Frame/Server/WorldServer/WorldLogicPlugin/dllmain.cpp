#include "SDK/Base/AFPlatform.hpp"

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "AFCore_d.lib" )
#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == AF_PLATFORM_ANDROID
////#pragma comment( lib, "RakNet_LibStatic_Debug_x64.a" )
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
#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == AF_PLATFORM_ANDROID
//#pragma comment( lib, "RakNet_LibStatic_Release_x64.a" )
#pragma comment( lib, "AFNet.a" )
#pragma comment( lib, "AFCore.a" )
#pragma comment( lib, "AFProto.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == AF_PLATFORM_APPLE_IOS
#endif

#endif
