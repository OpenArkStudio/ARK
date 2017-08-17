#include "Tutorial1.h"
#include "HelloWorld1.h"

#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, Tutorial1)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, Tutorial1)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int Tutorial1::GetPluginVersion()
{
    return 0;
}

const std::string Tutorial1::GetPluginName()
{
    return GET_CLASS_NAME(Tutorial1)
}

void Tutorial1::Install()
{
    REGISTER_MODULE(pPluginManager, HelloWorld1, HelloWorld1)
}

void Tutorial1::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, HelloWorld1, HelloWorld1)
}