set(S src/LilEngine)

set(ENGINE_SOURCES
    ${S}/Actor.cpp
    ${S}/Audio.cpp
    ${S}/Character.cpp
    ${S}/Component.cpp
    ${S}/ComponentRef.cpp
    ${S}/GameObject.cpp
    ${S}/Heightmap.cpp
    ${S}/LilEngine.cpp
    ${S}/Physics.cpp
    ${S}/PhysicsDebugRenderer.cpp
    ${S}/SceneManager.cpp
    ${S}/utils
    ${S}/Vehicle.cpp
    ${S}/World.cpp

    ${S}/Components/AnimatedModelComponent.cpp
    ${S}/Components/ColliderComponent.cpp
    ${S}/Components/InstancedModelComponent.cpp
    ${S}/Components/LightComponent.cpp
    ${S}/Components/ModelComponent.cpp

    ${S}/utils/ColliderHelper.cpp
    ${S}/utils/ColorHelper.cpp
    ${S}/utils/FileHelper.cpp
    ${S}/utils/MathHelper.cpp
    ${S}/utils/NameHelper.cpp

    ${S}/Assets/Sound.cpp
    ${S}/Assets/Texture.cpp
)

set(AB ${S}/Backend/Basic)
set(ADVANCED_BACKEND_SOURCES
    ${AB}/AnimationLib.cpp
    ${AB}/AnimationPlayer.cpp
    ${AB}/Environment.cpp
    ${AB}/Environment.cpp
    ${AB}/InstanceBuffer.cpp
    ${AB}/Light.cpp
    ${AB}/Model.cpp
    ${AB}/BackendAccess.cpp
    ${AB}/Frame.cpp
)

add_library(lil_engine STATIC 
${ENGINE_SOURCES}
${ADVANCED_BACKEND_SOURCES}
)
target_include_directories(lil_engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LilEngine
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external
)
