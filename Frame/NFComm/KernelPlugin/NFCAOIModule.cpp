#include "NFCAOIModule.h"

NFCAOIModule::NFCAOIModule()
{

}

NFCAOIModule::~NFCAOIModule()
{
    ObjectMap::iterator tmpIter;
    for(ObjectMap::iterator iter = objectMap.begin(); iter != objectMap.end();)
    {
        tmpIter = iter;
        ++iter;
        delete tmpIter->second;
    }
}

void NFCAOIModule::Add(const NFGUID& self, const Point3D& xPos, int nRadius /*= DEFAULT_RADIUS*/)
{
    if(objectMap.find(self) != objectMap.end())
    {
        return;
    }

    AOIObject* pObject = new AOIObject(self, xPos, nRadius);
    objectMap[pObject->id] = pObject;

    ObjectList::iterator itIndex;
    bool bFlag = false;

    //x轴
    ObjectMap xTempMap;
    for(ObjectList::iterator iter = xObjectList.begin(); iter != xObjectList.end(); ++iter)
    {
        int differ = (*iter)->x - pObject->x;
        if(abs(differ) <= nRadius)
        {
            xTempMap[(*iter)->id] = *iter;
        }

        if(!bFlag && differ > 0)
        {
            itIndex = iter;
            bFlag = true;
        }

        if(differ > nRadius)
        {
            break;
        }
    }

    if(bFlag)
    {
        //找到了，插入进去
        xObjectList.insert(itIndex, pObject);
    }
    else
    {
        //没找到，尾部加进去
        xObjectList.push_back(pObject);
        pObject->x_pos = itIndex--;
    }

    //y轴
    bFlag = false;
    for(ObjectList::iterator iter = yObjectList.begin(); iter != yObjectList.end();)
    {
        int differ = (*iter)->y - pObject->y;
        if(abs(differ) <= nRadius && xTempMap.find((*iter)->id) != xTempMap.end())
        {
            enterMap[(*iter)->id] = *iter;
        }

        if(!bFlag && differ > 0)
        {
            itIndex = iter;
            bFlag = true;
        }

        if(differ > nRadius)
        {
            break;
        }
    }

    if(bFlag)
    {
        yObjectList.insert(itIndex, pObject);
        pObject->y_pos = --itIndex;
    }
    else
    {
        yObjectList.push_back(pObject);
        pObject->y_pos = yObjectList.begin();
    }

    Update(pObject);
}

void NFCAOIModule::Move(const NFGUID& self, const Point3D& xPos)
{
    ObjectMap::iterator iter = objectMap.find(self);
    if(iter == objectMap.end())
    {
        return;
    }

    AOIObject* pObject = iter->second;
    ObjectList::iterator list_iter;
    ObjectMap oldMap;
    ObjectMap newMap;

    //Get the old range data
    GetRange(pObject, &oldMap);

    //Update position
    UpdateObejctPos(pObject, xPos);

    //Get the new range data
    GetRange(pObject, &newMap);

    //move data = Old MIX New
    for(ObjectMap::iterator iter = oldMap.begin(); iter != oldMap.end(); ++iter)
    {
        if(newMap.find(iter->first) != newMap.end())
        {
            moveMap[iter->first] = iter->second;
        }
    }

    //leave data = Old SUB Move
    for(ObjectMap::iterator iter = oldMap.begin(); iter != oldMap.end(); ++iter)
    {
        if(moveMap.find(iter->first) == moveMap.end())
        {
            leaveMap[iter->first] = iter->second;
        }
    }

    //enter data = New SUB move data
    for(ObjectMap::iterator iter = newMap.begin(); iter != newMap.end(); ++iter)
    {
        if(moveMap.find(iter->first) == moveMap.end())
        {
            enterMap[iter->first] = iter->second;
        }
    }

    Update(pObject);
}

void NFCAOIModule::Leave(const NFGUID& self)
{
    ObjectMap::iterator iter = objectMap.find(self);
    if(iter == objectMap.end())
    {
        return;
    }

    AOIObject* pObject = iter->second;

    GetRange(pObject, &leaveMap);
    Update(pObject);

    xObjectList.erase(pObject->x_pos);
    yObjectList.erase(pObject->y_pos);
    objectMap.erase(pObject->id);
    delete pObject;
}

void NFCAOIModule::Update(AOIObject* pObject)
{
    //call enter/leave/move callbacks
    //send enter msg to pObject
    {
        //DoEnterEvent(pObject->id, enterMap);
    }
    //send leave msg to pObject
    {
        //DoLeaveEvent(pObject->id, leaveMap);
    }
    //send move msg to pObject
    {
        //DoMoveEvent(pObject->id, moveMap);
    }

    enterMap.clear();
    leaveMap.clear();
    moveMap.clear();
}

