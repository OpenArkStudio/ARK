#include "AFCMemManger.h"

#ifdef NF_USE_TCMALLOC
#include "Dependencies/gperftools-2.1/src/gperftools/malloc_extension.h"
#endif

AFCMemManger::AFCMemManger()
{

}

AFCMemManger::~AFCMemManger()
{

}

void AFCMemManger::FreeMem()
{

#ifdef NF_USE_TCMALLOC
    // »ØÊÕÄÚ´æ
    MallocExtension::instance()->ReleaseFreeMemory();
#endif //NF_USE_TCMALLOC

}
