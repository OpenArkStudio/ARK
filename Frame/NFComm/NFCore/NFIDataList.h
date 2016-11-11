// -------------------------------------------------------------------------
//    @FileName         :    NFIDataList.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFIDataList
//
// -------------------------------------------------------------------------

#ifndef NFI_DATALIST_H
#define NFI_DATALIST_H

#include <string>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <assert.h>
#include <map>
#include <typeinfo>
#include <iomanip>
#include <functional>
#include <iostream>
#include <fstream>
#include <memory>
#include "common/variant.hpp"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "Math/Vector3.hpp"

//变量类型
enum TDATA_TYPE
{
    TDATA_UNKNOWN,  // 未知
    TDATA_INT,      // int64_t
    TDATA_DOUBLE,   // double
    TDATA_STRING,   // string
    TDATA_OBJECT,   // NFGUID
    TDATA_POINT,    // Point3D
    TDATA_MAX,
};

//////////////////////////////////////////////////////////////////////////
//default value
const static NFINT64 NULL_INT = 0;
const static double NULL_DOUBLE = 0.0;
const static std::string NULL_STR = "";
const static NFGUID NULL_GUID = NULL_GUID;
const static Point3D NULL_POINT = Point3D();
//////////////////////////////////////////////////////////////////////////
//类型接口
class NFIDataList
{
public:
    struct TData
    {
    public:
        TData()
        {
            nType = TDATA_UNKNOWN;
        }

        TData(TDATA_TYPE eType)
        {
            nType = eType;
        }

        TData(const TData& value)
        {
            nType = value.nType;
            variantData = value.variantData;
        }

        ~TData()
        {
            nType = TDATA_UNKNOWN;
        }

        inline bool operator==(const TData& src) const
        {
            //&& src.variantData == variantData
            if(src.GetType() == GetType())
            {
                switch(GetType())
                {
                case TDATA_INT:
                    {
                        if(src.GetInt() == GetInt())
                        {
                            return true;
                        }
                    }
                    break;
                case TDATA_DOUBLE:
                    {
                        double dValue = GetDouble() - src.GetDouble();
                        if(IsZeroDouble(dValue))
                        {
                            return true;
                        }
                    }
                    break;
                case TDATA_STRING:
                    {
                        if(src.GetString() == GetString())
                        {
                            return true;
                        }
                    }
                    break;
                case TDATA_OBJECT:
                    {
                        if(src.GetObject() == GetObject())
                        {
                            return true;
                        }
                    }
                    break;
                case TDATA_POINT:
                    {
                        if(src.GetPoint() == GetPoint())
                        {
                            return true;
                        }
                    }
                    break;
                default:
                    break;
                }
            }

            return false;
        }

        void Reset()
        {
            nType = TDATA_UNKNOWN;
        }

        bool IsNullValue() const
        {
            bool bChanged = false;

            switch(GetType())
            {
            case TDATA_INT:
                {
                    if(0 != GetInt())
                    {
                        bChanged = true;
                    }
                }
                break;
            case TDATA_DOUBLE:
                {
                    double dValue = GetDouble();
                    if(!IsZeroDouble(dValue))
                    {
                        bChanged = true;
                    }
                }
                break;
            case TDATA_STRING:
                {
                    const std::string& strData = GetString();
                    if(!strData.empty())
                    {
                        bChanged = true;
                    }
                }
                break;
            case TDATA_OBJECT:
                {
                    if(!GetObject().IsNull())
                    {
                        bChanged = true;
                    }
                }
                break;
            case TDATA_POINT:
                {
                    if(!GetPoint().IsZero())
                    {
                        bChanged = true;
                    }
                }
                break;
            default:
                break;
            }

            return !bChanged;
        }

        TDATA_TYPE GetType() const
        {
            return nType;
        }

        // 设置值，类型必须和之前一致
        void SetInt(const NFINT64 var)
        {
            if(nType == TDATA_INT || TDATA_UNKNOWN == nType)
            {
                nType = TDATA_INT;
                variantData = (NFINT64)var;
            }
        }

        void SetDouble(const double var)
        {
            if(nType == TDATA_DOUBLE || TDATA_UNKNOWN == nType)
            {
                nType = TDATA_DOUBLE;
                variantData = (double)var;
            }
        }

