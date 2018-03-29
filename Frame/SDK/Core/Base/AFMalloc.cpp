#include "AFMalloc.h"

AFMalloc* AFMalloc::m_pMalloc = nullptr;

void AFMalloc::Initialize(AFMalloc* pMalloc)
{
    if(pMalloc == nullptr)
    {
        pMalloc = new AFMalloc();
    }

    AFMalloc::m_pMalloc = pMalloc;
}

AFMalloc* AFMalloc::Instance()
{
    return AFMalloc::m_pMalloc;
}