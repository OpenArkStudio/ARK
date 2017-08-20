#include "SDK/Base/AFPlatform.hpp"

#ifdef _DEBUG

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "AFCore_d.lib" )
#pragma comment( lib, "AFProto_d.lib" )
#pragma comment( lib, "libprotobuf_d.lib" )
//#pragma comment( lib, "libglog_static_d.lib" )


#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == AF_PLATFORM_ANDROID
#pragma comment( lib, "AFCore_d.a" )
#pragma comment( lib, "AFProto_d.a" )
//#pragma comment( lib, "libglog_d.lib" )

#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == AF_PLATFORM_APPLE_IOS
#endif

#else

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "AFCore.lib" )
#pragma comment( lib, "AFProto.lib" )
#pragma comment( lib, "libprotobuf.lib" )

#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == AF_PLATFORM_ANDROID
#pragma comment( lib, "AFCore.a" )
#pragma comment( lib, "AFProto.a" )
#pragma comment( lib, "libprotobuf.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == AF_PLATFORM_APPLE_IOS
#endif

#endif
