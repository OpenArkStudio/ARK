// -------------------------------------------------------------------------
//    @FileName			:    NFCPropertyModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-07-05
//    @Module           :    NFCPropertyModule
//
// -------------------------------------------------------------------------

#ifndef NFC_PROPERTY_MODULE_H
#define NFC_PROPERTY_MODULE_H

#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIGameLogicModule.h"
#include "SDK/Interface/NFIPropertyModule.h"
#include "SDK/Interface/NFIElementModule.h"
#include "SDK/Interface/NFIClassModule.h"
#include "SDK/Interface/NFIPropertyConfigModule.h"
#include "SDK/Interface/NFIPluginManager.h"
#include "NFComm/NFMessageDefine/NFProtocolDefine.hpp"
#include "SDK/Interface/NFILevelModule.h"

class NFCPropertyModule : public NFIPropertyModule
{
public:
    NFCPropertyModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~NFCPropertyModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual int RefreshBaseProperty(const AFGUID& self);

    virtual int GetPropertyValue(const AFGUID& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType);
    virtual int SetPropertyValue(const AFGUID& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType, const int nValue);

    virtual int AddPropertyValue(const AFGUID& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType, const int nValue);
    virtual int SubPropertyValue(const AFGUID& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType, const int nValue);

    virtual bool FullHPMP(const AFGUID& self);
    virtual bool AddHP(const AFGUID& self, const NFINT64& nValue);
    virtual bool ConsumeHP(const AFGUID& self, const NFINT64& nValue);
    virtual bool EnoughHP(const AFGUID& self, const NFINT64& nValue);

    virtual bool AddMP(const AFGUID& self, const NFINT64& nValue);
    virtual bool ConsumeMP(const AFGUID& self, const NFINT64& nValue);
    virtual bool EnoughMP(const AFGUID& self, const NFINT64& nValue);

    virtual bool FullSP(const AFGUID& self);
    virtual bool AddSP(const AFGUID& self, const NFINT64& nValue);
    virtual bool ConsumeSP(const AFGUID& self, const NFINT64& nValue);
    virtual bool EnoughSP(const AFGUID& self, const NFINT64& nValue);

    virtual bool AddMoney(const AFGUID& self, const NFINT64& nValue);
    virtual bool ConsumeMoney(const AFGUID& self, const NFINT64& nValue);
    virtual bool EnoughMoney(const AFGUID& self, const NFINT64& nValue);

    virtual bool AddDiamond(const AFGUID& self, const NFINT64& nValue);
    virtual bool ConsumeDiamond(const AFGUID& self, const NFINT64& nValue);
    virtual bool EnoughDiamond(const AFGUID& self, const NFINT64& nValue);

protected:
    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFDataList& var);

    int OnObjectLevelEvent(const AFGUID& self, const std::string& strPropertyName, const AFDataList::TData& oldVar, const AFDataList::TData& newVar);

    int OnRecordPropertyEvent(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFDataList::TData& oldVar, const AFDataList::TData& newVar);

private:
    NFIKernelModule* m_pKernelModule;
    NFIPropertyConfigModule* m_pPropertyConfigModule;
    NFIElementModule* m_pElementModule;
    NFIClassModule* m_pClassModule;
    NFILevelModule* m_pLevelModule;
};


#endif
