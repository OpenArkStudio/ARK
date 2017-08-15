#include "SDK/Base/AFPlatform.hpp"

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "AFNetEvpp_d.lib" )
#pragma comment( lib, "Core_d.lib" )
#pragma comment( lib, "Proto_d.lib" )
#pragma comment( lib, "libprotobuf_d.lib" )
#pragma comment( lib, "event_core.lib" )


#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == NF_PLATFORM_ANDROID

#pragma comment( lib, "AFNetEvpp_d.a" )
#pragma comment( lib, "Core_d.a" )
#pragma comment( lib, "Proto_d.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#else

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "AFNetEvpp.lib" )
#pragma comment( lib, "Core.lib" )
#pragma comment( lib, "Proto.lib" )
#pragma comment( lib, "libprotobuf.lib" )
#pragma comment( lib, "event_core.lib" )

#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "AFNetEvpp.a" )
#pragma comment( lib, "Core.a" )
#pragma comment( lib, "Proto.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#endif
