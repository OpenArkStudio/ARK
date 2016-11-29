// -------------------------------------------------------------------------
//    @FileName         :    NFIClassModule.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-15
//    @Module           :    NFIClassModule
//
// -------------------------------------------------------------------------

#ifndef NFI_LOGICCLASS_MODULE_H
#define NFI_LOGICCLASS_MODULE_H

#include <iostream>
#include "NFIModule.h"
#include "NFComm/NFCore/NFIPropertyManager.h"
#include "NFComm/NFCore/NFIRecordManager.h"
#include "NFComm/NFCore/NFIComponentManager.h"

class NFIClass
    : public NFList<std::string>//include files
{
public:
    virtual ~NFIClass() {}

    /**
     * @fn  virtual NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager() = 0;
     *
     * @brief   Gets property manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The property manager.
     */

    virtual NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager() = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIRecordManager> GetRecordManager() = 0;
     *
     * @brief   Gets record manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The record manager.
     */

    virtual NF_SHARE_PTR<NFIRecordManager> GetRecordManager() = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIComponentManager> GetComponentManager() = 0;
     *
     * @brief   Gets component manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The component manager.
     */

    virtual NF_SHARE_PTR<NFIComponentManager> GetComponentManager() = 0;

    /**
     * @fn  virtual void SetParent(NF_SHARE_PTR<NFIClass> pClass) = 0;
     *
     * @brief   Sets a parent logic class.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   pClass  The class.
     */

    virtual void SetParent(NF_SHARE_PTR<NFIClass> pClass) = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIClass> GetParent() = 0;
     *
     * @brief   Gets the parent logic class of this class.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The parent.
     */

    virtual NF_SHARE_PTR<NFIClass> GetParent() = 0;

    /**
     * @fn  virtual void SetTypeName(const char* strType) = 0;
     *
     * @brief   Sets type name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strType The type.
     */

    virtual void SetTypeName(const char* strType) = 0;

    /**
     * @fn  virtual const std::string& GetTypeName() = 0;
     *
     * @brief   Gets type name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The type name.
     */

    virtual const std::string& GetTypeName() = 0;

    /**
     * @fn  virtual const std::string& GetClassName() = 0;
     *
     * @brief   Gets class name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The class name.
     */

    virtual const std::string& GetClassName() = 0;

    /**
     * @fn  virtual const bool AddConfigName(std::string& strConfigName) = 0;
     *
     * @brief   Adds a configuration name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param [in,out]  strConfigName   Name of the configuration.
     *
     * @return  A const bool.
     */

    virtual const bool AddConfigName(std::string& strConfigName) = 0;

    /**
     * @fn  virtual NFList<std::string>& GetConfigNameList() = 0;
     *
     * @brief   Gets Instance name list of this class.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The configuration name list.
     */

    virtual NFList<std::string>& GetConfigNameList() = 0;

    /**
     * @fn  virtual void SetInstancePath(const std::string& strPath) = 0;
     *
     * @brief   Sets instance path.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPath Full pathname of the file.
     */

    virtual void SetInstancePath(const std::string& strPath) = 0;

    /**
     * @fn  virtual const std::string& GetInstancePath() = 0;
     *
     * @brief   Gets instance path.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The instance path.
     */

    virtual const std::string& GetInstancePath() = 0;

    /**
     * @fn  virtual bool AddClassCallBack(const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;
     *
     * @brief   Adds the class call back.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   cb  The cb.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddClassCallBack(const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;

    /**
     * @fn  virtual bool DoEvent(const NFGUID& objectID, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& valueList) = 0;
     *
     * @brief   Executes the event operation.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   objectID    Identifier for the object.
     * @param   eClassEvent The class event.
     * @param   valueList   List of values.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool DoEvent(const NFGUID& objectID, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& valueList) = 0;
};

class NFIClassModule
    : public NFIModule,
  public NFMapEx<std::string, NFIClass>
{
public:
    virtual ~NFIClassModule() {}

    /**
     * @fn  virtual bool Load() = 0;
     *
     * @brief   Loads this Logic class Property.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Load() = 0;

    /**
     * @fn  virtual bool Save() = 0;
     *
     * @brief   Saves this object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Save() = 0;

    /**
     * @fn  virtual bool Clear() = 0;
     *
     * @brief   Clears this object to its blank/initial state.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Clear() = 0;

    /**
     * @fn  template<typename BaseType> bool AddClassCallBack(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const NFIDataList&))
     *
     * @brief   Adds the class call back.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  BaseType    Type of the base type.
     * @param           strClassName    Name of the class.
     * @param [in,out]  pBase           If non-null, the base.
     * @param [in,out]  handler         If non-null, the handler.
     *
     * @return  True if it succeeds, false if it fails.
     */

    template<typename BaseType>
    bool AddClassCallBack(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const NFIDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        CLASS_EVENT_FUNCTOR_PTR functorPtr(new CLASS_EVENT_FUNCTOR(functor));
        return AddClassCallBack(strClassName, functorPtr);
    }

    /**
     * @fn  virtual bool DoEvent(const NFGUID& objectID, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& valueList) = 0;
     *
     * @brief   Executes the event operation.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   objectID        Identifier for the object.
     * @param   strClassName    Name of the class.
     * @param   eClassEvent     The class event.
     * @param   valueList       List of values.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool DoEvent(const NFGUID& objectID, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& valueList) = 0;

    /**
     * @fn  virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;
     *
     * @brief   Adds the class call back to 'cb'.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strClassName    Name of the class.
     * @param   cb              The cb.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIPropertyManager> GetClassPropertyManager(const std::string& strClassName) = 0;
     *
     * @brief   Gets class property manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strClassName    Name of the class.
     *
     * @return  The class property manager.
     */

    virtual NF_SHARE_PTR<NFIPropertyManager> GetClassPropertyManager(const std::string& strClassName) = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIRecordManager> GetClassRecordManager(const std::string& strClassName) = 0;
     *
     * @brief   Gets class record manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strClassName    Name of the class.
     *
     * @return  The class record manager.
     */

    virtual NF_SHARE_PTR<NFIRecordManager> GetClassRecordManager(const std::string& strClassName) = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIComponentManager> GetClassComponentManager(const std::string& strClassName) = 0;
     *
     * @brief   Gets class component manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strClassName    Name of the class.
     *
     * @return  The class component manager.
     */

    virtual NF_SHARE_PTR<NFIComponentManager> GetClassComponentManager(const std::string& strClassName) = 0;
};

#endif