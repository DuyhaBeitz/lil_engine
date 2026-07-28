#imgui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/include/external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR})
target_include_directories(lil_engine PUBLIC ${IMGUI_DIR})
target_link_libraries(lil_engine PUBLIC imgui)

# refl-cpp
target_include_directories(lil_engine PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/refl-cpp
)

# raylib
set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/include/external/raylib)

target_link_libraries(lil_engine PUBLIC
    ${RAYLIB_DIR}/libraylib.a
    X11
    Xrandr
    Xi
    Xcursor
    Xinerama
    GL
    m
    pthread
    dl
    rt
)

target_include_directories(lil_engine PUBLIC 
    ${RAYLIB_DIR}/include
)

#raylib-gizmo
add_library(raylib-gizmo STATIC ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo/raygizmo.c)
target_include_directories(raylib-gizmo PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo/include
    ${RAYLIB_DIR}/include
)

target_include_directories(lil_engine  PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo/include
)

target_link_libraries(lil_engine PUBLIC
    raylib-gizmo
)

#rlImGui-Raylib
add_library(rlImGui STATIC ${CMAKE_SOURCE_DIR}/include/external/rlImGui-Raylib/rlImGui.cpp)
target_include_directories(rlImGui PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/rlImGui-Raylib/include
    ${RAYLIB_DIR}/include
    ${IMGUI_DIR}
)

target_link_libraries(rlImGui PUBLIC
    ${RAYLIB_DIR}/libraylib.a imgui
)

target_include_directories(lil_engine PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/rlImGui-Raylib/include
)

target_link_libraries(lil_engine PUBLIC
    rlImGui
)

# ReactPhysics3D
# set(REACTPHYSICS_DIR ${CMAKE_SOURCE_DIR}/include/external/reactphysics3d)

# target_link_libraries(lil_engine 
#     ${REACTPHYSICS_DIR}/libreactphysics3d.a
# )

# target_include_directories(lil_engine PUBLIC 
#     ${REACTPHYSICS_DIR}/include
# )

# JoltPhysics
set(TARGET_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(TARGET_HELLO_WORLD OFF CACHE BOOL "" FORCE)
set(TARGET_PERFORMANCE_TEST OFF CACHE BOOL "" FORCE)
set(TARGET_SAMPLES OFF CACHE BOOL "" FORCE)
set(TARGET_VIEWER OFF CACHE BOOL "" FORCE)

set(DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)
set(GENERATE_DEBUG_SYMBOLS ON CACHE BOOL "" FORCE)
set(OVERRIDE_CXX_FLAGS ON CACHE BOOL "" FORCE)
set(CROSS_PLATFORM_DETERMINISTIC OFF CACHE BOOL "" FORCE)
set(INTERPROCEDURAL_OPTIMIZATION ON CACHE BOOL "" FORCE)
set(FLOATING_POINT_EXCEPTIONS_ENABLED OFF CACHE BOOL "" FORCE)
set(CPP_EXCEPTIONS_ENABLED OFF CACHE BOOL "" FORCE)
set(CPP_RTTI_ENABLED ON CACHE BOOL "" FORCE)
set(OBJECT_LAYER_BITS 16 CACHE BOOL "" FORCE)

set(DEBUG_RENDERER_IN_DEBUG_AND_RELEASE ON CACHE BOOL "" FORCE)

set(JOLT_DIR ${CMAKE_SOURCE_DIR}/include/external/JoltPhysics)
add_subdirectory(${JOLT_DIR}/Build ${CMAKE_BINARY_DIR}/JoltBuild EXCLUDE_FROM_ALL)
target_include_directories(lil_engine PUBLIC 
    ${JOLT_DIR}
)
target_link_libraries(lil_engine PUBLIC Jolt)
target_compile_definitions(lil_engine PUBLIC JPH_DEBUG_RENDERER)

# tinyfiledialogs
set(TFD_DIR ${CMAKE_SOURCE_DIR}/include/external/tinyfiledialogs)
add_library(tinyfiledialogs ${TFD_DIR}/tinyfiledialogs.c)
target_include_directories(tinyfiledialogs PUBLIC 
    TFD_DIR
)

target_link_libraries(lil_engine PUBLIC
    tinyfiledialogs
)