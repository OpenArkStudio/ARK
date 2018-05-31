/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#pragma once

#include "AFPlatform.hpp"

class AFMemPool
{
    struct MemUnit
    {
        struct MemUnit* pPrev;
        struct MemUnit* pNext;
    };

public:
    AFMemPool(uint32_t unit_num = 50, uint32_t unit_size = 1024) :
        m_pAllocatedMemBlock(nullptr),
        m_pFreeMemBlock(nullptr),
        m_pMemBlock(nullptr),
        mnUnitSize(unit_size),
        mnBlockSize(unit_num * (unit_size + sizeof(struct MemUnit)))
    {
        m_pMemBlock = ARK_NEW char[mnBlockSize];

        if (m_pMemBlock != nullptr)
        {
            for (uint32_t i = 0; i < unit_num; ++i)
            {
                struct MemUnit* pCurrentUnit = (struct MemUnit*)((char*)m_pMemBlock + i * (unit_size + sizeof(struct MemUnit)));
                pCurrentUnit->pPrev = nullptr;
                pCurrentUnit->pNext = m_pFreeMemBlock;

                if (m_pFreeMemBlock != nullptr)
                {
                    m_pFreeMemBlock->pPrev = pCurrentUnit;
                }

                m_pFreeMemBlock = pCurrentUnit;
            }
        }
    }

    ~AFMemPool()
    {
        ::free(m_pMemBlock);
    }

    void* Alloc(uint32_t size, bool use_pool = true)
    {
        if (size > mnUnitSize ||
            use_pool == false ||
            m_pMemBlock == nullptr ||
            m_pFreeMemBlock == nullptr)
        {
            return ARK_NEW char[size];
        }

        //Now FreeList is not empty
        struct MemUnit* pCurrentUnit = m_pFreeMemBlock;

        m_pFreeMemBlock = pCurrentUnit->pNext;
        if (m_pFreeMemBlock != nullptr)
        {
            m_pFreeMemBlock->pPrev = nullptr;
        }

        pCurrentUnit->pNext = m_pAllocatedMemBlock;

        if (m_pAllocatedMemBlock != nullptr)
        {
            m_pAllocatedMemBlock->pPrev = pCurrentUnit;
        }

        m_pAllocatedMemBlock = pCurrentUnit;

        return (void*)((char*)pCurrentUnit + sizeof(struct MemUnit));
    }

    void Free(void* p)
    {
        if (m_pMemBlock < p && p < (void*)((unsigned char*)m_pMemBlock + mnBlockSize))
        {
            struct MemUnit* pCurrentUnit = (struct MemUnit*)((char*)p - sizeof(struct MemUnit));
            m_pAllocatedMemBlock = pCurrentUnit->pNext;
            if (m_pAllocatedMemBlock != nullptr)
            {
                m_pAllocatedMemBlock->pPrev = nullptr;
            }

            pCurrentUnit->pNext = m_pFreeMemBlock;
            if (m_pFreeMemBlock != nullptr)
            {
                m_pFreeMemBlock->pPrev = pCurrentUnit;
            }

            m_pFreeMemBlock = pCurrentUnit;
        }
        else
        {
            ARK_DELETE_ARRAY(p);
        }
    }

    bool InPool(void* p)
    {
        if (m_pMemBlock < p && p < (void*)((unsigned char*)m_pMemBlock + mnBlockSize))
        {
            return true;
        }

        return false;
    }

private:
    struct MemUnit* m_pAllocatedMemBlock;
    struct MemUnit* m_pFreeMemBlock;

public:
    void* m_pMemBlock;
    uint32_t mnUnitSize;
    uint32_t mnBlockSize;
};