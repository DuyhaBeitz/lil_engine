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
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_DIR})

target_link_libraries(${PROJECT_NAME}
    imgui
)

# refl-cpp
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/refl-cpp
)

# raylib
set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/include/external/raylib)
set(RAYLIB_LIB ${CMAKE_SOURCE_DIR}/web/libraylib.a)

target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIB})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${RAYLIB_DIR}"
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${RAYLIB_DIR}/include
)

#raylib-gizmo
add_library(raylib-gizmo STATIC ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo/raygizmo.c)
target_include_directories(raylib-gizmo PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo/include
    ${RAYLIB_DIR}/include
)

target_include_directories(${PROJECT_NAME}  PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo/include
)

target_link_libraries(${PROJECT_NAME}
    raylib-gizmo
)

#rlImGui-Raylib
add_library(rlImGui STATIC ${CMAKE_SOURCE_DIR}/include/external/rlImGui-Raylib/rlImGui.cpp)
target_include_directories(rlImGui PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/rlImGui-Raylib/include
    ${RAYLIB_DIR}/include
    ${IMGUI_DIR}
)

target_link_libraries(rlImGui ${RAYLIB_LIB} imgui)

target_include_directories(${PROJECT_NAME}  PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/rlImGui-Raylib/include
)

target_link_libraries(${PROJECT_NAME}
    rlImGui
)

# ReactPhysics3D
set(REACTPHYSICS_DIR ${CMAKE_SOURCE_DIR}/include/external/reactphysics3d)

target_link_libraries(${PROJECT_NAME} 
    ${CMAKE_SOURCE_DIR}/web/libreactphysics3d.a
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${REACTPHYSICS_DIR}/include
)

# tinyfiledialogs
set(TFD_DIR ${CMAKE_SOURCE_DIR}/include/external/tinyfiledialogs)
add_library(tinyfiledialogs ${TFD_DIR}/tinyfiledialogs.c)
target_include_directories(tinyfiledialogs PUBLIC 
    ${TFD_DIR}
)

target_link_libraries(${PROJECT_NAME} 
    tinyfiledialogs
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -sMAX_WEBGL_VERSION=2 --preload-file assets --preload-file imgui.ini --shell-file shell.html")

set_target_properties(game PROPERTIES
    OUTPUT_NAME "index"
    SUFFIX ".html"
)