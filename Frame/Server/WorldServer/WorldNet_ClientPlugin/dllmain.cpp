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

#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == NF_PLATFORM_ANDROID
////#pragma comment( lib, "RakNet_LibStatic_Debug_x64.a" )
#pragma comment( lib, "AFNet_d.a" )
#pragma comment( lib, "AFCore_d.a" )
#pragma comment( lib, "AFProto_d.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#else

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "AFNet.lib" )
#pragma comment( lib, "AFCore.lib" )
#pragma comment( lib, "AFProto.lib" )
#pragma comment( lib, "libprotobuf.lib" )
#pragma comment( lib, "event_core.lib" )

#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "AFNet.a" )
#pragma comment( lib, "AFCore.a" )
#pragma comment( lib, "AFProto.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#endif