        void SetString(const std::string& var)
        {
            if(nType == TDATA_STRING || TDATA_UNKNOWN == nType)
            {
                nType = TDATA_STRING;
                variantData = (std::string)var;
            }
        }

        void SetObject(const NFGUID& var)
        {
            if(nType == TDATA_OBJECT || TDATA_UNKNOWN == nType)
            {
                nType = TDATA_OBJECT;
                variantData = (NFGUID)var;
            }
        }

        void SetPoint(const Point3D& var)
        {
            if(nType == TDATA_POINT || TDATA_UNKNOWN == nType)
            {
                nType = TDATA_POINT;
                variantData = (Point3D)var;
            }
        }

        NFINT64 GetInt() const
        {
            if(TDATA_INT == nType)
            {
                //return boost::get<NFINT64>(variantData);
                return variantData.get<NFINT64>();
            }

            return NULL_INT;
        }

        double GetDouble() const
        {
            if(TDATA_DOUBLE == nType)
            {
                //return boost::get<double>(variantData);
                return variantData.get<double>();
            }

            return NULL_DOUBLE;
        }
        const std::string& GetString() const
        {
            if(TDATA_STRING == nType)
            {
                //return boost::get<const std::string&>(variantData);
                return variantData.get<std::string>();
            }

            return NULL_STR;
        }

        const char* GetCharArr() const
        {
            if(TDATA_STRING == nType)
            {
                //return boost::get<const std::string&>(variantData);
                return variantData.get<std::string>().c_str();
            }

            return NULL_STR.c_str();
        }

        const NFGUID& GetObject() const
        {
            if(TDATA_OBJECT == nType)
            {
                //return boost::get<const NFGUID&>(variantData);
                return variantData.get<NFGUID>();
            }

            return NULL_GUID;
        }

        const Point3D& GetPoint() const
        {
            if(TDATA_POINT == nType)
            {
                return variantData.get<Point3D>();
            }

            return NULL_POINT;
        }

        std::string StringValEx() const
        {
            std::string strData;

            switch(nType)
            {
            case TDATA_INT:
                strData = lexical_cast<std::string> (GetInt());
                break;

            case TDATA_DOUBLE:
                strData = lexical_cast<std::string> (GetDouble());
                break;

            case TDATA_STRING:
                strData = GetString();
                break;

            case TDATA_OBJECT:
                strData = GetObject().ToString();
                break;
            case TDATA_POINT:
                strData = GetPoint().ToString();
                break;
            default:
                strData = NULL_STR;
                break;
            }
            return strData;
        }

    private:

        TDATA_TYPE nType;

    public:
        mapbox::util::variant<NFINT64, double, std::string, NFGUID, Point3D> variantData;
    };

    NFIDataList()
    {
        mnUseSize = 0;
        mvList.reserve(STACK_SIZE);
        for(int i = 0; i < STACK_SIZE; ++i)
        {
            mvList.push_back(NF_SHARE_PTR<TData>(NF_NEW TData()));
        }
    }

    virtual ~NFIDataList() = 0;

    virtual std::string StringValEx(const int index) const = 0;
    virtual bool ToString(std::string& str, const std::string& strSplit) const = 0;

public:

    virtual const NF_SHARE_PTR<TData> GetStack(const int index) const = 0;

    // 合并
    virtual bool Concat(const NFIDataList& src) = 0;
    // 部分添加
    virtual bool Append(const NFIDataList& src) = 0;
    virtual bool Append(const NFIDataList& src, const int start, const int count) = 0;
    // 部分添加
    virtual bool Append(const NFIDataList::TData& sTData) = 0;
    // 清空
    virtual void Clear() = 0;
    // 是否为空
    virtual bool IsEmpty() const = 0;
    // 数据数量
    virtual int GetCount() const = 0;
    // 数据类型
    virtual TDATA_TYPE Type(const int index) const = 0;
    //数据类型检测
    virtual bool TypeEx(const  int nType, ...) const = 0;
    //新进入拆分
    virtual bool Split(const std::string& str, const std::string& strSplit) = 0;

