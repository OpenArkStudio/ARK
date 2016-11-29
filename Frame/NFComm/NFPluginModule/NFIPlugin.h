// -------------------------------------------------------------------------
//    @FileName         :    NFIPlugin.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-15
//    @Module           :    NFIPlugin
//
// -------------------------------------------------------------------------

#ifndef NFI_PLUGIN_H
#define NFI_PLUGIN_H

#include <iostream>
#include <assert.h>
#include "NFComm/NFCore/NFMap.h"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

#define REGISTER_MODULE(pManager, classBaseName, className)  \
	assert((TIsDerived<classBaseName, NFIModule>::Result));	\
	assert((TIsDerived<className, classBaseName>::Result));	\
	NFIModule* pRegisterModule##className= new className(pManager); \
    pRegisterModule##className->strName = (#className); \
    pManager->AddModule( #classBaseName, pRegisterModule##className );AddElement( #classBaseName, pRegisterModule##className );

#define UNREGISTER_MODULE(pManager, classBaseName, className) NFIModule* pUnRegisterModule##className =  \
       dynamic_cast<NFIModule*>( pManager->FindModule( #classBaseName )); pManager->RemoveModule( #classBaseName ); RemoveElement( #classBaseName ); delete pUnRegisterModule##className;


#define CREATE_PLUGIN(pManager, className)  NFIPlugin* pCreatePlugin##className = new className(pManager); pManager->Registered( pCreatePlugin##className );

#define DESTROY_PLUGIN(pManager, className) pManager->UnRegistered( pManager->FindPlugin((#className)) );

#define GET_CLASS_NAME(className) (#className);

/*
#define REGISTER_COMPONENT(pManager, className)  NFIComponent* pRegisterComponent##className= new className(pManager); \
    pRegisterComponent##className->strName = (#className); \
    pManager->AddComponent( (#className), pRegisterComponent##className );

#define UNREGISTER_COMPONENT(pManager, className) NFIComponent* pRegisterComponent##className =  \
        dynamic_cast<NFIComponent*>( pManager->FindComponent( (#className) ) ); pManager->RemoveComponent( (#className) ); delete pRegisterComponent##className;
*/

class NFIPluginManager;

class NFIPlugin : public NFIModule,
    public NFMap<std::string, NFIModule>
{

public:

    /**
     * @fn  virtual const int GetPluginVersion() = 0;
     *
     * @brief   Gets plugin version.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The plugin version.
     */

    virtual const int GetPluginVersion() = 0;

    /**
     * @fn  virtual const std::string GetPluginName() = 0;
     *
     * @brief   Gets plugin name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The plugin name.
     */

    virtual const std::string GetPluginName() = 0;

    /**
     * @fn  virtual void Install() = 0;
     *
     * @brief   Installs this plugin.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual void Install() = 0;

    /**
     * @fn  virtual bool Init()
     *
     * @brief   init this plugin.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Init()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
            bool bRet = pModule->Init();
            if (!bRet)
            {
                assert(0);
            }

            pModule = Next();
        }
        return true;
    }

    /**
     * @fn  virtual bool AfterInit()
     *
     * @brief   Determines if we can after init.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AfterInit()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
            bool bRet = pModule->AfterInit();
            if (!bRet)
            {
                assert(0);
            }

            pModule = Next();
        }
        return true;
    }

    /**
     * @fn  virtual bool CheckConfig()
     *
     * @brief   Determines if we can check configuration.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool CheckConfig()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
            pModule->CheckConfig();

            pModule = Next();
        }

        return true;
    }

    /**
     * @fn  virtual bool Execute()
     *
     * @brief   Executes this object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Execute()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
            pModule->Execute();

            pModule = Next();
        }

        return true;
    }

    /**
     * @fn  virtual bool BeforeShut()
     *
     * @brief   Determines if we can before shut.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool BeforeShut()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
            pModule->BeforeShut();

            pModule = Next();
        }
        return true;
    }

    /**
     * @fn  virtual bool Shut()
     *
     * @brief   Shuts this object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Shut()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
            pModule->Shut();

            pModule = Next();
        }

        return true;
    }

    /**
     * @fn  virtual void Uninstall() = 0;
     *
     * @brief   Uninstalls this object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual void Uninstall() = 0;
};

#endif
