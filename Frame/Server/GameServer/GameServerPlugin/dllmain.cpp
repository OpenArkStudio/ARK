#include "SDK/Base/AFPlatform.hpp"

#ifdef _DEBUG

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "Core_d.lib" )
#pragma comment( lib, "Proto_d.lib" )
#pragma comment( lib, "libprotobuf_d.lib" )
//#pragma comment( lib, "libglog_static_d.lib" )


#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "Core_d.a" )
#pragma comment( lib, "Proto_d.a" )
//#pragma comment( lib, "libglog_d.lib" )

#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#else

#if ARK_PLATFORM == PLATFORM_WIN
#pragma comment( lib, "Core.lib" )
#pragma comment( lib, "Proto.lib" )
#pragma comment( lib, "libprotobuf.lib" )

#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "Core.a" )
#pragma comment( lib, "Proto.a" )
#pragma comment( lib, "libprotobuf.a" )
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#endif