    // 添加数据
    virtual bool Add(const NFINT64 value) = 0;
    virtual bool Add(const double value) = 0;
    virtual bool Add(const std::string& value) = 0;
    virtual bool Add(const NFGUID& value) = 0;
    virtual bool Add(const Point3D& value) = 0;

    virtual bool Set(const int index, const NFINT64 value) = 0;
    virtual bool Set(const int index, const double value) = 0;
    virtual bool Set(const int index, const std::string& value) = 0;
    virtual bool Set(const int index, const NFGUID& value) = 0;
    virtual bool Set(const int index, const Point3D& value) = 0;

    // 获得数据
    virtual NFINT64 Int(const int index) const = 0;
    virtual double Double(const int index) const = 0;
    virtual const std::string& String(const int index) const = 0;
    virtual const NFGUID& Object(const int index) const = 0;
    virtual const Point3D& Point(const int index) const = 0;

    bool AddInt(const NFINT64 value)
    {
        return Add(value);
    }
    bool AddDouble(const double value)
    {
        return Add(value);
    }
    bool AddString(const std::string& value)
    {
        return Add(value);
    }
    bool AddStringFromChar(const char* value)
    {
        return Add(value);
    }
    bool AddObject(const NFGUID& value)
    {
        return Add(value);
    }
    bool AddPoint(const Point3D& value)
    {
        return Add(value);
    }

    bool SetInt(const int index, const NFINT64 value)
    {
        return Set(index, value);
    }
    bool SetFloat(const int index, const double value)
    {
        return Set(index, value);
    }
    bool SetString(const int index, const std::string& value)
    {
        return Set(index, value);
    }
    bool SetObject(const int index, const NFGUID& value)
    {
        return Set(index, value);
    }
    bool SetPoint(const int index, const Point3D& value)
    {
        return Set(index, value);
    }

    inline bool Compare(const int nPos, const NFIDataList& src) const
    {
        if(src.GetCount() > nPos
                && GetCount() > nPos
                && src.Type(nPos) == Type(nPos))
        {
            switch(src.Type(nPos))
            {
            case TDATA_INT:
                return Int(nPos) == src.Int(nPos);
                break;
            case TDATA_DOUBLE:
                return IsZeroDouble(Double(nPos) - src.Double(nPos));
                break;
            case TDATA_STRING:
                return String(nPos) == src.String(nPos);
                break;
            case TDATA_OBJECT:
                return Object(nPos) == src.Object(nPos);
                break;
            case TDATA_POINT:
                return Point(nPos) == src.Point(nPos);
                break;
            default:
                return false;
                break;
            }
        }

        return false;
    }


    inline bool operator==(const NFIDataList& src) const
    {
        if(src.GetCount() == GetCount())
        {
            for(int i = 0; i < GetCount(); i++)
            {
                if(!Compare(i, src))
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    inline bool operator!=(const NFIDataList& src)
    {
        return !(*this == src);
    }
    inline NFIDataList& operator<<(const double value)
    {
        Add(value);
        return *this;
    }
    inline NFIDataList& operator<<(const char* value)
    {
        Add(value);
        return *this;
    }
    inline NFIDataList& operator<<(const std::string& value)
    {
        Add(value);
        return *this;
    }

    inline NFIDataList& operator<<(const NFINT64& value)
    {
        Add(value);
        return *this;
    }
    inline NFIDataList& operator<<(const int value)
    {
        Add((NFINT64)value);
        return *this;
    }
    inline NFIDataList& operator<<(const NFGUID& value)
    {
        Add(value);
        return *this;
    }
    inline NFIDataList& operator<<(const Point3D& value)
    {
        Add(value);
        return *this;
    }

    inline NFIDataList& operator<<(const NFIDataList& value)
    {
        Concat(value);
        return *this;
    }
    enum { STACK_SIZE = 8 };

protected:
    int mnUseSize;
    std::vector< NF_SHARE_PTR<TData> > mvList;
    std::map<std::string, NF_SHARE_PTR<TData> > mxMap;
};

inline NFIDataList::~NFIDataList() {}

const static NFIDataList::TData NULL_TDATA = NFIDataList::TData();

#endif