// -------------------------------------------------------------------------
//    @FileName         :    NFMap.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    切记，不能在循环中自己删除自己
//
// -------------------------------------------------------------------------

#ifndef NF_MAP_H
#define NF_MAP_H

#include <map>
#include <list>
#include <string>
#include <iostream>
#include <typeinfo>
#include "NFMapEx.h"

/**
 * @class   NFMap
 *
 * @brief   A nf map.
 *
 * @author  flyicegood
 * @date    2016/11/22
 *
 * @tparam  T   Generic type parameter.
 * @tparam  TD  Type of the td.
 */

template <typename T , typename TD>
class NFMap
{
public:

    /**
     * @typedef std::map<T, TD*> NFMapOBJECT
     *
     * @brief   Defines an alias representing the nf map object.
     */

    typedef std::map<T, TD*> NFMapOBJECT;

    /**
     * @fn  NFMap::NFMap()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    NFMap() {};

    /**
     * @fn  virtual NFMap::~NFMap()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual ~NFMap()
    {
        //mObjectList.clear();
        //新的NFMap只管理元素，不管理元素所占的内存，谁new 谁delete
        //DeleteAllElement();
    };

    /**
     * @fn  virtual bool NFMap::AddElement(const T& name, TD* data)
     *
     * @brief   Adds an element to 'data'.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           name    The name.
     * @param [in,out]  data    If non-null, the data.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddElement(const T& name, TD* data)
    {
        typename NFMapOBJECT::iterator itr = mObjectList.find(name);
        if (itr == mObjectList.end())
        {
            mObjectList.insert(typename NFMapOBJECT::value_type(name, data));
            // mObjectList[name] = data;
            return true;
        }

        return false;
    }

    /**
     * @fn  virtual TD* NFMap::RemoveElement(const T& name)
     *
     * @brief   Removes the element described by name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   name    The name.
     *
     * @return  Null if it fails, else a pointer to a TD.
     */

    virtual TD* RemoveElement(const T& name)
    {
        TD* pData = NULL;
        typename NFMapOBJECT::iterator itr = mObjectList.find(name);
        if (itr != mObjectList.end())
        {
            pData = itr->second;
            mObjectList.erase(itr);
        }

        return pData;
    }

    /**
     * @fn  virtual TD* NFMap::GetElement(const T& name)
     *
     * @brief   Gets an element.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   name    The name.
     *
     * @return  Null if it fails, else the element.
     */

    virtual TD* GetElement(const T& name)
    {
        typename NFMapOBJECT::iterator itr = mObjectList.find(name);
        if (itr != mObjectList.end())
        {
            return itr->second;
        }
        else
        {
            return NULL;
        }
    }

    /**
     * @fn  virtual TD* NFMap::First()
     *
     * @brief   Gets the first.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  Null if it fails, else a pointer to a TD.
     */

    virtual TD* First()
    {
        if (mObjectList.size() <= 0)
        {
            return NULL;
        }

        mObjectCurIter = mObjectList.begin();
        if (mObjectCurIter != mObjectList.end())
        {
            return mObjectCurIter->second;
        }
        else
        {
            return NULL;
        }
    }

    /**
     * @fn  virtual TD* NFMap::Next()
     *
     * @brief   Move to the next item in the collection.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  Null if it fails, else a pointer to a TD.
     */

    virtual TD* Next()
    {
        if (mObjectCurIter == mObjectList.end())
        {
            return NULL;
        }

        ++mObjectCurIter;
        if (mObjectCurIter != mObjectList.end())
        {
            return mObjectCurIter->second;
        }
        else
        {
            return NULL;
        }
    }

    /**
     * @fn  virtual TD* NFMap::First(T& name)
     *
     * @brief   Firsts the given name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param [in,out]  name    The name.
     *
     * @return  Null if it fails, else a pointer to a TD.
     */

    virtual TD* First(T& name)
    {
        if (mObjectList.size() <= 0)
        {
            return NULL;
        }

        mObjectCurIter = mObjectList.begin();
        if (mObjectCurIter != mObjectList.end())
        {
            name = mObjectCurIter->first;
            return mObjectCurIter->second;
        }
        else
        {
            return NULL;
        }
    }

    /**
     * @fn  virtual TD* NFMap::Next(T& name)
     *
     * @brief   Move to the next item in the collection.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param [in,out]  name    The name.
     *
     * @return  Null if it fails, else a pointer to a TD.
     */

    virtual TD* Next(T& name)
    {
        if (mObjectCurIter == mObjectList.end())
        {
            return NULL;
        }

        mObjectCurIter++;
        if (mObjectCurIter != mObjectList.end())
        {
            name = mObjectCurIter->first;
            return mObjectCurIter->second;
        }
        else
        {
            return NULL;
        }
    }

    /**
     * @fn  int NFMap::Count()
     *
     * @brief   Gets the count.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  An int.
     */

    int Count()
    {
        return mObjectList.size();
    }

    /**
     * @fn  bool NFMap::ClearAll()
     *
     * @brief   Clears all.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    bool ClearAll()
    {
        mObjectList.clear();
        return true;
    }
private:
    /** @brief   List of objects. */
    NFMapOBJECT     mObjectList;

    /**
     * @property    typename NFMapOBJECT::iterator mObjectCurIter
     *
     * @brief   Gets the object current iterator.
     *
     * @return  The m object current iterator.
     */

    typename NFMapOBJECT::iterator mObjectCurIter;
};

#endif
