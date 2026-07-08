# Raylib
set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/include/external/raylib)

target_link_libraries(${PROJECT_NAME} 
    ${RAYLIB_DIR}/libraylib.a
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${RAYLIB_DIR}"
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${RAYLIB_DIR}/include
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
set(NFD_DIR ${CMAKE_SOURCE_DIR}/include/external/nfd)

target_link_libraries(${PROJECT_NAME} 
    ${NFD_DIR}/libnfd.a
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${NFD_DIR}/include
)