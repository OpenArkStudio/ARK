// -------------------------------------------------------------------------
//    @FileName         :    AFIModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFIModule
//
// -------------------------------------------------------------------------

#ifndef AFI_LOGIC_MODULE_H
#define AFI_LOGIC_MODULE_H

#include <string>
#include "SDK/Core/AFMap.h"
#include "SDK/Core/AFList.h"
#include "SDK/Core/AFDefine.h"
#include "SDK/Core/AFCDataList.h"
#include "SDK/Core/AFIRecord.h"
#include "SDK/Core/Math/Vector3.hpp"
#include "SDK/EventDefine/AFEventDefine.h"

template<typename DerivedType, typename BaseType>
class TIsDerived
{
public:
    static int AnyFunction(BaseType* base)
    {
        return 1;
    }

    static  char AnyFunction(void* t2)
    {
        return 0;
    }

    enum
    {
        Result = (sizeof(int) == sizeof(AnyFunction((DerivedType*)NULL))),
    };
};

class AFIPluginManager;

class AFIModule
{
public:
    AFIModule()
    {
        mbReloading = false;
    }

    virtual ~AFIModule() {}

    virtual bool Init()
    {

        return true;
    }

    virtual bool AfterInit()
    {
        return true;
    }

    virtual bool CheckConfig()
    {
        return true;
    }

    virtual bool BeforeShut()
    {
        return true;
    }

    virtual bool Shut()
    {
        return true;
    }

    virtual bool Execute()
    {
        return true;
    }

    virtual bool StartReLoadState()
    {
        mbReloading = true;
        return true;
    }

    virtual bool EndReLoadState()
    {
        mbReloading = false;
        return true;
    }

    virtual AFIPluginManager* GetPluginManager() const
    {
        return pPluginManager;
    }

    bool Loading() const
    {
        return mbReloading;
    }
public:
    std::string strName;


protected:
    AFIPluginManager* pPluginManager;
    bool mbReloading;
};
#endif
