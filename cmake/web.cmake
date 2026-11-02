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
add_library(raylib_imported STATIC IMPORTED)
set_target_properties(raylib_imported PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/web/libraylib.a"
)
add_library(raylib ALIAS raylib_imported)
target_include_directories(lil_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/raylib/include
)

#raylib-gizmo
add_library(raylib-gizmo STATIC ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo/raygizmo.c)
target_include_directories(raylib-gizmo PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo/include
)
target_include_directories(raylib-gizmo  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/raylib/include
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
    ${IMGUI_DIR}
)
target_include_directories(rlImGui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/raylib/include
)

target_link_libraries(rlImGui PUBLIC
    raylib imgui
)

target_include_directories(lil_engine PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/rlImGui-Raylib/include
)

target_link_libraries(lil_engine PUBLIC
    rlImGui
)

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
#set(USE_ASSERTS ON CACHE BOOL "" FORCE)

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
    ${TFD_DIR}
)

target_link_libraries(lil_engine PUBLIC
    tinyfiledialogs
)

# configparser
set(CONFPARS_DIR ${CMAKE_SOURCE_DIR}/include/external/configparser)
add_library(configparser ${CONFPARS_DIR}/configparser.cpp)
target_include_directories(configparser PUBLIC 
    ${CONFPARS_DIR}
)

target_link_libraries(lil_engine PUBLIC
    configparser
)


target_compile_definitions(lil_engine PUBLIC
    PLATFORM_WEB
)

target_link_options(lil_engine PUBLIC
    "-sUSE_GLFW=3"
    "-sASSERTIONS=1"
    "-sASYNCIFY"
    "-sMAX_WEBGL_VERSION=2"
    "--preload-file"
    "${CMAKE_SOURCE_DIR}/assets@assets"
    "--shell-file"
    "${CMAKE_SOURCE_DIR}/web/shell.html"

    "-sINITIAL_MEMORY=134217728"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sMAXIMUM_MEMORY=1073741824"
)

# set_target_properties(lil_engine PROPERTIES
#     OUTPUT_NAME "index"
#     SUFFIX ".html"
# )