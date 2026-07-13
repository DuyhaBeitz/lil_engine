# refl-cpp
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/refl-cpp
)

# raylib
set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/include/external/raylib)

target_link_libraries(${PROJECT_NAME} 
    ${RAYLIB_DIR}/libraylib.a
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${RAYLIB_DIR}
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${RAYLIB_DIR}/include
)

#raylib-gizmo
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
    ${REACTPHYSICS_DIR}/libreactphysics3d.a
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${REACTPHYSICS_DIR}/include
)

#native file dialog
# set(NFD_DIR ${CMAKE_SOURCE_DIR}/include/external/nfd)

# target_link_libraries(${PROJECT_NAME} 
#     ${NFD_DIR}/libnfd.a
# )

# target_include_directories(${PROJECT_NAME} PUBLIC 
#     ${NFD_DIR}/include
# )