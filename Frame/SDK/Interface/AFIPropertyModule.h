// -------------------------------------------------------------------------
//    @FileName         ：    AFIPropertyModule.h
//    @Author           ：    Ark Game Tech
//    @Date             ：    2012-12-15
//    @Module           ：    AFIPropertyModule
//
// -------------------------------------------------------------------------

#ifndef AFI_PROPERTY_MODULE_H
#define AFI_PROPERTY_MODULE_H

#include <iostream>
#include "AFIModule.h"

class AFIPropertyModule
    : public AFIModule
{
public:
    enum NFPropertyGroup
    {
        NPG_JOBLEVEL        ,//职业等级基础
        NPG_EFFECTVALUE     ,//一级属性影响二级属性的部分(直接用公式计算，其他部分不用动)
        NPG_REBIRTH_ADD     ,//重生奖励
        NPG_EQUIP           ,//装备不会影响体质之类的导致2次计算(直接不要力量体质什么的)
        NPG_EQUIP_AWARD     ,//套装奖励
        NPG_STATIC_BUFF     ,//永久类型的BUFF，天赋之类的，或者吃的永久药水

        NPG_RUNTIME_BUFF    ,//动态BUFF

        NPG_ALL ,
    };


    virtual int RefreshBaseProperty(const AFGUID& self) = 0;
    virtual int GetPropertyValue(const AFGUID& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType) = 0;
    virtual int SetPropertyValue(const AFGUID& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType, const int nValue) = 0;
    virtual int AddPropertyValue(const AFGUID& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType, const int nValue) = 0;
    virtual int SubPropertyValue(const AFGUID& self, const std::string& strPropertyName, const NFPropertyGroup eGroupType, const int nValue) = 0;

    virtual bool FullHPMP(const AFGUID& self) = 0;
    virtual bool AddHP(const AFGUID& self, const AFINT64& nValue) = 0;
    virtual bool ConsumeHP(const AFGUID& self, const AFINT64& nValue) = 0;
    virtual bool EnoughHP(const AFGUID& self, const AFINT64& nValue) = 0;

    virtual bool AddMP(const AFGUID& self, const AFINT64& nValue) = 0;
    virtual bool ConsumeMP(const AFGUID& self, const AFINT64& nValue) = 0;
    virtual bool EnoughMP(const AFGUID& self, const AFINT64& nValue) = 0;

    virtual bool FullSP(const AFGUID& self) = 0;
    virtual bool AddSP(const AFGUID& self, const AFINT64& nValue) = 0;
    virtual bool ConsumeSP(const AFGUID& self, const AFINT64& nValue) = 0;
    virtual bool EnoughSP(const AFGUID& self, const AFINT64& nValue) = 0;

    virtual bool AddMoney(const AFGUID& self, const AFINT64& nValue) = 0;
    virtual bool ConsumeMoney(const AFGUID& self, const AFINT64& nValue) = 0;
    virtual bool EnoughMoney(const AFGUID& self, const AFINT64& nValue) = 0;

    virtual bool AddDiamond(const AFGUID& self, const AFINT64& nValue) = 0;
    virtual bool ConsumeDiamond(const AFGUID& self, const AFINT64& nValue) = 0;
    virtual bool EnoughDiamond(const AFGUID& self, const AFINT64& nValue) = 0;
};

#endif