void NFCAOIModule::GetRange(AOIObject* pObject, ObjectMap* pMap)
{
    ObjectMap xTempMap;
    ObjectList::iterator iter;
    int differ = pObject->radius;

    //x轴 向后找
    if(pObject->x_pos != xObjectList.end())
    {
        iter = pObject->x_pos;
        while(true)
        {
            ++iter;
            if(iter == xObjectList.end())
            {
                break;
            }

            if(pObject->x - (*iter)->x > differ)
            {
                break;
            }

            xTempMap[(*iter)->id] = *iter;
        }
    }

    //x轴 向前找
    if(pObject->x_pos != xObjectList.begin())
    {
        iter = pObject->x_pos;
        while(true)
        {
            --iter;
            if(((*iter)->x - pObject->x) > differ)
            {
                break;
            }

            xTempMap[(*iter)->id] = *iter;

            if(iter == xObjectList.begin())
            {
                break;
            }
        }
    }

    //y轴 向后找
    if(pObject->y_pos != yObjectList.end())
    {
        iter = pObject->y_pos;
        while(true)
        {
            ++iter;
            if(iter == yObjectList.end())
            {
                break;
            }

            if(pObject->y - (*iter)->y > differ)
            {
                break;
            }

            if(xTempMap.find((*iter)->id) != xTempMap.end())
            {
                (*pMap)[(*iter)->id] = *iter;
            }
        }
    }

    //y轴 向前找
    if(pObject->y_pos != yObjectList.begin())
    {
        iter = pObject->y_pos;
        while(true)
        {
            --iter;
            if(((*iter)->y - pObject->y) > differ)
            {
                break;
            }

            if(xTempMap.find((*iter)->id) != xTempMap.end())
            {
                (*pMap)[(*iter)->id] = *iter;
            }

            if(iter == yObjectList.begin())
            {
                break;
            }
        }
    }
}

void NFCAOIModule::UpdateObejctPos(AOIObject* pObject, const Point3D& xNewPos)
{
    int oldX = pObject->x;
    int OldY = pObject->y;

    pObject->x = (int)xNewPos.x;
    pObject->x = (int)xNewPos.x;

    ObjectList::iterator iter;
    ObjectList::iterator itIndex;

    //x轴找新的位置
    if(pObject->x > oldX)
    {
        if(pObject->x_pos != xObjectList.end())
        {
            iter = pObject->x_pos;
            ++iter;
            xObjectList.erase(pObject->x_pos);
            while(iter != xObjectList.end())
            {
                if(pObject->x - (*iter)->x < 0)
                {
                    itIndex = iter;
                    break;
                }

                ++iter;
            }

            if(iter != xObjectList.end())
            {
                xObjectList.insert(itIndex, pObject);
                pObject->x_pos = --itIndex;
            }
            else
            {
                xObjectList.push_back(pObject);
                pObject->x_pos = --xObjectList.end();
            }
        }
    }
    else if(pObject->x < oldX)
    {
        if(pObject->x_pos != xObjectList.begin())
        {
            iter = pObject->x_pos;
            --iter;
            xObjectList.erase(pObject->x_pos);
            while(iter != xObjectList.begin())
            {
                if(pObject->x - (*iter)->x > 0)
                {
                    itIndex = ++iter;
                    break;
                }

                --iter;
            }

            if(iter != xObjectList.begin())
            {
                xObjectList.insert(itIndex, pObject);
                pObject->x_pos = --itIndex;
            }
            else
            {
                xObjectList.push_front(pObject);
                pObject->x_pos = xObjectList.begin();
            }
        }
    }

    //y轴找新的位置
    if(pObject->y > OldY)
    {
        if(pObject->y_pos != yObjectList.end())
        {
            iter = pObject->y_pos;
            ++iter;
            yObjectList.erase(pObject->y_pos);
            while(iter != yObjectList.end())
            {
                if(pObject->y - (*iter)->y < 0)
                {
                    itIndex = iter;
                    break;
                }
                ++iter;
            }
            if(iter != yObjectList.end())
            {
                yObjectList.insert(itIndex, pObject);
                pObject->y_pos = --itIndex;
            }
            else
            {
                yObjectList.push_back(pObject);
                pObject->y_pos = --yObjectList.end();
            }
        }
    }
    else if(pObject->y < OldY)
    {
        if(pObject->y_pos != yObjectList.begin())
        {
            iter = pObject->y_pos;
            --iter;
            yObjectList.erase(pObject->y_pos);
            while(iter != yObjectList.begin())
            {
                if(pObject->y - (*iter)->y > 0)
                {
                    itIndex = ++iter;
                    break;
                }
                --iter;
            }
            if(iter != yObjectList.begin())
            {
                yObjectList.insert(itIndex, pObject);
                pObject->y_pos = --itIndex;
            }
            else
            {
                yObjectList.push_front(pObject);
                pObject->y_pos = yObjectList.begin();
            }
        }
    }
}