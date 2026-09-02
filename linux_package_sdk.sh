#!/usr/bin/env bash
set -e

rm -rf lil_engine_sdk
mkdir -p lil_engine_sdk/lib lil_engine_sdk/include

cp build/JoltBuild/libJolt.a         lil_engine_sdk/lib/
cp build/lib/libr3d.a                lil_engine_sdk/lib/
cp build/libtinyfiledialogs.a        lil_engine_sdk/lib/
cp build/libimgui.a                  lil_engine_sdk/lib/
cp build/libraylib-gizmo.a           lil_engine_sdk/lib/
cp build/librlImGui.a                lil_engine_sdk/lib/
cp build/liblil_engine.a             lil_engine_sdk/lib/
cp build/liblil_editor.a             lil_engine_sdk/lib/

if [ -d "include" ]; then
    cp -R include/* lil_engine_sdk/include/
fi

cp cmake/lil_engine.cmake lil_engine_sdk/
cp imgui.ini lil_engine_sdk/