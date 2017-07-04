// -------------------------------------------------------------------------
//    @FileName         :    NFIModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    NFIModule
//
// -------------------------------------------------------------------------

#ifndef NFI_LOGIC_MODULE_H
#define NFI_LOGIC_MODULE_H

#include <string>
#include "SDK/Core/NFMap.h"
#include "SDK/Core/NFList.h"
#include "SDK/Core/NFDefine.h"
#include "SDK/Core/AFCDataList.h"
#include "SDK/Core/NFIRecord.h"
#include "SDK/EventDefine/NFEventDefine.h"

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

class NFIPluginManager;

class NFIModule
{
public:
    NFIModule()
    {
        mbReloading = false;
    }

    virtual ~NFIModule() {}

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

    virtual NFIPluginManager* GetPluginManager() const
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
    NFIPluginManager* pPluginManager;
    bool mbReloading;
};
#endif
