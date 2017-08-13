// -------------------------------------------------------------------------
//    @FileName			:    AFCDynLib.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCDynLib
//
// -------------------------------------------------------------------------

#ifndef AFC_DYNLIB_H
#define AFC_DYNLIB_H

#include <stdio.h>
#include <iostream>
#include "SDK/Base/AFPlatform.hpp"

#if ARK_PLATFORM == PLATFORM_WIN
#    define DYNLIB_HANDLE hInstance
#    define DYNLIB_LOAD( a ) LoadLibraryEx( a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH )
#    define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#    define DYNLIB_UNLOAD( a ) FreeLibrary( a )

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == NF_PLATFORM_ANDROID
#include <dlfcn.h>
#define DYNLIB_HANDLE void*
#define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
#define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#define DYNLIB_UNLOAD( a ) dlclose( a )

#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == NF_PLATFORM_APPLE_IOS
#define DYNLIB_HANDLE void*
#define DYNLIB_LOAD( a ) mac_loadDylib( a )
#define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#define DYNLIB_UNLOAD( a ) dlclose( a )

#endif

class AFCDynLib
{

public:

    AFCDynLib(const std::string& strName)
    {
        mbMain = false;
        mstrName = strName;
#ifdef ARK_RUN_MODE_DEBUG
        mstrName.append("_d");
#endif

#if ARK_PLATFORM == PLATFORM_WIN
        mstrName.append(".dll");
#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == NF_PLATFORM_ANDROID
        mstrName.append(".so");
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == NF_PLATFORM_APPLE_IOS
        mstrName.append(".so");
#endif

        printf("LoadPlugin:%s\n", mstrName.c_str());
    }

    ~AFCDynLib()
    {

    }

    bool Load();

    bool UnLoad();

    /// Get the name of the library
    const std::string& GetName(void) const
    {
        return mstrName;
    }

    const bool GetMain(void) const
    {
        return mbMain;
    }

    void* GetSymbol(const char* szProcName);

protected:

    std::string mstrName;
    bool mbMain;

    DYNLIB_HANDLE mInst;
};

#endif
