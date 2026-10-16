include_directories(${ROOT_DIR}/3rdparty/physX/include)
set(PHYSIC_LIB_DIR ${ROOT_DIR}/3rdparty/physX/lib)

if(WIN32)
    BUILD_PLUGIN_MACRO(AFPhysicPlugin
        debug ${PHYSIC_LIB_DIR}/PxPvdSDKDEBUG_x64.lib
        debug ${PHYSIC_LIB_DIR}/LowLevelDEBUG.lib
        debug ${PHYSIC_LIB_DIR}/LowLevelAABBDEBUG.lib
        debug ${PHYSIC_LIB_DIR}/LowLevelClothDEBUG.lib
        debug ${PHYSIC_LIB_DIR}/LowLevelDynamicsDEBUG.lib
        debug ${PHYSIC_LIB_DIR}/LowLevelParticlesDEBUG.lib
        debug ${PHYSIC_LIB_DIR}/PhysX3CookingDEBUG_x64.lib
        debug ${PHYSIC_LIB_DIR}/PhysX3VehicleDEBUG.lib
        debug ${PHYSIC_LIB_DIR}/PxTaskDEBUG_x64.lib
        debug ${PHYSIC_LIB_DIR}/SceneQueryDEBUG.lib
        debug ${PHYSIC_LIB_DIR}/PhysX3CharacterKinematicDEBUG_x64.lib
        debug ${PHYSIC_LIB_DIR}/PhysX3ExtensionsDEBUG.lib
        debug ${PHYSIC_LIB_DIR}/PxFoundationDEBUG_x64.lib
        debug ${PHYSIC_LIB_DIR}/PhysX3CommonDEBUG_x64.lib
        debug ${PHYSIC_LIB_DIR}/PsFastXmlDEBUG_x64.lib
        debug ${PHYSIC_LIB_DIR}/PhysX3DEBUG_x64.lib
        debug ${PHYSIC_LIB_DIR}/SimulationControllerDEBUG.lib
        
        optimized ${PHYSIC_LIB_DIR}/PxPvdSDK_x64.lib
        optimized ${PHYSIC_LIB_DIR}/LowLevel.lib
        optimized ${PHYSIC_LIB_DIR}/LowLevelAABB.lib
        optimized ${PHYSIC_LIB_DIR}/LowLevelCloth.lib
        optimized ${PHYSIC_LIB_DIR}/LowLevelDynamics.lib
        optimized ${PHYSIC_LIB_DIR}/LowLevelParticles.lib
        optimized ${PHYSIC_LIB_DIR}/PhysX3Cooking_x64.lib
        optimized ${PHYSIC_LIB_DIR}/PhysX3Vehicle.lib
        optimized ${PHYSIC_LIB_DIR}/PxTask_x64.lib
        optimized ${PHYSIC_LIB_DIR}/SceneQuery.lib
        optimized ${PHYSIC_LIB_DIR}/PhysX3CharacterKinematic_x64.lib
        optimized ${PHYSIC_LIB_DIR}/PhysX3Extensions.lib
        optimized ${PHYSIC_LIB_DIR}/PxFoundation_x64.lib
        optimized ${PHYSIC_LIB_DIR}/PhysX3Common_x64.lib
        optimized ${PHYSIC_LIB_DIR}/PsFastXml_x64.lib
        optimized ${PHYSIC_LIB_DIR}/PhysX3_x64.lib
        optimized ${PHYSIC_LIB_DIR}/SimulationController.lib)
else(UNIX)
    BUILD_PLUGIN_MACRO(AFPhysicPlugin
        debug ${PHYSIC_LIB_DIR}/libLowLevelDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libLowLevelAABBDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libLowLevelClothDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libLowLevelDynamicsDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libLowLevelParticlesDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libPhysX3ExtensionsDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libPhysX3VehicleDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libPsFastXmlDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libPxTaskDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libSceneQueryDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libSimulationControllerDEBUG.a
        debug ${PHYSIC_LIB_DIR}/libPhysX3CharacterKinematicDEBUG_x64.so
        debug ${PHYSIC_LIB_DIR}/libPhysX3CommonDEBUG_x64.so
        debug ${PHYSIC_LIB_DIR}/libPhysX3CookingDEBUG_x64.so
        debug ${PHYSIC_LIB_DIR}/libPhysX3DEBUG_x64.so
        debug ${PHYSIC_LIB_DIR}/libPxPvdSDKDEBUG_x64.so
        debug ${PHYSIC_LIB_DIR}/libPxFoundationDEBUG_x64.so

        optimized ${PHYSIC_LIB_DIR}/libLowLevel.a
        optimized ${PHYSIC_LIB_DIR}/libLowLevelAABB.a
        optimized ${PHYSIC_LIB_DIR}/libLowLevelCloth.a
        optimized ${PHYSIC_LIB_DIR}/libLowLevelDynamics.a
        optimized ${PHYSIC_LIB_DIR}/libLowLevelParticles.a
        optimized ${PHYSIC_LIB_DIR}/libPhysX3Extensions.a
        optimized ${PHYSIC_LIB_DIR}/libPhysX3Vehicle.a
        optimized ${PHYSIC_LIB_DIR}/libPsFastXml.a
        optimized ${PHYSIC_LIB_DIR}/libPxTask.a
        optimized ${PHYSIC_LIB_DIR}/libSceneQuery.a
        optimized ${PHYSIC_LIB_DIR}/libSimulationController.a
        optimized ${PHYSIC_LIB_DIR}/libPhysX3CharacterKinematic_x64.so
        optimized ${PHYSIC_LIB_DIR}/libPhysX3Common_x64.so
        optimized ${PHYSIC_LIB_DIR}/libPhysX3Cooking_x64.so
        optimized ${PHYSIC_LIB_DIR}/libPhysX3_x64.so
        optimized ${PHYSIC_LIB_DIR}/libPxPvdSDK_x64.so
        optimized ${PHYSIC_LIB_DIR}/libPxFoundation_x64.so)
endif()

