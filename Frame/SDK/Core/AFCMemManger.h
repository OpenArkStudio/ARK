#ifndef AFC_AFCMEMMANGER_H
#define AFC_AFCMEMMANGER_H

#include "SDK/Interface/AFPlatform.h"
#include "AFSingleton.h"

//#define  NF_USE_TCMALLOC

#ifdef NF_USE_TCMALLOC

#ifdef NF_DEBUG_MODE

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "libtcmalloc_minimal_d.lib" )
#pragma comment(linker, "/include:__tcmalloc")
#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "libtcmalloc_minimal.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#else

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "libtcmalloc_minimal.lib" )
#pragma comment(linker, "/include:__tcmalloc")
#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "libtcmalloc_minimal.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#endif // NF_USE_TCMALLOC

#endif

class AFCMemManger: public NFSingleton<AFCMemManger>
{
public:
    AFCMemManger();
    ~AFCMemManger();

public:
    virtual void FreeMem();

private:

};

#endif