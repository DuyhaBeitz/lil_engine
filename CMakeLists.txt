cmake_minimum_required(VERSION 3.20)
project(lil_engine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
option(PLATFORM "platform (Desktop/Web)" Desktop)

# -------------------------------
# Engine Library
# -------------------------------
file(GLOB_RECURSE ENGINE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/LilEngine/*.cpp
)
add_library(lil_engine STATIC ${ENGINE_SOURCES})
target_include_directories(lil_engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LilEngine
)

# -------------------------------
# Editor Library
# -------------------------------
file(GLOB_RECURSE EDITOR_SOURCES ${CMAKE_SOURCE_DIR}/src/LilEditor/*.cpp)

add_library(lil_editor ${EDITOR_SOURCES})
target_link_libraries(lil_editor PUBLIC
    lil_engine
)
target_include_directories(lil_editor PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LilEditor
)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PLATFORM}.cmake)

# -------------------------------
# Game executable
# -------------------------------
add_executable(game 
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)
target_link_libraries(game PUBLIC
    lil_editor
)
target_include_directories(game PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LilEngine
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LilEditor
)
