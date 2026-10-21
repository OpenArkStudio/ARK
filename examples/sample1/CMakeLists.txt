BUILD_PLUGIN_MACRO("sample1_plugin")
set_target_properties(sample1_plugin PROPERTIES
    CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN YES)
