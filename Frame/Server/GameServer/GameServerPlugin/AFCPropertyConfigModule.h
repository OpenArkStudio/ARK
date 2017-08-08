// -------------------------------------------------------------------------
//    @FileName         :    AFCPropertyConfigModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-09-30
//    @Module           :    AFCPropertyConfigModule
//
// -------------------------------------------------------------------------

#ifndef AFC_PROPERTY_COAFIG_MODULE_H
#define AFC_PROPERTY_COAFIG_MODULE_H

#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_iterators.hpp"
#include "RapidXML/rapidxml_print.hpp"
#include "RapidXML/rapidxml_utils.hpp"
#include "SDK/Interface/AFIPropertyConfigModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Proto/NFProtocolDefine.hpp"

class AFCPropertyConfigModule
    : public AFIPropertyConfigModule
{
public:
    AFCPropertyConfigModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCPropertyConfigModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual int CalculateBaseValue(const int nJob, const int nLevel, const std::string& strProperty);
    virtual bool LegalLevel(const int nJob, const int nLevel);

protected:
    void Load();

private:
    //
    //diffent job, diffrent PropertyID[Level->EffectData]
    AFMapEx<int, AFMapEx<int, std::string> > mhtCoefficienData;

    AFIClassModule* m_pClassModule;
    AFIElementModule* m_pElementModule;
};


#endif
