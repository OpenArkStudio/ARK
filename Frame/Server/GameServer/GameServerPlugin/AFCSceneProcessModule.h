// -------------------------------------------------------------------------
//    @FileName         :    AFCSceneProcessModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-04-14
//    @Module           :    AFCSceneProcessModule
//
// -------------------------------------------------------------------------

#ifndef AFC_SCENEPROCESS_MODULE_H
#define AFC_SCENEPROCESS_MODULE_H

#include <string>
#include <map>
#include <iostream>
#include "SDK/Base/AFMap.h"
#include "SDK/Base/AFList.h"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_iterators.hpp"
#include "RapidXML/rapidxml_print.hpp"
#include "RapidXML/rapidxml_utils.hpp"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIGameServerConfigModule.h"
#include "SDK/Interface/AFISceneProcessModule.h"
#include "SDK/Interface/AFIPropertyModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Proto/NFProtocolDefine.hpp"
#include "SDK/Interface/AFIGameServerNet_ServerModule.h"

class AFCSceneProcessModule
    : public AFISceneProcessModule
{
public:
    AFCSceneProcessModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCSceneProcessModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual E_SCENE_TYPE GetCloneSceneType(const int nSceneID);
    virtual bool IsCloneScene(const int nSceneID);
    virtual bool ApplyCloneGroup(const int nSceneID, int& nGroupID);
    virtual bool ExitCloneGroup(const int nSceneID, const int& nGroupID);

protected:
    int CreateCloneScene(const int& nSceneID);

    bool CreateSceneObject(const int nSceneID, const int nGroupID);

    bool LoadSceneResource(const int nSceneID);

protected:

    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var);

    int OnEnterSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);
    int OnLeaveSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

protected:
    void OnClienSwapSceneProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

private:

    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFIGameServerNet_ServerModule* m_pGameServerNet_ServerModule;
    //////////////////////////////////////////////////////////////////////////
    struct SceneSeedResource
    {
        std::string strSeedID;
        std::string strConfigID;
        float fSeedX;
        float fSeedY;
        float fSeedZ;
    };

    //SceneID,(SeedID,SeedData)
    AFMapEx<int, AFMapEx<std::string, SceneSeedResource>> mtSceneResourceConfig;

    //////////////////////////////////////////////////////////////////////////
};

#endif
