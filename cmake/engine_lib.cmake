set(S src/LilEngine)

set(ENGINE_SOURCES
    ${S}/Actor.cpp
    ${S}/Audio.cpp
    ${S}/Character.cpp
    ${S}/Component.cpp
    ${S}/ComponentRef.cpp
    ${S}/GameObject.cpp
    ${S}/Heightmap.cpp
    ${S}/Input.cpp
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

if(BACKEND STREQUAL "Advanced")
    set(B "${S}/Backend/Advanced")
    set(BACKEND_SOURCES
        ${B}/AnimationLib.cpp
        ${B}/AnimationPlayer.cpp
        ${B}/BackendAccess.cpp
        ${B}/Environment.cpp
        ${B}/Frame.cpp
        ${B}/InstanceBuffer.cpp
        ${B}/Light.cpp
        ${B}/Model.cpp
    )
else()
    set(B "${S}/Backend/Basic")
    set(BACKEND_SOURCES
        ${B}/AnimationLib.cpp
        ${B}/AnimationPlayer.cpp
        ${B}/Environment.cpp
        ${B}/Environment.cpp
        ${B}/InstanceBuffer.cpp
        ${B}/Light.cpp
        ${B}/Model.cpp
        ${B}/BackendAccess.cpp
        ${B}/Frame.cpp
        ${B}/lil3d_impl.c
    )
endif()

add_library(lil_engine STATIC 
${ENGINE_SOURCES}
${BACKEND_SOURCES}
)
target_include_directories(lil_engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LilEngine
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external
)
