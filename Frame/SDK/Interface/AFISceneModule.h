// -------------------------------------------------------------------------
//    @FileName			:    AFISceneModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFISceneModule
//
// -------------------------------------------------------------------------

#ifndef AFI_CONTAINER_MODULE_H
#define AFI_CONTAINER_MODULE_H

#include <list>
#include <iostream>
#include <algorithm>
#include "SDK/Core/AFList.h"
#include "SDK/Core/AFMap.h"
#include "SDK/Core/AFIObject.h"
#include "SDK/Core/AFGUID.h"

// all object in this group
/*
if a object in the group of '0', them it can be see by all object in this scene.
*/
class AFCSceneGroupInfo
    //: public NFList<AFGUID>
{
public:
    AFCSceneGroupInfo(int nSceneID, int nGroupID)
    {
        mnGroupID = nGroupID;
    }

    AFCSceneGroupInfo(int nSceneID, int nGroupID, int nWidth)
    {
        mnGroupID = nGroupID;
    }

    virtual ~AFCSceneGroupInfo()
    {
    }

    bool Execute()
    {
        return true;
    }

    AFMapEx<AFGUID, int> mxPlayerList;
    AFMapEx<AFGUID, int> mxOtherList;
    int mnGroupID;
};

// all group in this scene
class AFCSceneInfo
    : public AFMapEx<int, AFCSceneGroupInfo>
{
public:

    AFCSceneInfo(int nSceneID)
    {
        mnGroupIndex = 0;
        mnSceneID = nSceneID;
        mnWidth = 512;
    }

    AFCSceneInfo(int nSceneID, int nWidth)
    {
        mnGroupIndex = 0;
        mnSceneID = nSceneID;
        mnWidth = nWidth;
    }

    virtual ~AFCSceneInfo()
    {
        ClearAll();
    }

    int NewGroupID()
    {
        return ++mnGroupIndex;
    }

    int GetWidth()
    {
        return mnWidth;
    }

    bool AddObjectToGroup(const int nGroupID, const AFGUID& ident, bool bPlayer)
    {
        NF_SHARE_PTR<AFCSceneGroupInfo> pInfo = GetElement(nGroupID);
        if (pInfo.get())
        {
            if (bPlayer)
            {
                return pInfo->mxPlayerList.AddElement(ident, NF_SHARE_PTR<int>()); // TODO:Map.second为空，使用的时候千万注意
            }
            else
            {
                return pInfo->mxOtherList.AddElement(ident, NF_SHARE_PTR<int>()); // TODO:Map.second为空，使用的时候千万注意
            }
        }

        return false;
    }

    bool RemoveObjectFromGroup(const int nGroupID, const AFGUID& ident, bool bPlayer)
    {
        NF_SHARE_PTR<AFCSceneGroupInfo> pInfo = GetElement(nGroupID);
        if (pInfo.get())
        {
            if (bPlayer)
            {
                return pInfo->mxPlayerList.RemoveElement(ident);
            }
            else
            {
                return pInfo->mxOtherList.RemoveElement(ident);
            }
        }

        return false;
    }

    bool Execute()
    {
        NF_SHARE_PTR<AFCSceneGroupInfo> pGroupInfo = First();
        while (pGroupInfo.get())
        {
            pGroupInfo->Execute();

            pGroupInfo = Next();
        }
        return true;
    }
protected:
private:
    int mnGroupIndex;
    int mnSceneID;
    int mnWidth;
};

class AFISceneModule
    : public AFIModule,
  public AFMapEx<int, AFCSceneInfo>
{
public:
    virtual ~AFISceneModule()
    {
        ClearAll();
    }

protected:
private:
};
#endif
