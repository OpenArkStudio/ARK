add_executable(ark_task_example main.cpp)
# Ordinary SDK consumer: no plugin library, generated messages or private headers.
set_property(TARGET ark_task_example PROPERTY LINK_LIBRARIES ark::core)
set_target_properties(ark_task_example PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(ark_task_example PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_BINARY_DIR}")
endforeach()
