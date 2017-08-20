#include "SDK/Base/AFPlatform.hpp"

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "AFCore_d.lib" )
#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == AF_PLATFORM_ANDROID
#pragma comment( lib, "AFCore_d.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == AF_PLATFORM_APPLE_IOS
#endif

#else

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "AFCore.lib" )
#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == AF_PLATFORM_ANDROID
#pragma comment( lib, "AFCore.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == AF_PLATFORM_APPLE_IOS
#endif

#endif