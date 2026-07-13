# refl-cpp
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include/external/refl-cpp
)

# raylib
set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/include/external/raylib)

target_link_libraries(${PROJECT_NAME} 
    ${CMAKE_SOURCE_DIR}/web/libraylib.a
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${RAYLIB_DIR}"
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${RAYLIB_DIR}/include
)

# raylib-gizmo
add_library(raylib-gizmo STATIC ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo/raygizmo.c)
target_include_directories(raylib-gizmo PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo
    ${RAYLIB_DIR}/include
)

target_include_directories(${PROJECT_NAME}  PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/raylib-gizmo
)

target_link_libraries(${PROJECT_NAME}
    raylib-gizmo
)

# ReactPhysics3D
set(REACTPHYSICS_DIR ${CMAKE_SOURCE_DIR}/include/external/reactphysics3d)

target_link_libraries(${PROJECT_NAME} 
    ${CMAKE_SOURCE_DIR}/web/libreactphysics3d.a
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${REACTPHYSICS_DIR}/include
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -sMAX_WEBGL_VERSION=2 --preload-file assets --shell-file shell.html")

set_target_properties(lil_editor PROPERTIES
    OUTPUT_NAME "index"
    SUFFIX ".html"
)

#native file dialog
# set(NFD_DIR ${CMAKE_SOURCE_DIR}/include/external/nfd)

# target_link_libraries(${PROJECT_NAME} 
#     ${NFD_DIR}/libnfd.a
# )

# target_include_directories(${PROJECT_NAME} PUBLIC 
#     ${NFD_DIR}/include
# )