// -------------------------------------------------------------------------
//    @FileName			:    AFCDynLib.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCDynLib
//
// -------------------------------------------------------------------------

#include "AFCDynLib.h"

bool AFCDynLib::Load()
{
    std::string strLibPath = "./";
    strLibPath += mstrName;
    mInst = (DYNLIB_HANDLE)DYNLIB_LOAD(strLibPath.c_str());

    return mInst != NULL;
}

bool AFCDynLib::UnLoad()
{
    DYNLIB_UNLOAD(mInst);
    return true;
}

void* AFCDynLib::GetSymbol(const char* szProcName)
{
    return (DYNLIB_HANDLE)DYNLIB_GETSYM(mInst, szProcName);
}
