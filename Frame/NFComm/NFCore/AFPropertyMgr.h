// -------------------------------------------------------------------------
//    @FileName         :    AFCPropertyMgr.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    AFCPropertyMgr
//
// -------------------------------------------------------------------------

#ifndef NFC_PROPERTYMANAGER_H
#define NFC_PROPERTYMANAGER_H

#include "NFDefine.h"
#include "NFMap.h"
#include "AFDataList.hpp"
#include "AFCoreDef.hpp"
#include "AFArrayPod.hpp"
#include "AFStringPod.hpp"
#include "AFString.hpp"

struct AFProperty
{
    AFString<char, 16, CharTraits<char>, CoreAlloc> name; //属性名，最大16个字符
    AFDataList::TData value;    //属性值
};

class AFPropertyMgr
{
public:

    AFPropertyMgr();
    ~AFPropertyMgr();
    void Clear();

    bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb);
    bool AddProperty(const char* name, const AFDataList::TData& value);

    bool SetProperty(const char* name, const AFDataList::TData& value);
    bool SetPropertyInt(const char* name, const NFINT64 value);
    bool SetPropertyDouble(const char* name, const double value);
    bool SetPropertyString(const char* name, const std::string& value);
    bool SetPropertyObject(const char* name, const NFGUID& value);
    bool SetPropertyPoint(const char* name, const Point3D& value);
    NFINT64 GetPropertyInt(const char* name);
    double GetPropertyDouble(const char* name);
    const std::string& GetPropertyString(const char* name);
    const NFGUID& GetPropertyObject(const char* name);
    const Point3D& GetPropertyPoint(const char* name);

protected:
    bool FindIndex(const char* name, size_t& index);

private:
    ArraryPod<AFProperty*, 1, CoreAlloc> mxPropertys;
    StringPod<char, size_t, StringTraits<char>, CoreAlloc> mxIndices;
};


#endif
