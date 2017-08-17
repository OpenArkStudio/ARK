#include "Tutorial2.h"
#include "HelloWorld2.h"

#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, Tutorial2)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, Tutorial2)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int Tutorial2::GetPluginVersion()
{
    return 0;
}

const std::string Tutorial2::GetPluginName()
{
    return GET_CLASS_NAME(Tutorial2)
}

void Tutorial2::Install()
{
    REGISTER_MODULE(pPluginManager, HelloWorld2, HelloWorld2)
    
}

void Tutorial2::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, HelloWorld2, HelloWorld2)
}