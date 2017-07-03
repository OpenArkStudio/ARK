#ifndef NF_AOI_MODULE_H
#define NF_AOI_MODULE_H

#pragma once
#include "NFComm/NFCore/NFGUID.h"
#include "NFComm/NFCore/NFIDataList.h"
#include <map>
#include <list>

#define DEFAULT_RADIUS 20

class NFCAOIModule
{
public:
    NFCAOIModule();
    ~NFCAOIModule();

    void Add(const NFGUID& self, const Point3D& xPos, int nRadius = DEFAULT_RADIUS);
    void Move(const NFGUID& self, const Point3D& xPos);
    void Leave(const NFGUID& self);

    struct AOIObject
    {
        AOIObject(const NFGUID& self, const Point3D& pos, const int nRadius)
        {
            id = self;
            x = (int)pos.x;
            y = (int)pos.z;
            radius = nRadius;
        }

        int x = 0;
        int y = 0;
        NFGUID id = NULL_GUID;
        int radius = 0;
        std::list<AOIObject*>::iterator x_pos;
        std::list<AOIObject*>::iterator y_pos;
    };

private:
    using ObjectMap = std::map<NFGUID, AOIObject*>;
    using ObjectList = std::list<AOIObject*>;

    void Update(AOIObject* pObject);
    void GetRange(AOIObject* pObject, ObjectMap* pMap);
    void UpdateObejctPos(AOIObject* pObject, const Point3D& xNewPos);

private:
    ObjectList xObjectList;
    ObjectList yObjectList;
    ObjectMap  objectMap;

    ObjectMap moveMap;
    ObjectMap enterMap;
    ObjectMap leaveMap;
};

#endif //!NF_AOI_MODULE_H