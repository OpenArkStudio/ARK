// -------------------------------------------------------------------------
//    @FileName         :    AFIPlugin.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFIPlugin
//
// -------------------------------------------------------------------------

#ifndef AFI_PLUGIN_H
#define AFI_PLUGIN_H

#include <iostream>
#include <assert.h>
#include "SDK/Core/AFMap.h"
#include "SDK/Interface/AFIModule.h"
#include "SDK/Interface/AFPlatform.h"
#include "SDK/Interface/AFIPluginManager.h"

#define REGISTER_MODULE(pManager, classBaseName, className)  \
	assert((TIsDerived<classBaseName, AFIModule>::Result));	\
	assert((TIsDerived<className, classBaseName>::Result));	\
	AFIModule* pRegisterModule##className= new className(pManager); \
    pRegisterModule##className->strName = (#className); \
    pManager->AddModule( #classBaseName, pRegisterModule##className );AddElement( #classBaseName, pRegisterModule##className );

#define UNREGISTER_MODULE(pManager, classBaseName, className) AFIModule* pUnRegisterModule##className =  \
       dynamic_cast<AFIModule*>( pManager->FindModule( #classBaseName )); pManager->RemoveModule( #classBaseName ); RemoveElement( #classBaseName ); delete pUnRegisterModule##className;


#define CREATE_PLUGIN(pManager, className)  AFIPlugin* pCreatePlugin##className = new className(pManager); pManager->Registered( pCreatePlugin##className );

#define DESTROY_PLUGIN(pManager, className) pManager->UnRegistered( pManager->FindPlugin((#className)) );

#define GET_CLASS_NAME(className) (#className);

/*
#define REGISTER_COMPONENT(pManager, className)  AFIComponent* pRegisterComponent##className= new className(pManager); \
    pRegisterComponent##className->strName = (#className); \
    pManager->AddComponent( (#className), pRegisterComponent##className );

#define UNREGISTER_COMPONENT(pManager, className) AFIComponent* pRegisterComponent##className =  \
        dynamic_cast<AFIComponent*>( pManager->FindComponent( (#className) ) ); pManager->RemoveComponent( (#className) ); delete pRegisterComponent##className;
*/

class AFIPluginManager;

class AFIPlugin : public AFIModule,
    public NFMap<std::string, AFIModule>
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
        AFIModule* pModule = First();
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
        AFIModule* pModule = First();
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
        AFIModule* pModule = First();
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
        AFIModule* pModule = First();
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
        AFIModule* pModule = First();
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
        AFIModule* pModule = First();
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
