#include "SDK/Base/AFPlatform.hpp"

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "Core_d.lib" )
#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "Core_d.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#else

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "Core.lib" )
#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "Core.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#endif