cmake_minimum_required(VERSION 3.20)
project(lil_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
option(PLATFORM "platform (Desktop/Web)" Desktop)

# -------------------------------
# Engine Library
# -------------------------------
file(GLOB_RECURSE ENGINE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/lil_engine/*.cpp
)
add_library(${PROJECT_NAME} STATIC ${ENGINE_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lil_engine
)

# -------------------------------
# Editor Executable
# -------------------------------
file(GLOB_RECURSE EDITOR_SOURCES ${CMAKE_SOURCE_DIR}/src/editor/*.cpp)

add_executable(lil_editor ${EDITOR_SOURCES})
target_link_libraries(lil_editor PRIVATE
    ${PROJECT_NAME}
)
target_include_directories(lil_editor PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/editor
)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PLATFORM}.cmake)