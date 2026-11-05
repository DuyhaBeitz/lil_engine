get_filename_component(_lil_engine_root "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

if(WIN32)
    set(_lil_engine_system_libs winmm gdi32 opengl32)
elseif(PLATFORM STREQUAL "Web")
    set(_lil_engine_system_libs)
else()
    find_package(raylib 5.5 EXACT REQUIRED)
    find_package(assimp REQUIRED)
    set(_lil_engine_system_libs X11 Xrandr Xi Xcursor Xinerama GL m pthread dl rt)
endif()

set(_lil_engine_include_dirs
    "${_lil_engine_root}/include"
    "${_lil_engine_root}/include/LilEngine"
    "${_lil_engine_root}/include/LilEditor"
    "${_lil_engine_root}/include/external"
    "${_lil_engine_root}/include/external/raylib/include"
    "${_lil_engine_root}/include/external/JoltPhysics"
    "${_lil_engine_root}/include/external/refl-cpp"
    "${_lil_engine_root}/include/external/imgui"
    "${_lil_engine_root}/include/external/rlImGui-Raylib/include"
    "${_lil_engine_root}/include/external/raylib-gizmo/include"
    "${_lil_engine_root}/include/external/configparser"
)


if (PLATFORM STREQUAL "Web")
    set(lib_folder web-lib)
    set(_lil_engine_archives lil_engine lil_editor imgui raylib-gizmo rlImGui Jolt tinyfiledialogs configparser raylib)
    set(libs "lil_engine::configparser;lil_engine::imgui;lil_engine::raylib;lil_engine::raylib-gizmo;lil_engine::rlImGui;lil_engine::Jolt;lil_engine::tinyfiledialogs;${_lil_engine_system_libs}")
else()
    set(lib_folder lib)
    set(_lil_engine_archives lil_engine lil_editor imgui raylib-gizmo rlImGui Jolt tinyfiledialogs configparser)
    set(libs "lil_engine::configparser;lil_engine::imgui;lil_engine::raylib-gizmo;lil_engine::rlImGui;lil_engine::Jolt;lil_engine::tinyfiledialogs;raylib;${_lil_engine_system_libs}")
endif()


foreach(_lib ${_lil_engine_archives})
    if(NOT TARGET lil_engine::${_lib})
        add_library(lil_engine::${_lib} STATIC IMPORTED)
        set_target_properties(lil_engine::${_lib} PROPERTIES
            IMPORTED_LOCATION "${_lil_engine_root}/${lib_folder}/lib${_lib}.a"
            INTERFACE_INCLUDE_DIRECTORIES "${_lil_engine_root}/include"
        )
    endif()
endforeach()


set_target_properties(lil_engine::lil_engine PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_lil_engine_include_dirs}"
    INTERFACE_COMPILE_DEFINITIONS "JPH_DEBUG_RENDERER"
    INTERFACE_LINK_LIBRARIES "${libs}"
)